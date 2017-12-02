/*
ubicacion - Esta librería se desarrolló para el proyecto de fin de carrera, 
Robots Comunicados. Es la que incluye las funciones encargadas de 
establecer la comunicación con el magnetómetro y el GPS, del tratamiento 
de las medidas que de ellos se obtienen y funciones que permitan la 
comunicación capas arriba con Raspberry Pi.

Muchas de las funciones de esta librería hacen uso fundamental de funciones 
definidas en “TinyGPS", “I2Cdev " y “compass", pero se redefinieron nuevamente 
para hacer el tratamiento correspondiente de las medidas obtenidas y el manejo 
correcto de las variables globales.

Copyright (C) 2017 RoCo
All rights reserved.

Autores: Gabriel Bibbó, Mariana Gelóz, Martín Randall
*/

// Definitions ----------------------------------------------------------------  
#define numprom 35 
#define errorGPS 300 //este es el error mÃ¡ximo (en centÃ­metros) que se puede 
                     //obtener con una medida de GPS
#define sin_calibrar false
#define con_calibrar true
// Global Variables ----------------------------------------------------------
  extern int indice_promediosGPS;

void inicializacion_sensores(); //inicializar los sensores 

void calibrarMagne(); //calibrar el magnetómetro

void calibrarMagneizq(); //calibrar el magnetómetro (girando a la izquierda)

void ubicacionGPS(); //obtener la posición del GPS disminuyendo el error al 
                     //promediar múltiplos de 35 medidas

void ubicacionAngulo(); //calcular el ángulo del mangetómetro con respecto al 
                        //norte magnético

int calcularAnguloGiroConGPS(long latDeseada, long lonDeseada); //calcular el 
                        //ángulo que debe girar el robot para ir a una posición 
                        //deseada utilizando la información de su posición actual 
                        //obtenida del GPS

int calcularAnguloGiroSinGPS(); //Supone que previamente se utilizÃ³ calcularAnguloGiroConGPS y se 
                                //tiene guardado el Ã¡ngulo entre la posicion actual y la deseada ya 
                                //que el robot no se desplazÃ³ aun
                                //Devuelve Ã¡ngulo e (-180 +180) que debe girar el robot

long calcularDistancia(long latDeseada, long lonDeseada); //calcular la distancia en 
                                //centímetros que separa al robot de la posición deseada

boolean enviar_paquete(long *latDeseada, long *lonDeseada, int *calidad); //enviar la 
                                //posición actual del robot al Raspberry Pi e información 
                                //de si llegó a la posición deseada o aún está en 
                                //tránsito, y recibir la posición deseada actualizada 
                                //e información sobre la calidad de la señal.


