# V1.0.0.20230111

- [雷达板]修复支持SSD1315芯片驱动
- [雷达板]修复按键切换误触发问题，双击切换模式
- [雷达板]修复点击重启按钮误触发模式切换问题

# V1.0.0.20230108

- [雷达板]增加模式显示功能
- [雷达板]修改单击BOOT切换配置/运行模式功能
- [雷达板]增加WIFI，用户名和密码显示功能
- [雷达板]增加服务地址和端口显示功能

# V1.0.0.20230107

- [雷达板]增加OLED显示网络IP功能


```
echo 11ca 0211 > /sys/bus/usb-serial/drivers/cp210x/new_id 
sudo ./cp210x-cfg -m 11ca:0211 -V 0x10C4 -P 0xEA60

sudo ./cp210x-cfg -m 11ca:0212 
sudo ./cp210x-cfg -m 10C4:EA60 -N "FishBot's CP2102 UART Bridge"
```