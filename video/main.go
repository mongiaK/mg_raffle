/*================================================================
*
*  文件名称：main.go
*  创 建 者: mongia
*  创建日期：2021年05月06日
*
================================================================*/

package main

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
)

type vClass struct {
	TypeID   string `json:"type_id"`
	TypeName string `json:"type_name"`
}

type vSimpleInfo struct {
	VodID       string `json:"vod_id"`
	VodName     string `json:"vod_name"`
	TypeID      string `json:"type_id"`
	TypeName    string `json:"type_name"`
	VodEnName   string `json:"vod_en"`
	VodTime     string `json:"vod_time"`
	VodRemarks  string `json:"vod_remarks"`
	VodPlayFrom string `json:"vod_play_from"`
}

type vDetailInfo struct {
	VodTime       string `json:"vod_time"`
	VodID         string `json:"vod_id"`
	VodName       string `json:"vod_name"`
	VodEnName     string `json:"vod_enname"`
	VodSubName    string `json:"vod_subname"`
	VodLetter     string `json:"vod_letter"`
	VodColor      string `json:"vod_color"`
	VodTag        string `json:"vod_tag"`
	TypeID        string `json:"type_id"`
	TypeName      string `json:"type_name"`
	VodPic        string `json:"vod_pic"`
	VodLang       string `json:"vod_lang"`
	VodArea       string `json:"vod_area"`
	VodYear       string `json:"vod_year"`
	VodRemark     string `json:"vod_remark"`
	VodActor      string `json:"vod_actor"`
	VodDirector   string `json:"vod_director"`
	VodSerial     string `json:"vod_serial"`
	VodLock       string `json:"vod_lock"`
	VodLevel      string `json:"vod_level"`
	VodHits       string `json:"vod_hits"`
	VodHitsDay    string `json:"vod_hits_day"`
	VodHitsWeek   string `json:"vod_hits_week"`
	VodHitsMonth  string `json:"vod_hits_month"`
	VodDuration   string `json:"vod_duration"`
	VodUp         string `json:"vod_up"`
	VodDown       string `json:"vod_down"`
	VodScore      string `json:"vod_score"`
	VodScoreAll   string `json:"vod_socre_all"`
	VodScoreNum   string `json:"vod_score_num"`
	VodPointsPlay string `json:"vod_points_play"`
	VodPointsDown string `json:"vod_points_down"`
	VodContent    string `json:"vod_content"`
	VodPlayFrom   string `json:"vod_play_from"`
	VodPlayNote   string `json:"vod_play_note"`
	VodPlayServer string `json:"vod_play_server"`
	VodPlayUrl    string `json:"vod_play_url"`
	VodDownFrom   string `json:"vod_down_from"`
	VodDownNote   string `json:"vod_down_note"`
	VodDownServer string `json:"vod_down_server"`
	VodDownUrl    string `json:"vod_down_url"`
}

type mhSimpleResponse struct {
	Code      int           `json:"code"`
	Msg       string        `json:"msg"`
	Page      int           `json:"page"`
	PageCount int           `json:"pagecount"`
	Limit     int           `json:"limit"`
	Total     int           `json:"total"`
	List      []vSimpleInfo `json:"list"`
	Class     []vClass      `json:"class"`
}

const (
	videoTypeUrl = "https://www.mhapi123.com/inc/apidownjson.php"
)

func getAllVideoTypes() {
	resp, err := http.Get(videoTypeUrl)
	if nil != err {
		log.Fatalf("get video type failed. %s", err.Error())
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if nil != err {
		log.Fatalf("parse video type failed. %s", err.Error())
	}

	var typeAll mhSimpleResponse
	json.Unmarshal(body, &typeAll)
	log.Printf("body: %+v", typeAll)
}

func getVideoDetails() {

}

func main() {
	getAllVideoTypes()
}
