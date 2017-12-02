/*
Pograma principal del proyecto de fin de carrera en ingeniería eléctrica 
Roco(Robots comunicados)

Luego de que se inicializan los sensores y se obtiene la primer medida del GPS, el 
programa se detiene durante dos minutos para que el sensor mejore su precisión. Al cabo 
de este tiempo, comienza la ejecución del loop principal.
 
Una vez dentro de él, lo primero es enviar la posición actual del robot a Raspberry 
Pi utilizando la comunicación serial y luego esperar hasta que éste conteste con una 
posición de destino. Si no contesta al cabo de 2 segundos, repite el procedimiento 
de enviar su posición y esperar. En el momento en que Arduino recibe una posición de 
destino válida (por válida entendemos que debe estar a una distancia menor a 2km del 
robot), el programa chequea que la calidad de la señal de la comunicación con la 
radiobase sea buena y que la distancia entre las posiciones sea mayor al error del GPS. 
En caso que esto se cumpla, se calibra el magnetómetro y se orienta al robot. Si no 
se logró orientar con ninguno de los dos métodos anteriormente mencionados, se calibra 
nuevamente el magnetómetro y se ejecuta el algoritmo de orientación. Cuando el robot 
se logra orientar hacia su destino, avanza 3mts hacia adelante y se detiene.
 
Copyright (C) 2017 RoCo
All rights reserved.

Autores: Gabriel Bibbó, Mariana Gelóz, Martín Randall
*/

#include <TinyGPS.h>

#include <SoftwareSerial.h>

#include <I2Cdev.h>

#include <Wire.h>

#include "ubicacion.h"
#include "motores.h"
#include "compass.h"


long latDeseada,lonDeseada;
int calidad=1;
long distancia;
int angulos;
boolean mensaje_recibido;
boolean primera_vez;

void setup() {
  latDeseada=0;
  lonDeseada=0;
  primera_vez=true;
  Serial.begin(9600); 
  Serial.println("Inicializando. . .");
  inicializar(); // inicializa los motores esto se encuentra en otro programa (Motores.ino)   
  inicializacion_sensores();
  ubicacionGPS(); //espera hasta que llegue primera medida del GPS
  Serial.println("Convergencia del GPS...");
  Serial.println("Esperando 2:00 minutos"); //esperamos 2 minutos para que el GPS se estabilice
  delay(30000); 
  Serial.println("Esperando 1:30 minuto");
  delay(30000); 
  Serial.println("Esperando 1:00 minuto");
  delay(30000); 
  Serial.println("Esperando 30 segundos");
  delay(15000); 
  Serial.println("Esperando 15 segundos");
  delay(15000); 
  Serial.println("Fin Inicializacion");  
}

void loop() {

  int resultado_avance = 0; //0=sin actualizacion de estado; 1=IR1 tocando; 2=IR2 tocando; 5=tiempo>3min; 0=distancia recorrida; 6=desviado
  int resultado_orientacion = 0; //0=no orientado; 1=orientado
  int nro_intento = 1; //1er intento es normal; 2do es girando 90Â° a la derecha; 3ro calibrando
 
  while (enviar_paquete(&latDeseada,&lonDeseada,&calidad) && (calidad==1) && (calcularDistancia(latDeseada,lonDeseada)>errorGPS))  {
      if (primera_vez==true) {
          resultado_orientacion = orientar_robot (latDeseada, lonDeseada, con_calibrar); nro_intento=2;
      }
      if (primera_vez==false) {
          resultado_orientacion = orientar_robot (latDeseada, lonDeseada, sin_calibrar); nro_intento=2;
      }
      primera_vez=false;
      Serial.print("resultado_orientacion="); 
      Serial.println(resultado_orientacion);
      if((resultado_orientacion==0)&&(nro_intento==2)) {
          Serial.println("DERECHA 90");derecha(90); 
          resultado_orientacion = orientar_robot (latDeseada, lonDeseada, sin_calibrar); nro_intento=3; 
      }
      if((resultado_orientacion==0)&&(nro_intento==3)) 
          resultado_orientacion = orientar_robot (latDeseada, lonDeseada, con_calibrar); 
      nro_intento==1;
      Serial.print("Distancia faltante="); 
      Serial.println(calcularDistancia(latDeseada,lonDeseada)); 
      if (resultado_orientacion==1) 
          if(!(!(adelante(300))))  
              esquivar(latDeseada,lonDeseada);
      Serial.print("calidad="); Serial.println(calidad);
  }
  if (calidad==0) 
      Serial.println("Mala Senal!! -> robot para");
  if (calcularDistancia(latDeseada,lonDeseada)<=errorGPS) { 
      Serial.print("Distancia faltante="); Serial.println(calcularDistancia(latDeseada,lonDeseada));   
      Serial.println("Supuestamente llegÃ³!!!!!!!!!!!!!!!!!!----------"); }

  delay(500);

  ubicacionGPS();

  while (calcularDistancia(latDeseada,lonDeseada)<=errorGPS) {   // Arduino se tranca enviando su posición hasta recibir una posición destino valida desde Raspberry Pi
      enviar_paquete(&latDeseada,&lonDeseada,&calidad);
  }

}  //fin del loop principal

