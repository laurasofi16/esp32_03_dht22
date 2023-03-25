#include <Arduino.h>
#include "libwifi.h"
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTPIN 15  //Pin de datos del sensor DHT11/22

#define DHTTYPE DHT11  //Especificamos el tipo de sensor DHT11 DHT22 DHT21

const char * ssid = "lsgo";
const char * password = "1234abcd";
const char * host = "dweet.io";
const int puerto = 80;
String url = "/dweet/for/uceva097?temperatura";
DHT dht(DHTPIN, DHTTYPE);  //Creamos el objeto que representa al sensor


void setup() {
  pinMode(15, OUTPUT);  //Coloco el pin 2 como salida
  Serial.begin(115200);
  Serial.println("Inicializando dispositivo");
  conectarWifi(ssid, password);
}


void loop() {
  WiFiClient cliente; //Creamos un cliente TCP por wifi
  if(!cliente.connect(host, puerto)){
    Serial.println("Error conexion al host fallida");
    delay(2000);
    return;
  }


   delay(2000);
   float humedad = dht.readHumidity(); //Leemos la humedad en % de humedad relativa (0 al 100%)
   float temperatura = dht.readTemperature(); //Leemos la temperatura en °C
   float tempFarenheit = dht.readTemperature(true); //Leemos la temperatura en grados farenheit
   
   //Verificamos las lecturas del sensor
   if(isnan(humedad) || isnan(temperatura) || isnan(tempFarenheit)){
     Serial.println("Fallo la lectura del sensor");
     return;
   }

   //Calculamos la sensacion termica o indice de calor
   float indice = dht.computeHeatIndex(tempFarenheit, humedad);

   //Calculamos la sensacion termica o indice de calor en celsius
   float indiceCelsius = dht.computeHeatIndex(temperatura, humedad, false);

   Serial.print(F("Humedad: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(humedad);
   Serial.print(F("%  Temperatura: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(temperatura);
   Serial.print(F("°C  Indice de calor: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(indice);
   Serial.print(F("°F  Indice de calor: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(indiceCelsius);
   Serial.println(F("°C")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   
   String url = "/dweet/for/uceva097?temperatura="+String(temperatura)+"&humedad="+String(humedad)+"&indice="+String(indice)+"";
  //Peticion (request) GET al servidor http
  Serial.print("Solicitando el recurso");
  Serial.print(url);
  cliente.print("POST"+url+" HTTP/1.1\r\nHost: "+String(host)+"\r\n"+"Connection: close\r\n\r\n");
  unsigned long milisegundos = millis(); //Hora de inicio
  while(cliente.available()==0){  //Preguntamos si no hay datos recibidos disponibles      
      if(millis()-milisegundos > 5000   ){
         Serial.println("Se expiro el tiempo de la conexion");
         cliente.stop();
      }
  }

  while(cliente.available()){
    String linea = cliente.readStringUntil('\r');
    Serial.println(linea);
  }

  Serial.println("Fin de la conexion");
  cliente.stop();

  delay(5000);  
}