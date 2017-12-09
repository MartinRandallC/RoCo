# -*- coding: utf-8 -*-
import os
import socket
import time
import numpy as np


######FUNCION PARA RECEPCION POR SOCKET#########	
def rx_socket(direccion):
    dic_pos["Latitud vieja " + direccion] = dic_pos["Latitud nueva " + direccion]
    dic_pos["Longitud vieja " + direccion] = dic_pos["Longitud nueva " + direccion]
    dato_aux = ""
    dato_valido = False
    #Me quedo esperando hasta recibir una posicion valida
    while not dato_valido:
        if (direccion=="Metalico"):
            dato_aux = s.recv(4096)
        else:
            dato_aux = s3.recv(4096) 
        print("Recibi por socket: %s " % dato_aux)
        pos = dato_aux.find(" ") + 1
        direccion_obtenida = dato_aux[0:pos-1]
        dato = dato_aux[pos::]
        # para debuguear lo que llega, no parece necesario: print('el dato es: ' + dato + ' y la dir es: ' + direccion_obtenida)
        if (direccion_obtenida == 'EstacionBase') and ((dato[0]=='0') or (dato[0]=='1')):
            dato_valido = True
    print("Recibi por socket posicion valida donde se encuentra AR: %s " % dato)
    llegue = int(dato[0])
    if (llegue == 1):
        print('-=-=-=-=- Fatality!!! Ha llegado el %s a su destino. -=-=-=-=- ' % direccion)
    coordenadas = dato[1::]
    latitud_actual = int(coordenadas[0:8])
    dic_pos["Latitud nueva " + direccion] = latitud_actual
    longitud_actual = int(coordenadas[8:16])
    dic_pos["Longitud nueva " + direccion] = longitud_actual
    return llegue, coordenadas


######FUNCION PARA MEDIR QOS#########
def QoS_measure(direccion):
    intentos = 0
    exitos = 0
    umbral = 40.0
    mensaje_comienzo = direccion + ' Hola'
    s2.sendto(mensaje_comienzo, (host, port2) )
    while (intentos<100):
        respuesta = s.recv(4096)
        if (respuesta=='Try!'):
            print('Intentos' + str(intentos))
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
    if (direccion=="Metalico"):
        s2.sendto(dato, (host, port2) )
    else:
        s4.sendto(dato, (host, port4) ) 
    print("Envie por socket posicion a la que debe dirigirse AR: %s " % dato)


#######FUNCION PARA CALCULO DE PROYECCION#########
def calcular_proyeccion(lat_origen,lon_origen,lat_destino,lon_destino,lat_actual,lon_actual):
    b11 = lat_origen * ((lon_destino - lon_origen) / (lat_destino - lat_origen)) - lon_origen
    b21 = lat_actual * ((lat_origen - lat_destino) / (lon_destino - lon_origen)) - lon_actual
    a11 = (lon_destino - lon_origen) / (lat_destino - lat_origen)
    a12 = -1
    a21 = (lat_origen - lat_destino) / (lon_destino - lon_origen)
    a22 = -1
    #import numpy as np
    a = np.array([[a11,a12], [a21,a22]])
    b = np.array([b11,b21])
    x = np.linalg.solve(a, b)
    return int(x[0]), int(x[1])
 

##############PROGRAMA PRINCIPAL##################

dic_pos = {"Latitud nueva Metalico":0, "Latitud nueva Repetidor":0, "Longitud nueva Metalico":0, "Longitud nueva Repetidor":0}

umbral_alto = 90
umbral_repetidor = 50
umbral_bajo = 20

host = '127.0.0.1'

#Creo SERVIDOR UDP para RECIBIR datos de GNU Radio (habla con Metalico)
port = 50007
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((host, port))

#Creo CLIENTE UDP para ENVIAR datos a GNU Radio (habla con Metalico)
port2 = 50008
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#Creo SERVIDOR UDP para RECIBIR datos de GNU Radio (habla con Repetidor)
port3 = 50017
s3 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s3.bind((host, port3))

#Creo CLIENTE UDP para ENVIAR datos a GNU Radio (habla con Repetidor)
port4 = 50018
s4 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

destino_metalico = '3491927556166300%'#Posicion club de bochas: '3491716156166108%'

origen = '3491907556166150' #Medir y completar

s2.sendto('Metalico despertate!', (host, port2))
print("Envie por socket: Metalico despertate!")
llego_metalico, pos_metalico = rx_socket('Metalico')
QoS_metalico = input("Ingrese QoS_metalico: ")

print('llego_metalico == ' + str(llego_metalico))
while (llego_metalico == 0):
    
    print("Dentro del while principal")
    #QoS_metalico = QoS_measure()  #  * Revisar!! Al principio va a hacer dos veces la medida

    if (QoS_metalico < umbral_bajo):
        x, y = calcular_proyeccion(int(origen[0:8]), int(origen[8:16]), int(destino_metalico[0:8]), int(destino_metalico[8:16]), dic_pos["Latitud vieja Metalico"], dic_pos["Longitud vieja Metalico"])
        destino_repetidor = str(x)+str(y)+"%"  ###Ver calculo de destino_repetidor####
        s4.sendto('Repetidor despertate!', (host, port4))   #Se puede obviar si guardamos en listas la posicion
        print("Envie por socket: Repetidor despertate!")
        llego_repetidor, pos_repetidor = rx_socket('Repetidor')
        QoS_repetidor = input("Ingrese QoS_repetidor: ")
        if (QoS_repetidor >= umbral_alto):
            tx_socket('Repetidor', QoS_repetidor, destino_repetidor)
        elif (QoS_repetidor < umbral_repetidor):
            print('La QoS del Repetidor es mala, deberia estar volviendo al origen!')
            tx_socket('Repetidor', 1, origen)

        while (QoS_repetidor >= umbral_repetidor): #and (llego_repetidor == 0)):    ### Si Q0S rep <umbral_alto??? volver al origen
            print("Dentro del while de movimiento del Repetidor")
            s4.sendto('Repetidor despertate!', (host, port4))  
            print("Envie por socket: Repetidor despertate!")
            llego_repetidor, pos_repetidor = rx_socket('Repetidor')
            QoS_repetidor = input("Ingrese QoS_repetidor: ")
            QoS_metalico = input("Ingrese QoS_metalico: ")
            if llego_repetidor == 1:
                break
            if (QoS_repetidor >= umbral_repetidor):
                tx_socket('Repetidor', QoS_repetidor, destino_repetidor)
            elif (QoS_repetidor < umbral_repetidor):
                print('La QoS del Repetidor es mala, deberia estar volviendo al origen!')
                tx_socket('Repetidor', 1, origen)

	
    if (QoS_metalico < umbral_alto) and (QoS_metalico >= umbral_bajo):
        print('origen[0:8]' + str(int(origen[0:8])))
        x, y = calcular_proyeccion(int(origen[0:8]), int(origen[8:16]), int(destino_metalico[0:8]), int(destino_metalico[8:16]), dic_pos["Latitud nueva Metalico"], dic_pos["Longitud nueva Metalico"])
        destino_repetidor = str(x) + str(y) + "%"  ###Ver calculo de destino_repetidor####
        s4.sendto('Repetidor despertate!', (host, port4))  # Se puede obviar si guardamos en listas la posicion
        print("Envie por socket: Repetidor despertate!")
        llego_repetidor, pos_repetidor = rx_socket('Repetidor')
        QoS_repetidor = input("Ingrese QoS_repetidor: ")
        if (QoS_repetidor >= umbral_repetidor):
            tx_socket('Repetidor', QoS_repetidor, destino_repetidor)
        elif (QoS_repetidor < umbral_repetidor):
            print('La QoS del Repetidor es mala, deberia estar volviendo al origen!')
            tx_socket('Repetidor', 1, origen)

        while (QoS_repetidor >= umbral_repetidor):  # and (llego_repetidor == 0)):    ### Si Q0S rep <umbral_alto??? volver al origen
            print("Dentro del while de movimiento del Repetidor")
            s4.sendto('Repetidor despertate!', (host, port4))  
            print("Envie por socket: Repetidor despertate!")
            llego_repetidor, pos_repetidor = rx_socket('Repetidor')
            QoS_repetidor = input("Ingrese QoS_repetidor: ")
            QoS_metalico = input("Ingrese QoS_metalico: ")
            if llego_repetidor == 1:
                break
            if (QoS_repetidor >= umbral_repetidor):
                tx_socket('Repetidor', QoS_repetidor, destino_repetidor)
            elif (QoS_repetidor < umbral_repetidor):
                print('La QoS del Repetidor es mala, deberia estar volviendo al origen!')
                tx_socket('Repetidor', 1, origen)


    while (QoS_metalico >= umbral_alto): #and (llego_metalico == 0)):
        print("Dentro del while de movimiento del Metalico")
        s2.sendto('Metalico despertate!', (host, port2))
        print("Envie por socket: Metalico despertate!")
        llego_metalico, pos_metalico = rx_socket('Metalico')
        QoS_metalico = input("Ingrese QoS_metalico: ")
        if llego_metalico == 1:
            break
        if (QoS_metalico >= umbral_alto):
            tx_socket('Metalico', QoS_metalico, destino_metalico)
