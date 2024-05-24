package logging

import (
	"os"
	"time"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func Init() {
	zerolog.TimeFieldFormat = time.RFC3339
	// 创建日志文件
	logFile, err := os.OpenFile("app.log", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		panic(err)
	}

	// 设置日志输出为文件
	zerolog.SetGlobalLevel(zerolog.DebugLevel)
	log.Logger = log.Output(zerolog.ConsoleWriter{
		Out:        logFile,
		TimeFormat: "2006-01-02 15:04:05",
		NoColor:    true,
	})

	// 设置时间格式
	//log.Logger = log.With().Timestamp().Logger()
	//	log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})
}
