'''
作者: 小鱼
公众号: 鱼香ROS
QQ交流群: 2642868461
描述: file content
'''
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan

import math,time
from queue import Queue
from server import UdpServer,SerialServer

class LaserDriver:
    def __init__(self,type="serial",udp_port=3333,serial_port="/dev/ttyUSB0",serial_baut=115200) -> None:
        rclpy.init()
        self.laser_node = Node("laser_friver")
        self.pub = self.laser_node.create_publisher(LaserScan,"scan",10)
        self.laser_data = LaserScan()
        self.queue_frame = Queue()
        if type=="serial": self.server = SerialServer(self.queue_frame,addr=serial_port,baut=serial_baut)
        if type=="udp": self.server = UdpServer(self.queue_frame,port=udp_port)
        self.init()
        self.stop()



    def init(self):
        self.laser_data.angle_increment = (2.0*math.pi)/360
        self.laser_data.angle_min = 0.0
        self.laser_data.angle_max = 2.0*math.pi-(2.0*math.pi)/360
        self.laser_data.range_min = 0.0
        self.laser_data.range_max = 3.14
        self.laser_data.ranges = [0.0]*360
        self.laser_data.intensities = [0.0]*360
        self.laser_data.time_increment


    def parse_spped(self,speed):
        return (speed[1]<<2) + ((speed[0]&0xc0)>>6) + ((speed[0]&0x3F)/64)


    def parse_point(self,data0):
        flag0 = (data0[1]&0x80)>>7 #距离信息是否有效
        flag1 = (data0[1]&0x40)>>6 #强度信息是否有效
        distance = ((data0[1]&0x3F)<<8)+data0[0]
        strength = (data0[3]<<8)+data0[2]
        return flag0,flag1,distance/1000.0,strength


    def start(self):
        self.server.send_frame(b"startlds$")
        time.sleep(0.5)


    def stop(self):
        self.server.send_frame(b'stoplds$')
        time.sleep(0.5)


    def run_server(self):
        count,start = 0,0
        speed = 0.0
        while rclpy.ok():
            if self.queue_frame.qsize()>0:
                frame = self.queue_frame.get()
                if len(frame)<22: continue
                if frame[1]==0xa0:
                    print("start a frame",count)
                    start = 1
                    index ,count ,speed = 0,0,0.0
                    self.laser_data.ranges = [0.0]*360
                    self.laser_data.intensities = [0.0]*360
                    
                if start==1 :
                    index = frame[1]-160
                    # print("index",index)
                    speed +=  self.parse_spped(frame[2:4])
                    count += 1
                    for i in range(4):
                        flag0,flag1,distance,strength = self.parse_point(frame[4*i:4*(i+1)])
                        # if index<90 and index>=0:
                        if flag0==0 and distance>0.1 and flag1==0: 
                            self.laser_data.ranges[index*4+i] = distance
                            self.laser_data.intensities[index*4+i] = strength
        
                if frame[1]== 0xf9:
                    print("end a frame",count,self.laser_data.ranges[0])
                    start = 0
                    self.laser_data.header.stamp = self.laser_node.get_clock().now().to_msg()
                    self.laser_data.header.frame_id= "laser"
                    if count>0:
                        self.laser_data.time_increment = float(1.0/float(speed/count/10.0*6.0))
                        self.laser_data.scan_time = self.laser_data.time_increment*360
                    self.pub.publish(self.laser_data)

        self.stop()
        self.server.event.set() #退出循环
        rclpy.shutdown()
        


if __name__=='__main__':
    # driver = LaserDriver(type="serial",serial_port='/dev/ttyACM1')
    driver = LaserDriver(type="tcp",udp_port=8848)
    driver.start()
    driver.run_server()
    driver.server.event.set()

    