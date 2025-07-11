#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
端口显示格式测试脚本
测试端口号显示为 tcp:8889 或 udp:8889 格式
"""

import serial
import time

def test_port_display():
    """测试端口显示格式"""
    
    # 配置串口
    ser = serial.Serial(
        port='/dev/ttyUSB0',  # 根据实际情况修改
        baudrate=115200,
        timeout=1
    )
    
    print("开始测试端口显示格式...")
    
    # 测试读取当前配置
    print("\n1. 读取当前配置:")
    ser.write(b"$command=read_config\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 测试设置TCP模式
    print("\n2. 设置TCP模式:")
    ser.write(b"$net_mode=tcp\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 验证TCP模式下的端口显示
    print("\n3. 验证TCP模式端口显示:")
    ser.write(b"$command=read_config\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 测试设置UDP模式
    print("\n4. 设置UDP模式:")
    ser.write(b"$net_mode=udp\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 验证UDP模式下的端口显示
    print("\n5. 验证UDP模式端口显示:")
    ser.write(b"$command=read_config\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 恢复TCP模式
    print("\n6. 恢复TCP模式:")
    ser.write(b"$net_mode=tcp\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    ser.close()
    print("\n测试完成!")

if __name__ == "__main__":
    test_port_display() 