#!/usr/bin/env python3

import os
import pty
import socket
import select
import argparse

parser = argparse.ArgumentParser(description='Creates a virtual pty for a remote tcp/udp socket')
parser.add_argument('port', type=int)
parser.add_argument('uart_name', type=str)
args = parser.parse_args()

lport = args.port
luser_name = args.uart_name

def restart_laser_driver_ros2():
    """
    Restart YdLaser Driver With Ros2!
    1.LifeCycle 
    2.Restart Laser SubProcess
    """
    pass

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('0.0.0.0', lport))
    s.listen(5)
    master, slave = pty.openpty()
    if os.path.exists(luser_name): os.remove(luser_name)
    os.symlink(os.ttyname(slave),luser_name)
    print(f"UART2SOCKET:{lport}->{luser_name}")
    mypoll = select.poll()
    mypoll.register(master, select.POLLIN)

    try:
        while True:
            print("Prepare to Accept connect!")
            client,client_address = s.accept()
            mypoll.register(client.fileno(), select.POLLIN)
            print(s.fileno(),client,master)
            print('PTY: Opened {} for {}:{}'.format(os.ttyname(slave), '0.0.0.0', lport))
            is_connect = True
            try:
                while is_connect:
                    fdlist = mypoll.poll(256)
                    for fd,event in fdlist:
                        data = os.read(fd, 256)
                        write_fd = client.fileno() if fd == master else master
                        if len(data)==0:
                            is_connect = False
                            break
                        os.write(write_fd, data)
                        print(fd, event,data)
            except ConnectionResetError:
                is_connect = False
                print("远程被迫断开链接")
            finally:
                mypoll.unregister(client.fileno())
    finally:
        s.close()
        os.close(master)
        os.close(slave)
        os.remove(luser_name)

if __name__ == "__main__":
    main()