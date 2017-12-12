import os
import time
import subprocess

while 1:
    os.system("ps x > log.txt")
    file = open('log.txt', 'r')
    archivo_leido = file.read()
    if "com_serial.py" not in archivo_leido:
        os.system("echo Reabriendo com_serial.py")
        os.system("python2 com_serial.py &")
    if "top_block.py" not in archivo_leido:
        os.system("echo Reabriendo top_block.py")
        os.system("python2 top_block.py &")
        
    temperatura = subprocess.check_output("/opt/vc/bin/vcgencmd measure_temp", shell=True)
    print(temperatura)
    temperatura = temperatura.split("=")
    temperatura = temperatura[1].split(".")
    if(temperatura[0] > 95):
        os.system("echo passwd | sudo shutdown now")
        
    time.sleep(10)
    
file.close()
