package config

import ()

type RxAgentConfig struct {

}

func Init() error {
	return nil
}

package config

import (
    "fmt"
    "sync"

    "gopkg.in/yaml.v2"
)

// AppConfig 定义应用程序的配置结构体
type AppConfig struct {
    Server struct {
        Port int `yaml:"port"`
    } `yaml:"server"`
    Database struct {
        DSN string `yaml:"dsn"`
    } `yaml:"database"`
}

var (
    config *AppConfig
    once   sync.Once
)

// Load 加载并解析 YAML 配置文件
func Load() error {
    var err error
    once.Do(func() {
        config = &AppConfig{}
        err = yaml.UnmarshalStrict([]byte(configYAML), config)
    })
    return err
}

// Reload 重新加载配置
func Reload() error {
    return Load()
}

// Get 获取当前配置
func Get() *AppConfig {
    return config
}
