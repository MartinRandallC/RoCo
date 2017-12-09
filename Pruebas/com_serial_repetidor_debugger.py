# -*- coding: utf-8 -*-
import socket
import serial
import time
import sys
import pty
import os

######FUNCION PARA RECEPCION POR PUERTO SERIAL#######
def rx_serial():
    llegada = False
    dato_valido = False
    #Me quedo esperando hasta recibir una posicion valida
    while not dato_valido:	  
        dato_rx_serial = ser.readline()
        print("Recibi por serial: %s" % dato_rx_serial)
        if ((len(dato_rx_serial)==20) and ((dato_rx_serial[0]=='0') or (dato_rx_serial[0]=='1')) and (dato_rx_serial[1]=='3') and (dato_rx_serial[9]=='5')):
            dato_valido = True
    print("Recibi por serial posicion valida donde se encuentra AR: %s" % dato_rx_serial)
    if (dato_rx_serial[0] == '1'):
        llegada = True
        print("Llegue al destino")
    dato_rx_serial = "EstacionBase " + dato_rx_serial
    return dato_rx_serial, llegada


#####FUNCION PARA TRANSMISION POR PUERTO SERIAL#####
def tx_serial(dato_tx_serial):
    ser.write(dato_tx_serial)
    print("Envie por serial posicion a la que debe dirigirse AR: %s" % dato_tx_serial)


#######FUNCION PARA RECEPCION POR SOCKET###########
def rx_socket():
    dato_valido = False
    #Me quedo esperando hasta recibir una posicion valida
    while not dato_valido:
        dato_rx_socket = s.recv(4096)
        print("Recibi por socket: %s" % dato_rx_socket)
        pos = dato_rx_socket.find(" ") + 1
        largo = len(dato_rx_socket)
        dato_rx_socket_cortado = dato_rx_socket[pos:largo]        
        if dato_rx_socket_cortado == 'Stop!':
            print("Llego a Stop! => Cierro top_block.py")
            os.system("pkill -f top_block.py")                        #watchdog.py lo va a reabrir
        if (len(dato_rx_socket_cortado)==18):
            dato_valido = True
        if (dato_rx_socket_cortado == 'despertate!'):
            dato_valido = True
    print("Recibi por socket dato valido: %s" % dato_rx_socket_cortado)
    return dato_rx_socket_cortado


######FUNCION PARA TRANSMISION POR SOCKET#########	
def tx_socket(dato_tx_socket):
    s2.sendto(dato_tx_socket, (host, port2) )
    print("Envie por socket posicion valida donde se encuentra AR: %s" % dato_tx_socket)

##############PROGRAMA PRINCIPAL##################

contador = 0

llegue = False

#Abro comunicacion serial
#ser = serial.Serial('/dev/ttyACM0', 9600)    #Alternativamente 115200

host = '127.0.0.1'

#Creo SERVIDOR UDP para RECIBIR datos de GNU Radio 
port = 50015
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((host, port))

#Creo CLIENTE UDP para ENVIAR datos a GNU Radio
port2 = 50016
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


################LOOP INFINITO####################
	   
while 1:
    contador += 1
    print(contador)
    msje_rx_socket = rx_socket()
    if msje_rx_socket == 'despertate!':
        msje_rx_serial = input("Ingrese posicion: ")
        tx_socket('EstacionBase ' + msje_rx_serial)
    else:
        print("Transmito por serial: %s" % msje_rx_socket)

