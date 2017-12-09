import os
import time

os.system("echo Abriendo com_serial.py, top_block.py y watchdog.py")
os.system("cd /home/pi")
os.system("python2 top_block.py &")
time.sleep(60)
os.system("python2 com_serial.py &")
time.sleep(10)
os.system("python2 watchdog.py &")