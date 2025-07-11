# FishBot雷达转接板驱动程序

## 功能特性

- 支持UDP和TCP两种网络模式
- 可通过串口配置网络参数
- 支持WiFi连接和配置
- 支持激光雷达数据转发

## 网络模式配置

### 配置参数

设备支持以下配置参数：

- `wifi_ssid`: WiFi网络名称
- `wifi_pswd`: WiFi密码  
- `server_ip`: 服务器IP地址
- `server_port`: 服务器端口 (格式: tcp:8889 或 udp:8889)
- `motor_speed`: 电机速度
- `laser_baud`: 激光雷达波特率
- `net_mode`: 网络模式 (udp/tcp) - 默认tcp

### 配置方法

通过串口发送配置命令：

```
$net_mode=udp    # 设置为UDP模式
$net_mode=tcp    # 设置为TCP模式
$command=read_config  # 读取当前配置
```

### 网络模式说明

- **UDP模式**: 默认模式，使用UDP协议进行数据传输
- **TCP模式**: 使用TCP协议进行数据传输，提供更可靠的数据传输

## Make & MergeBin

make

```
docker run -it --rm --privileged -v=/dev:/dev  -v `pwd`:`pwd` -w `pwd` xingrz/esp8266-rtos-builder make 
```

merge

```
docker run -it --rm --privileged -v=/dev:/dev  -v `pwd`:`pwd` -w `pwd` fishros2/fishbot-tool esptool.py --chip ESP8266 merge_bin -o fishbot_laser_control_v1.0.0.`date +%y%m%d`.bin --flash_mode dio --flash_size 4MB 0x00 build/bootloader/bootloader.bin 0x8000 build/partitions_singleapp.bin 0x10000 build/uart2udp.bin
```

flash 

```
docker run -it --rm --privileged -v=/dev:/dev  -v `pwd`:`pwd` -w `pwd` xingrz/esp8266-rtos-builder make flash
```


设计相关

1. 添加按键控制波特率

```

```