/*
ubicacion - Esta librería se desarrolló para el proyecto de fin de carrera, 
Robots Comunicados. En ella se agrupan todas las funciones que involucran 
directamente el manejo del driver de motores. Al comienzo se definen varias 
constantes obtenidas experimentalmente. Algunas son velocidades de giro de los 
motores que funcionan por PWM (Pulse-Width-Modulation), otras son constantes 
por las que se multiplica la cuenta llevada a cabo por los encoders para estimar 
la distancia recorrida en línea recta o el ángulo de giro. Existe un juego 
distinto de constantes para cada uno de los robots debido a sus diferentes 
dimensiones constructivas.

Copyright (C) 2017 RoCo
All rights reserved.

Autores: Gabriel Bibbó, Mariana Gelóz, Martín Randall
*/


#ifndef motores.h
  #define motores.h

// Definitions ----------------------------------------------------------------
//Comentar las constantes segun el robot que se vaya a utilizar!!!///

#define velAvanceIzq 252 // robot chico!!!!
#define velAvanceDer 255 // robot chico!!!!
#define velgiro 250 // robot chico !!!!
#define velgiroCalibracion 240 //robot chico!!!!
#define velgiroLento 200 // robot chico !!!!
#define constGiroIzq 0.53 // robot chico!!!!
#define constGiroDer 0.66 // robot chico!!!!
#define constVel 0.22 // robot chico !!!!
#define robot 1 // robot chico !!!! robot chico -> robot=1 ; robot grande -> robot=2
#define declinacion -5//10.583 // robot chico !!!!  -10.583; //declinaciÃ³n de -10Â° 35' (Montevideo)


// #define velAvanceIzq 255 // robot grande!!!!
// #define velAvanceDer 245 // robot grande!!!!
// #define velgiro 255 // robot grande !!!!
// #define velgiroCalibracion 255 //robot grande!!!!
// #define velgiroLento 255//182 // robot grande !!!! 150 es el valor probado en el exterior y 120 en interior (se usa como metodo de calibracion 2)
// #define constGiroDer 0.383 // robot grande!!!!
// #define constGiroIzq 0.41 // robot grande!!!!
// #define constVel 52.9/100 // robot grande !!!!
// #define robot 2 // robot grande !!!! robot chico -> robot=1 ; robot grande -> robot=2
// #define declinacion -2 // robot grande !!!!
 
#define IN1 5 
#define IN2 6
#define IN3 9
#define IN4 10
#define pinIR1 11
#define pinIR2 12
#define ToleranciaAngulo 2
// Global Variables ----------------------------------------------------------
  extern int nollega;
  extern int indice_promediosGPS;

void inicializar(); //inicializar las salidas del microcontrolador para manejo 
                    //de motores 

void izquierda (int grados); //girar a la izquierda a máxima velocidad un cierto ángulo

void derecha (int grados); //girar a la derecha a máxima velocidad un cierto ángulo 

void giroLentoder(int velgiroL); //girar a la derecha a una velocidad específica 

void giroLentoizq(int velgiroL); //girar a la izquierda a una velocidad específica 

void adelante (); //avanzar en línea recta

void esquivar (long latDeseada, long lonDeseada); //algoritmo para esquivar obstáculos, 
                          //que detiene al robot si los sensores IR detectan un 
                          //obstáculo, retrocede 0,4m, gira 90º en la dirección de 
                          //su destino y avanza 0,8m

void atras (); //retroceder en línea recta

int adelante (int centimetros); //avanzar en línea recta una distancia específica

void atras (int centimetros); //retroceder en línea recta una distancia específica

void parar (); //parar

void encoderD(); //rutina de atención a interrupción de encoder derecho 

void encoderI(); //rutina de atención a interrupción de encoder izquierdo

int orientar_robot  (long latDeseada, long lonDeseada, boolean calibrar); //algoritmo 
                      //de orientación del robot que incluye dos métodos.
                      //calibrar=true (se orienta el robot luego de calibrar 
                      //el magnetometro); calibrar=false(se orienta el robot 
                      //con la Ãºltima calibraciÃ³n)


  
#endif

