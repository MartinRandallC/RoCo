import socket 
import sys
import time
from Tkinter import *


def leftKey(event):
    message = 'Metalico 4'+'%'
    s2.sendto(message, (host, port2) )
    print "Izquierda"

def rightKey(event):
    message = 'Metalico 5'+'%'	
    s2.sendto(message, (host, port2) )
    print "Derecha"

def upKey(event):
    message = 'Metalico 3'+'%'
    s2.sendto(message, (host, port2) )
    print "Adelante"

def downKey(event):
    message = 'Metalico 6'+'%'
    s2.sendto(message, (host, port2) )
    print "Atras"

def tabKey(event):
    message = 'Metalico 7'+'%'
    s2.sendto(message, (host, port2) )
    print "Parar"

host = '127.0.0.1'

#Creo SERVIDOR UDP para RECIBIR datos de GNU Radio 
port = 50007
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((host, port))

#Creo CLIENTE UDP para ENVIAR datos a GNU Radio
port2 = 50008
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

main = Tk()
main.bind('<Left>', leftKey)
main.bind('<Right>', rightKey)
main.bind('<Up>', upKey)
main.bind('<Down>', downKey)
main.bind('<Tab>', tabKey)
main.mainloop() 
    
s.close()
