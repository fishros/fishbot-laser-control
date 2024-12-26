# FishBot雷达转接板驱动程序

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