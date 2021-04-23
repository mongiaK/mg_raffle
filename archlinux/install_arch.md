# archlinux 桌面版安装

## 安装手册
![](https://wiki.archlinux.org/index.php/Installation_guide_(%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87))

### 注意事项

* 如果使用的wifi连接，安装软件记得安装iwd，此软件包含iwctl 工具
* 目前基本使用的grub引导

## grub引导
![](https://wiki.archlinux.org/index.php/GRUB_(%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87))

* 如果需要关闭开机引导选择，需要修改grub里面的参数，直接在boot分区改，或者重新制作grub引导配置
* fsck磁盘安装检查的打印建议别关闭  (类似的打印 /dev/sda2 clean ...)
* start version 打印也在这里，注意删除

## archlinux 常用软件包

* panon 状态栏音频跳动软件，装逼神器
* latte 仿mac的dock栏
* 主题选择chromeOS-dark
* 字体选择source Code Pro
* icons 使用plasmaXDark
* KDEConnect 手机互联
* Qv2ray 翻墙vpn
* flameshot截图
* thunderbird 邮件
* listen1 音乐播放器
* wechat-uos
* chrome
* utools
* ... 

## bugs

* 蓝牙无法连接
    
journalctl 显示protocol not found
编辑 /etc/pulse/daemon.conf   修改 exit-ilde-time = -1
