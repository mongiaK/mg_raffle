package main

import (
	"os"
	"os/signal"
	"syscall"

	"github.com/rs/zerolog/log"

	"rxagent/internal/resource"
	"rxagent/internal/service"
	"rxagent/pkg/config"
	"rxagent/pkg/logging"
)

func main() {
	// 设置进程名称为 "rxagent"
	os.Args[0] = "rxagent"

	worker()

	log.Info().Msg("server start")

	// 监听信号
	signalCh := make(chan os.Signal, 1)
	signal.Notify(signalCh, syscall.SIGINT, syscall.SIGTERM)

	sig := <-signalCh
	log.Info().Msgf("signal %v", sig)

	os.Exit(0)
}

func worker() {
	logging.Init()

	// 初始化配置管理模块
	if err := config.Init(); err != nil {
		log.Fatal().Err(err).Msg("Failed to initialize config")
		return
	}

	// 初始化资源管理模块
	if err := resource.Init(); err != nil {
		log.Fatal().Err(err).Msg("Failed to initialize resource")
		return
	}

	// 启动核心业务服务
	if err := service.Run(); err != nil {
		log.Fatal().Err(err).Msg("Failed to initialize service")
		return
	}
}
