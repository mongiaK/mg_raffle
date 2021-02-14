/*================================================================
*
*  文件名称：main.go
*  创 建 者: mongia
*  创建日期：2021年02月14日
*
================================================================*/
package main

import (
	"errors"
	"flag"
	"fmt"
	"io"
	"net"
	"net/http"
	"strings"

	"github.com/aliyun/alibaba-cloud-sdk-go/services/alidns"
)

var (
	RegionID               = flag.String("regionid", "cn-hangzhou", "aliyun regionid")
	AccessKeyID            = flag.String("accesskeyid", "", "aliyun access key id")
	AccessKeySecret        = flag.String("accesskeysecret", "", "aliyun access key secret")
	DomainName             = flag.String("domain", "", "domain name")
	AliyunScheme    string = "https"
)

func GetInternalIP() (string, error) {
	// 思路来自于Python版本的内网IP获取，其他版本不准确
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err != nil {
		return "", errors.New("internal IP fetch failed, detail:" + err.Error())
	}
	defer conn.Close()

	// udp 面向无连接，所以这些东西只在你本地捣鼓
	res := conn.LocalAddr().String()
	res = strings.Split(res, ":")[0]
	return res, nil
}

func GetExternalIP() (string, error) {
	// 有很多类似网站提供这种服务，这是我知道且正在用的
	// 备用：https://myexternalip.com/raw （cip.cc 应该是够快了，我连手机热点的时候不太稳，其他自己查）
	response, err := http.Get("http://ip.cip.cc")
	if err != nil {
		return "", errors.New("external IP fetch failed, detail:" + err.Error())
	}

	defer response.Body.Close()
	res := ""

	// 类似的API应当返回一个纯净的IP地址
	for {
		tmp := make([]byte, 32)
		n, err := response.Body.Read(tmp)
		if err != nil {
			if err != io.EOF {
				return "", errors.New("external IP fetch failed, detail:" + err.Error())
			}
			res += string(tmp[:n])
			break
		}
		res += string(tmp[:n])
	}

	return strings.TrimSpace(res), nil
}

func main() {
	flag.Parse()

	externalIP, err := GetExternalIP()
	if nil != err {
		fmt.Print(err.Error())
		return
	}

	fmt.Printf("external ip [%s]", externalIP)

	client, err := alidns.NewClientWithAccessKey(*RegionID, *AccessKeyID, *AccessKeySecret)
	if nil != err {
		fmt.Print(err.Error())
		return
	}

	request := alidns.CreateDescribeDomainRecordsRequest()
	request.Scheme = AliyunScheme
	request.DomainName = *DomainName

	domainRecords, err := client.DescribeDomainRecords(request)
	if nil != err {
		fmt.Print(err.Error())
		return
	}

	fmt.Printf("Domain list: %+v", domainRecords)
	if domainRecords.TotalCount > 0 {
		for _, record := range domainRecords.DomainRecords.Record {
			if "A" != record.Type {
				continue
			}

			fmt.Printf("update record %+v", record)
			subReq := alidns.CreateUpdateDomainRecordRequest()
			subReq.Scheme = AliyunScheme
			subReq.RR = record.RR
			subReq.RecordId = record.RecordId
			subReq.Type = record.Type
			subReq.Value = externalIP

			res, err := client.UpdateDomainRecord(subReq)
			if nil != err {
				fmt.Print(err.Error())
				continue
			}
			fmt.Printf("update record response: %+v", res)
		}
	}
}
