# -*- coding: utf-8 -*-
'''
作者: 小鱼
公众号: 鱼香ROS
QQ交流群: 2642868461
描述: file content
'''
import socket
from queue import Queue
import threading
import time
import struct
import serial #导入模块
import math

from yaml import scan

import rospy
from sensor_msgs.msg import LaserScan

server = None

class SerialServer:
    def __init__(self,queue=Queue(),addr='/dev/ttyUSB0',baut=115200):
        self.event = threading.Event()
        self.frame_queue = queue
        self.addr = addr
        self.baut = baut
        self.connected = False
        self.__enter__()

    def __enter__(self):
        try:
            self.ser=serial.Serial(self.addr,self.baut,timeout=0)
            self.connected = True
        except socket.error as e:
            print('Open failed:{}'.format(e))
            self.connected = False
            raise
        print('Starting server on addr={} baut={}'.format(self.addr,self.baut))
        # 启动线程读取数据
        self.server_thread = threading.Thread(target=self.run_server,name='run_server')
        self.server_thread.start()
        return self

    def send_frame(self,frame):
        print("Send to %s, frame: %s",self.addr,frame)
        self.ser.write(frame)

    def run_server(self):
        print("server start....")
        frames = b''
        count = 0
        last_time = time.time()
        start =  time.time()
        while not rospy.is_shutdown():
            data = self.ser.read_all()
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
                if int(time.time() - last_time) >= 4:
                    print("服务信息：平均帧率:%s/s, 运行时间:%ds,frames_len: %d" % (str(int(count / (time.time() - last_time))),int(time.time()-start),len(frames)))
                    last_time = time.time()
                    count = 0

    def __exit__(self, exc_type, exc_value, traceback):
        self.server_thread.join()
        self.socket.close()


rospy.init_node("lds_driver",anonymous=True)
pub = rospy.Publisher("scan",LaserScan,queue_size=1)
laser_data = LaserScan()
laser_data.angle_increment = (2.0*math.pi)/360
laser_data.angle_min = 0
laser_data.angle_max = 2.0*math.pi-(2.0*math.pi)/360
laser_data.range_min = 0
laser_data.range_max = 3.5
laser_data.ranges = [0]*360
laser_data.intensities = [0]*360

queue_frame = Queue()
server =  SerialServer(queue_frame)
server.send_frame(b"stoplds$")
time.sleep(1)
server.send_frame(b"startlds$")



def parse_point(data0):
    flag0 = (data0[1]&0x80)>>7 #距离信息是否有效
    flag1 = (data0[1]&0x40)>>6 #强度信息是否有效
    distance = ((data0[1]&0x3F)<<8)+data0[0]
    strength = (data0[3]<<8)+data0[2]
    return flag0,flag1,distance/1000.0,strength

count = 0
start = 1
index = 0

while not rospy.is_shutdown():
    if queue_frame.qsize()>0:
        frame = queue_frame.get()
        if len(frame)<22: continue
        if frame[1]==0xa0:
            print("start a frame",count)
            start = 1
            index = 0
            laser_data.ranges = [0]*360
            laser_data.intensities = [0]*360
            
        if start==1 :
            index = frame[1]-160
            for i in range(4):
                flag0,flag1,distance,strength = parse_point(frame[4*i:4*(i+1)])
                # print("distance:",distance,"index",index,index*4+i)
                if index<90:
                    if flag0==0 and distance>0.1 and flag1==0: 
                        laser_data.ranges[index*4+i] = distance
                        laser_data.intensities[index*4+i] = strength
 
        if frame[1]== 0xf9:
            print("end a frame",count,laser_data.ranges[0])
            start = 0
            laser_data.header.stamp = rospy.Time.now()
            laser_data.header.frame_id= "laser"
            # laser_data.header.frame_id= "laser"
            pub.publish(laser_data)

server.send_frame(b"stoplds$")