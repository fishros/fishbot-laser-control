#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
网络模式配置测试脚本
测试UDP和TCP模式的配置功能
"""

import serial
import time

def test_net_mode_config():
    """测试网络模式配置功能"""
    
    # 配置串口
    ser = serial.Serial(
        port='/dev/ttyUSB0',  # 根据实际情况修改
        baudrate=115200,
        timeout=1
    )
    
    print("开始测试网络模式配置...")
    
    # 测试读取当前配置
    print("\n1. 读取当前配置:")
    ser.write(b"$command=read_config\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 测试设置TCP模式（默认）
    print("\n2. 设置TCP模式（默认）:")
    ser.write(b"$net_mode=tcp\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 测试设置UDP模式
    print("\n3. 设置UDP模式:")
    ser.write(b"$net_mode=udp\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 验证配置
    print("\n4. 验证配置:")
    ser.write(b"$command=read_config\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    # 恢复TCP模式（默认）
    print("\n5. 恢复TCP模式（默认）:")
    ser.write(b"$net_mode=tcp\n")
    time.sleep(0.5)
    while ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  {response}")
    
    ser.close()
    print("\n测试完成!")

if __name__ == "__main__":
    test_net_mode_config() 