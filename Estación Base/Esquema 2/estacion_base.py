# -*- coding: utf-8 -*-
import os
import socket
import serial
import time


######FUNCION PARA RECEPCION POR SOCKET#########	
def rx_socket():
    dato_aux = ""
    dato = ""
    dato_valido = False
    #Me quedo esperando hasta recibir una posicion valida
    while not dato_valido:
        dato_aux = s.recv(4096)
        print("Recibi por socket: %s \n" % dato_aux)
        pos = dato_aux.find(" ") + 1
        dato = dato_aux[pos::]
        if (len(dato)==20) and ((dato[0]=='0') or (dato[0]=='1')) and (dato[1]=='3') and (dato[9]=='5'):
            dato_valido = True
    print("Recibi por socket posicion valida donde se encuentra AR: %s \n" % dato)
    #return dato_rx_socket


######FUNCION PARA MEDIR QOS#########
def QoS_measure(direccion):
    intentos = 0
    exitos = 0
    mensaje_comienzo = direccion + ' Hola'
    s2.sendto(mensaje_comienzo, (host, port2) )
    while (intentos<100):
        respuesta = s.recv(4096)
        if (respuesta=='Try!'):
            print('Intentos ' + str(intentos))
            intentos += 1
        elif (respuesta=='Success!'):
            exitos += 1
            s2.sendto(mensaje_comienzo, (host, port2) )
        elif (respuesta=='Stop!'):
            os.system("pkill -f top_block.py") # mato el top_block para reiniciarlo
            os.system("xfce4-terminal -x sh -c 'python top_block.py; bash'")
        else:
            pass

    tasa_exito = 100*float(exitos)/float(intentos)
    print('---->>>> La QoS es ' + str(tasa_exito) + '% <<<<----')
    return tasa_exito


######FUNCION PARA TRANSMISION POR SOCKET#########		
def tx_socket(direccion, QoS, posicion):
    if (QoS <= umbral_alto):
        dato = direccion + ' ' + '0' + posicion
    else:
        dato = direccion + ' ' + '1' + posicion
    s2.sendto(dato, (host, port2) )
    print("Envie por socket posicion a la que debe dirigirse AR: %s " % dato)
 

##############PROGRAMA PRINCIPAL##################

#avanza = 1

umbral_alto = 10

contador = 0

host = '127.0.0.1'

msje_tx_socket = '3491927556166300%'

#Creo SERVIDOR UDP para RECIBIR datos de GNU Radio 
port = 50007
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((host, port))

#Creo CLIENTE UDP para ENVIAR datos a GNU Radio
port2 = 50008
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


################LOOP INFINITO####################
while 1:
    contador += 1
    print(contador)
    rx_socket()
    #mensaje = str('Metalico ') + msje_tx_socket    
    avanza = QoS_measure('Metalico')
    tx_socket('Metalico', avanza, msje_tx_socket)
