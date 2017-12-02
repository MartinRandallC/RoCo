#include "Arduino.h"
#include "motores.h"
#include "ubicacion.h"

long  cuentaD;
long  cuentaI;
int nollega=0;

void inicializar(){
  cuentaD=0;
  cuentaI=0;
  attachInterrupt(1,encoderD,FALLING);
  attachInterrupt(0,encoderI,FALLING);
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 10 
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 9
  pinMode (IN2, OUTPUT);    // Input2 conectada al pin 6 
  pinMode (IN1, OUTPUT);    // Input1 conectada al pin 5
  pinMode(pinIR1, INPUT);
  pinMode(pinIR2, INPUT);
  parar();
}



void adelante () {
 indice_promediosGPS=0;
 analogWrite (IN4, 0);
 analogWrite (IN3, velAvanceDer);
 analogWrite (IN2, 0);
 analogWrite (IN1, velAvanceIzq);
}


void atras () {
 indice_promediosGPS=0;
 analogWrite (IN3, 0);
 analogWrite (IN4, velAvanceDer);
 analogWrite (IN1, 0);
 analogWrite (IN2, velAvanceIzq);
}


void parar (){
 analogWrite (IN1, 0);
 analogWrite (IN2, 0);
 analogWrite (IN3, 0);
 analogWrite (IN4, 0);
}



void izquierda (int grados) {
  unsigned long  tiempo=millis();
  nollega=1;
  cuentaD=0;
  cuentaI=0;
  attachInterrupt(1,encoderD,FALLING);
  attachInterrupt(0,encoderI,FALLING);
 while(((millis()-tiempo)<80000)&&(nollega)){
   if(cuentaD<constGiroIzq*grados){ 
     if (robot==1){ //robot chico  
       analogWrite (IN2, 0);
       analogWrite (IN1, velgiro); 
     }
     if (robot==2){ //robot grande
       analogWrite (IN1, 0);
       analogWrite (IN2, velgiro);  
     }
   }
   if(cuentaI<constGiroIzq*grados){
     if (robot==1){ //robot chico 
       analogWrite (IN3, 0); 
       analogWrite (IN4, velgiro);
     }
     if (robot==2){ //robot grande       
       analogWrite (IN4, 0);
       analogWrite (IN3, velgiro);
     }
   }
   if((cuentaD>=constGiroIzq*grados)&&(cuentaI>=constGiroIzq*grados)){
   nollega=0;
   }
 }
 parar();
 detachInterrupt(1);
 detachInterrupt(0);  
}


void derecha (int grados) {  
  unsigned long  tiempo=millis();
  nollega=1;
  cuentaD=0;
  cuentaI=0;
  attachInterrupt(1,encoderD,FALLING);
  attachInterrupt(0,encoderI,FALLING);
 while(((millis()-tiempo)<80000)&&(nollega)){
   if(cuentaD<constGiroDer*grados){
     if (robot==1){ //robot chico    
       analogWrite (IN1, 0);
       analogWrite (IN2, velgiro); 
     }
     if (robot==2){ //robot grande 
       analogWrite (IN2, 0);
       analogWrite (IN1, velgiro);     
     }
   }
   if(cuentaI<constGiroDer*grados){
     if (robot==1){ //robot chico 
       analogWrite (IN4, 0); 
       analogWrite (IN3, velgiro);
     }
     if (robot==2){ //robot grande
       analogWrite (IN3, 0); 
       analogWrite (IN4, velgiro); 
     }
   }
   if((cuentaD>=constGiroDer*grados)&&(cuentaI>=constGiroDer*grados)){
   nollega=0;   
   }
 }
 parar();
 detachInterrupt(1);
 detachInterrupt(0);  
}



void giroLentoder(int velgiroL) {
   if (robot==1){ //robot chico      
     analogWrite (IN1, 0);
     analogWrite (IN2, velgiroL); 
     analogWrite (IN4, 0); 
     analogWrite (IN3, velgiroL);
   }
   if (robot==2){ //robot grande
     analogWrite (IN2, 0);
     analogWrite (IN1, velgiroL);  
     analogWrite (IN3, 0); 
     analogWrite (IN4, velgiroL);
   }
}

void giroLentoizq(int velgiroL) {
   if (robot==1){ //robot chico  
     analogWrite (IN2, 0);
     analogWrite (IN1, velgiroL);  
     analogWrite (IN3, 0); 
     analogWrite (IN4, velgiroL);
   }
   if (robot==2){ //robot grande
     analogWrite (IN1, 0);
     analogWrite (IN2, velgiroL); 
     analogWrite (IN4, 0); 
     analogWrite (IN3, velgiroL);
   }
}

int adelante (int centimetros) {
   indice_promediosGPS=0;
   unsigned long  tiempo=millis();
   int distRecorrida=0;
   nollega=1;
   cuentaD=0;
   cuentaI=0;
   attachInterrupt(1,encoderD,FALLING);
   attachInterrupt(0,encoderI,FALLING);
   while (((millis()-tiempo)<240000)&&(nollega)&&(digitalRead(pinIR1))&&(digitalRead(pinIR2))){
      adelante ();
      distRecorrida=((cuentaD+cuentaI)/2)*constVel;
      if (distRecorrida>=centimetros) nollega=0;
      Serial.print("Distancia Recorrida = ");
      Serial.println(distRecorrida);
   }
   detachInterrupt(1);
   detachInterrupt(0);
   parar();
   if (digitalRead(pinIR1)) 
     return 1; //IR 1 tocando
   if (digitalRead(pinIR2)) 
     return 2; //IR 2 tocando
   if ((millis()-tiempo)>240000) 
     return 5; //ERROR, salimos del loop por overtime
   if (!(nollega)) 
     return 0; //OK, llegamos
}

void esquivar(long latDeseada, long lonDeseada) {
  if (digitalRead(pinIR1)&&(!digitalRead(pinIR2))) {
    atras(40);
    derecha(85);
    adelante(80);
  }
  if ((!digitalRead(pinIR1))&&digitalRead(pinIR2)) {
    atras(40);
    izquierda(85);
    adelante(80);
  }
  if (!(digitalRead(pinIR1))&&(!(digitalRead(pinIR2)))) {
    atras(80);
    if (calcularAnguloGiroConGPS(latDeseada,lonDeseada)>0) {
      derecha(90);
      adelante(80);
    }
    else {
      izquierda(90);
      adelante(80);
    }
  }
}

void atras (int centimetros) {
   indice_promediosGPS=0;
   unsigned long  tiempo=millis();
   int distRecorrida=0;
   cuentaD=0;
   cuentaI=0;
   attachInterrupt(1,encoderD,FALLING);
   attachInterrupt(0,encoderI,FALLING);
   while (((millis()-tiempo)<240000)&&(distRecorrida<centimetros)){
     atras();
     distRecorrida=((cuentaD+cuentaI)/2)*constVel;
   }
   detachInterrupt(1);
   detachInterrupt(0);
   parar();
}


void encoderD(){
  cuentaD++;
}

void encoderI(){
  cuentaI++;
}


int orientar_robot (long latDeseada, long lonDeseada, boolean calibrar) {
  long cuentaDsave=cuentaD; //guardo las variables para que no se borren en el cÃ¡lculo de la distancia recorrida del loop principal
  long cuentaIsave=cuentaI;
  unsigned long tiempo;
  int angulos;
  int intentos;
  int metodo_que_funciona=0; //0=no se ha orientado con ningun metodo; 1=se orientÃ³ con mÃ©todo 1; 2= se orientÃ³ con metodo 2
  Serial.println("Orientando Robot...");
  if (calibrar) calibrarMagne();
  parar();
  ubicacionAngulo();
  delay(10);
  ubicacionAngulo();
  delay(10);
  angulos = calcularAnguloGiroSinGPS();
  
//// METODO 2 de orientacion ///////
if ((metodo_que_funciona==0)) {
  Serial.println("METODO DE ORIENTACION 2");
  angulos = calcularAnguloGiroConGPS(latDeseada,lonDeseada);
  intentos=52;
  while ((abs(angulos)>ToleranciaAngulo)&&(intentos>0)) {
     Serial.print("Angulo = ");
     Serial.println(angulos);
     Serial.print(calcularDistancia(latDeseada,lonDeseada));
     Serial.println("cm de distancia");
     tiempo=millis();
     if (angulos<(-1)*ToleranciaAngulo) {
       angulos = calcularAnguloGiroSinGPS();
       angulos = calcularAnguloGiroSinGPS();
       angulos = calcularAnguloGiroSinGPS();
       Serial.println("GIRA Izq");
       giroLentoizq(velgiroLento);
       angulos = calcularAnguloGiroSinGPS();
       while((angulos<(-1)*(ToleranciaAngulo+5))&&((millis()-tiempo)<(intentos*60)))
          angulos = calcularAnguloGiroSinGPS();
     }
     else {
       angulos = calcularAnguloGiroSinGPS();
       angulos = calcularAnguloGiroSinGPS();
       angulos = calcularAnguloGiroSinGPS();
       Serial.println("GIRA Der");
       giroLentoder(velgiroLento);
       angulos = calcularAnguloGiroSinGPS();
       while((angulos>(ToleranciaAngulo+5))&&((millis()-tiempo)<intentos*60)) 
           angulos = calcularAnguloGiroSinGPS();
     }
     parar();
     intentos=intentos-5;       
     delay(20);
     tiempo = millis();
     while (((millis()-tiempo)<20000)&&(calcularAnguloGiroSinGPS()==angulos)){ //esto querrÃ­a decir que se trancÃ³ el magnetometro y se debe esperar a que se destranque en menos de 20s
        delay(20);
        ubicacionAngulo();
        delay(20);
        ubicacionAngulo();
        delay(20);
        ubicacionAngulo();
     }
     angulos = calcularAnguloGiroSinGPS(); 
  }
  if (abs(angulos)<=ToleranciaAngulo) metodo_que_funciona=2;
}
//////fin metodo 2//////////  
  
  
////// METODO 1 de orientacion /////
if ((metodo_que_funciona==0)&&(robot==3)){
  Serial.println("METODO DE ORIENTACION 1");
  angulos = calcularAnguloGiroConGPS(latDeseada,lonDeseada);
  intentos=1;
  while ((abs(angulos)>ToleranciaAngulo)&&(intentos<10)){   //definida en motores.h
    Serial.print("Angulo = ");
    Serial.println(angulos);
    Serial.print(calcularDistancia(latDeseada,lonDeseada));
    Serial.println("cm de distancia");
    if (angulos<0){
      Serial.println("Izquierda");
          izquierda(abs(angulos));
    }
    if (angulos>0){
      Serial.println("Derecha");
      derecha (angulos);
    }
    delay(1000);
    //angulos = calcularAnguloGiroConGPS(latDeseada,lonDeseada);
    tiempo = millis();
    while (((millis()-tiempo)<20000)&&(calcularAnguloGiroSinGPS()==angulos)){ //esto querrÃ­a decir que se trancÃ³ el magnetometro y se debe esperar a que se destranque en menos de 20s
      delay(100);
      calcularAnguloGiroSinGPS();
      delay(100);
      calcularAnguloGiroSinGPS();
      delay(100);
      calcularAnguloGiroSinGPS();
    } 
    angulos = calcularAnguloGiroSinGPS();
    intentos++;
  }
  if (abs(angulos)<=ToleranciaAngulo) metodo_que_funciona=1;
}
//////////fin metodo 1////////////
  
  parar();
  cuentaD=cuentaDsave; //reintegro las variables para poder seguir con el cÃ¡lculo de distancia en el loop principal
  cuentaI=cuentaIsave;
  angulos = calcularAnguloGiroSinGPS(); 
  if (abs(angulos)<=ToleranciaAngulo){   //definida en motores.h
    Serial.print("PARE!!! Angulo = ");   Serial.println(angulos);
    Serial.print("funciono con el metodo "); Serial.println(metodo_que_funciona);
    delay(3000);
    return 1;
  }
  else {
    Serial.print("NO LLEGO :/    Angulos=");   Serial.println(angulos);
    Serial.print("metodo_que_funciona="); Serial.println(metodo_que_funciona);
    delay(500);
    return 0;
  }
}

