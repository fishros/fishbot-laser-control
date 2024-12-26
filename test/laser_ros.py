import subprocess
import time

class LaserScanRos2():

    def __init__(self) -> None:
        self.laser_pro = None

    def kill(self):
        if self.laser_pro:
            self.laser_pro.terminate()
            self.laser_pro.kill()
            self.laser_pro.wait()

    def restart(self):
        self.kill()
        self.laser_pro = subprocess.Popen("./ydlidar_x2_ros2", shell=False)
    


laser = LaserScanRos2()
laser.restart()
time.sleep(5)
laser.restart()
time.sleep(10)
laser.kill()
