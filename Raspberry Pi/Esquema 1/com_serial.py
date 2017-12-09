import socket
import serial
import time
import sys

ser = serial.Serial('/dev/ttyACM0', 9600)

host = '127.0.0.1'

#Creo SERVIDOR UDP para RECIBIR datos de GNU Radio 
port = 50007
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((host, port))

#Creo CLIENTE UDP para ENVIAR datos a GNU Radio
port2 = 50008
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


while 1:
    data, addr = s.recvfrom(1024)    #UDP    
    print(data)
    ser.write(data)    
conn.close()
