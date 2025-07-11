#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
UDP服务器测试脚本
用于测试UDP客户端的数据传输
"""

import socket
import threading
import time

class UDPServer:
    def __init__(self, host='0.0.0.0', port=3347):
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind((self.host, self.port))
        self.running = False
        
    def start(self):
        """启动UDP服务器"""
        self.running = True
        print(f"UDP服务器启动在 {self.host}:{self.port}")
        
        while self.running:
            try:
                data, addr = self.socket.recvfrom(1024)
                print(f"收到来自 {addr} 的数据: {data.decode('utf-8', errors='ignore')}")
                
                # 发送响应
                response = f"UDP服务器已收到: {data.decode('utf-8', errors='ignore')}"
                self.socket.sendto(response.encode('utf-8'), addr)
                
            except Exception as e:
                print(f"接收数据错误: {e}")
                break
                
    def stop(self):
        """停止UDP服务器"""
        self.running = False
        self.socket.close()
        print("UDP服务器已停止")

def main():
    """主函数"""
    server = UDPServer(port=3347)
    
    try:
        server.start()
    except KeyboardInterrupt:
        print("\n正在停止服务器...")
        server.stop()

if __name__ == "__main__":
    main() 