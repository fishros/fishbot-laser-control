'''
作者: 小鱼
公众号: 鱼香ROS
QQ交流群: 2642868461
描述: file content
'''
'''
作者: 小鱼
公众号: 鱼香ROS
QQ交流群: 2642868461
描述: file content
'''
'''
作者: 小鱼
公众号: 鱼香ROS
QQ交流群: 2642868461
描述: file content
'''
# -*- coding: utf-8 -*-
import socket
from queue import Queue
import threading
import time
import math



class UdpServer:
    def __init__(self,queue=Queue(),port=3333):
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.event = threading.Event()
        
        self.frame_queue = queue
        self.addr = None

        self.__enter__()

    def __enter__(self):
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print('Bind failed:{}'.format(e))
            raise
        # self.socket.listen()
        print('Starting server on port={} ip={}'.format(self.port,"0.0.0.0"))
        # 启动线程接受连接
        self.server_thread = threading.Thread(target=self.run_server,name='run_server')
        self.server_thread.start()
        return self

    def send_frame(self,frame):
        print("Send to %s, frame: %s",self.addr,frame)
        while self.addr is None:
            time.sleep(1)
            print("wait address...")
        self.socket.sendto(frame,self.addr)

    def run_server(self):
        print("server start....")
        frames = b''
        count = 0
        last_time = time.time()
        start =  time.time()
        while not self.event.is_set():
            data, self.addr = self.socket.recvfrom(220)
            if not data: continue
            frames += data
            first = frames.find(b'\xfa')
            while first != -1 and len(frames)>=22:
                frame = frames[first:first+22]
                frames = frames[first+22:]
                if len(frame)!=22:
                    print("frame",frame)
                first = frames.find(b'\xfa')

                self.frame_queue.put(frame)
                count += 1

    def __exit__(self, exc_type, exc_value, traceback):
        self.server_thread.join()
        self.socket.close()


