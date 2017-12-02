#include "Wire.h"
#include "I2Cdev.h"
#include "SoftwareSerial.h"
#include "TinyGPS.h"
#include "ubicacion.h"
#include "motores.h"
#include "compass.h"

/////////////////////////////////////
////Variables de manejo de sensores//

long lat,lon,totlat,totlon,totlat2,totlon2;  //guardan la posicion tal como la entrega el GPS ( grados*1000000 )
float lat1,long1,lat2,long2; //guardan la posicion en grados
float angulo;
float anguloGPS;
int cuenta;
int indice_promediosGPS;
TinyGPS gps;
SoftwareSerial gpsSerial(8, 7); // crea la coneccion con el sensor gps       7=Rx del GPS; 8=Tx del GPS   
int16_t mx, my, mz;

////////////////////////////////////////////
////Variables de comunicacion Serial con PI/

int digito_verificador_Rx;
long lat3, lon3; //se usan para guardar la posiciÃ³n que envia el GPS
char latitud_char_Tx[8];
char longitud_char_Tx[8];
char latitud_char_Rx[9];
char longitud_char_Rx[9];
char paquete_Tx[18];
char lat_Tx[9];
char lon_Tx[9];
char payload[18];
boolean dato_en_canal;
int count =0;



///////////////////////////////////////////////
/////Funciones de UBICACION GPS////////////////


void inicializacion_sensores(){
      Wire.begin();
      gpsSerial.begin(9600); // conecta el sensor gps
}

void calibrarMagne(){
  compass_x_offset = -304.13;
  compass_y_offset = 319.38;
  compass_z_offset = 23.32;
  compass_x_gainError = 1.12;
  compass_y_gainError = 1.13;
  compass_z_gainError = 1.03;
  
  
  
  compass_init(2);
  compass_debug = 1;
  compass_offset_calibration(3);
}

void ubicacionGPS(){
    totlat=0;
    totlon=0;
    cuenta=0;
    if (indice_promediosGPS==0) {
        totlat2=0;
        totlon2=0;
    }
    long lat_temp, lon_temp;
    if (indice_promediosGPS<36) {
    while(cuenta<numprom){
      if(gpsSerial.available()){ // chequea que haya un dato en el gps
       if(gps.encode(gpsSerial.read())){ // encode gps data
        gps.get_position(&lat_temp,&lon_temp); // tiene latitud y longitud
        cuenta=cuenta+1;
        totlat=totlat+lat_temp;
        totlon=totlon+lon_temp;         //hace un promedio
        
       }
      }
      }
      totlat2=totlat2+totlat/numprom;
      totlon2=totlon2+totlon/numprom;
      indice_promediosGPS++;
    }
    lat=totlat2/indice_promediosGPS;
    lon=totlon2/indice_promediosGPS;
}

void ubicacionAngulo(){
        int i=0;
        for(i=0;i<7;i++){ 
        compass_heading();}
        int temp=bearing;
        temp=temp-declinacion; //corregimos la declinaciÃƒÂ³n magnÃƒÂ©tica
        angulo=temp;
}

int calcularAnguloGiroConGPS(long latDeseada, long lonDeseada){ //positivo a la derecha)
  ubicacionAngulo();
  lat1=latDeseada / 1000000;
  lat1=(-1)*(lat1+(latDeseada-lat1*1000000)/1000000);
  long1=lonDeseada / 1000000;
  long1=(-1)*(long1+(lonDeseada-long1*1000000)/1000000);
  lat2=lat / 1000000;
  lat2=(-1)*(lat2+(lat-lat2*1000000)/1000000);
  long2=lon / 1000000;
  long2=(-1)*(long2+(lon-long2*1000000)/1000000);
  anguloGPS=gps.course_to (lat1,long1,lat2,long2);
  int retorno=(int)round(anguloGPS-angulo);
  if ((retorno<-180)&&(retorno>=-360)) retorno=retorno+360;
  if ((retorno>180)&&(retorno<=360)) retorno=retorno-360;
  return retorno;
}

int calcularAnguloGiroSinGPS(){

    ubicacionAngulo();
    int retorno=(int)round(anguloGPS-angulo);
    if ((retorno<-180)&&(retorno>=-360)) retorno=retorno+360;
    if ((retorno>180)&&(retorno<=360)) retorno=retorno-360;
    Serial.print("calcularAnguloGiroSinGPS -> (anguloGPS - angulo) = ");
    Serial.println(retorno);
    return retorno;
}

long calcularDistancia(long latDeseada, long lonDeseada){ //devuelve la distancia entre (latDeseada,lonDeseada) y la posicion actual del robot
  //distance_between devuelve en metros, CalcularDistancia devuelve la distancia en centÃ­metros

  lat1=latDeseada / 1000000;
  lat1=(-1)*(lat1+(latDeseada-lat1*1000000)/1000000);
  long1=lonDeseada / 1000000;
  long1=(-1)*(long1+(lonDeseada-long1*1000000)/1000000);
  lat2=lat / 1000000;
  lat2=(-1)*(lat2+(lat-lat2*1000000)/1000000);
  long2=lon / 1000000;
  long2=(-1)*(long2+(lon-long2*1000000)/1000000);
  long c= 100*gps.distance_between (lat1, long1, lat2, long2);
  return c;
}


// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
/// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 

///////////////////////////////////////////////////////////////
////////COMUNICACION CON PI////////////////////////////////////

boolean enviar_paquete(long *latDeseada, long *lonDeseada, int *calidad) {

  lat_Tx[9]='\0';
  lon_Tx[9]='\0';
  payload[0] = '\0';
  boolean data = false;
  char aux = 'O';
  int i = 0;
  ubicacionGPS();
  
  ///ENVIAMOS MENSAJE////
  if (calcularDistancia(*latDeseada,*lonDeseada)>errorGPS) Serial.print("0"); //0=no llegÃ³, 1=llegÃ³
  else Serial.print("1");
  Serial.print((-1)*lat); Serial.print((-1)*lon);  Serial.println("%");  
  
  /// ESPERAMOS ALGUN TIEMPO HASTA RECIBIR EL MENSAJE ///
  unsigned long tiempoGPS=millis();
  while((!(Serial.available()>0))&&((millis()-tiempoGPS)<2000)){} //se queda trancado hasta recibir mensaje o hasta que pase el tiempo
  if ((Serial.available()>0)) { // si hay bytes recibidos en el buffer
    while (i<17 and aux != '%'){ // recibe hasta que llega un % (caracter de fin de comando                    // o hasta que llegan 19 bytes que es el espacio mÃ¡ximo del array
         aux = Serial.read();
         /*POR SIMPLICIDAD en el mensaje solo se aceptan numeros*/
         if ((aux>='0' && aux <= '9')) {
               payload[i] = aux;
               i++;
               payload[i] = '\0';    
         }
     }
     digito_verificador_Rx=payload[0]-'0';
     for (i=1;i<9;i++) {
       latitud_char_Rx[i-1]=payload[i];
     }
     for (i=9;i<17;i++){
       longitud_char_Rx[i-9]=payload[i];
     }

     if ((atol(latitud_char_Rx)>30000000)&&(atol(latitud_char_Rx)<40000000)) {
       *latDeseada=(-1)*atol(latitud_char_Rx);
       if ((atol(longitud_char_Rx)>50000000)&&(atol(longitud_char_Rx)<60000000)) {
         *lonDeseada=(-1)*atol(longitud_char_Rx);
         data = true;        
       }
     }
     *calidad=digito_verificador_Rx;
     
 }
  while(Serial.available()){  //borra monitor serial por si el PI manda un comando fuera de tiempo
      Serial.read();
  } 
  long temp=calcularDistancia(*latDeseada,*lonDeseada);
  if (temp>20000)  
      data=false;
  Serial.print("enviar_paquete --> DISTANCIA="); 
  Serial.println(temp);
  //se enviÃ³ el paquete pero la informacion se considera basura porque la distancia es mayor a 2 cuadras
  return data;
}


