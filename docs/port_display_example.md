# 端口显示格式示例

## 概述

FishBot激光雷达转接板的端口号现在显示为 `协议:端口号` 的格式，例如 `tcp:8889` 或 `udp:3347`。

## 显示格式

### OLED屏幕显示
在OLED屏幕上，端口号显示为：
```
s_port:tcp:8889
```
或
```
s_port:udp:3347
```

### 串口配置输出
通过串口读取配置时，输出格式为：
```
$server_port=tcp:8889
```
或
```
$server_port=udp:3347
```

## 配置示例

### TCP模式配置
```bash
# 设置TCP模式
$net_mode=tcp

# 设置服务器IP和端口
$server_ip=192.168.1.100
$server_port=8889

# 读取配置（显示为 tcp:8889）
$command=read_config
```

### UDP模式配置
```bash
# 设置UDP模式
$net_mode=udp

# 设置服务器IP和端口
$server_ip=192.168.1.100
$server_port=3347

# 读取配置（显示为 udp:3347）
$command=read_config
```

## 配置验证

### 验证TCP模式
```bash
$command=read_config
```
预期输出：
```
$wifi_ssid=fishbot
$wifi_pswd=12345678
$server_ip=192.168.1.100
$server_port=tcp:8889
$motor_speed=600
$laser_baud=115200
$net_mode=tcp
$board=laser_board
```

### 验证UDP模式
```bash
$command=read_config
```
预期输出：
```
$wifi_ssid=fishbot
$wifi_pswd=12345678
$server_ip=192.168.1.100
$server_port=udp:3347
$motor_speed=600
$laser_baud=115200
$net_mode=udp
$board=laser_board
```

## 注意事项

1. 端口号显示格式会自动根据当前网络模式调整
2. 修改网络模式后，端口显示格式会立即更新
3. 配置保存在NVS中，重启后仍然有效
4. OLED屏幕和串口输出都使用相同的显示格式 