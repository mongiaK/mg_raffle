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
	"os"
	"os/exec"
	"os/signal"
	"strings"
	"syscall"

	"github.com/aliyun/alibaba-cloud-sdk-go/services/alidns"
	"github.com/robfig/cron/v3"
)

var (
	regionID        = flag.String("regionid", "cn-hangzhou", "aliyun regionid")
	accessKeyID     = flag.String("accesskeyid", "", "aliyun access key id")
	accessKeySecret = flag.String("accesskeysecret", "", "aliyun access key secret")
	domainName      = flag.String("domain", "", "domain name")
	isDaemon        = flag.Bool("daemon", false, "daemon true or not")

	aliyunScheme string = "https"
	preIP        string = ""
)

func GetInternalIP() (string, error) {
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err != nil {
		return "", errors.New("internal IP fetch failed, detail:" + err.Error())
	}
	defer conn.Close()

	res := conn.LocalAddr().String()
	res = strings.Split(res, ":")[0]
	return res, nil
}

func GetExternalIP() (string, error) {
	response, err := http.Get("http://ip.cip.cc")
	if err != nil {
		return "", errors.New("external IP fetch failed, detail:" + err.Error())
	}

	defer response.Body.Close()
	res := ""

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

func aliyunDDNSClient() {
	externalIP, err := GetExternalIP()
	if nil != err {
		fmt.Print(err.Error())
		return
	}

	if preIP == externalIP {
		fmt.Println("ip not change")
		return
	}

	fmt.Printf("external ip [%s]", externalIP)

	client, err := alidns.NewClientWithAccessKey(*regionID, *accessKeyID, *accessKeySecret)
	if nil != err {
		fmt.Print(err.Error())
		return
	}

	request := alidns.CreateDescribeDomainRecordsRequest()
	request.Scheme = aliyunScheme
	request.DomainName = *domainName

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
			subReq.Scheme = aliyunScheme
			subReq.RR = record.RR
			subReq.RecordId = record.RecordId
			subReq.Type = record.Type
			subReq.Value = externalIP

			res, err := client.UpdateDomainRecord(subReq)
			if nil != err {
				fmt.Print(err.Error())
				continue
			}

			preIP = externalIP
			fmt.Printf("update record response: %+v", res)
		}
	}

}

func daemon() {
	if *isDaemon {
		cmd := exec.Command(os.Args[0], flag.Args()...)
		if err := cmd.Start(); err != nil {
			fmt.Printf("start %s failed, error: %v\n", os.Args[0], err)
			os.Exit(1)
		}
		fmt.Printf("%s [PID] %d running...\n", os.Args[0], cmd.Process.Pid)
		os.Exit(0)
	}
}

func main() {
	flag.Parse()

	daemon()

	c := cron.New()

	c.AddFunc("@every 10s", aliyunDDNSClient)

	c.Start()

	sigs := make(chan os.Signal, 1)
	done := make(chan bool, 1)

	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)

	go func() {
		sig := <-sigs
		fmt.Println()
		fmt.Println(sig)
		done <- true
	}()

	fmt.Println("awaiting signal")
	<-done
	fmt.Println("exiting")
}
