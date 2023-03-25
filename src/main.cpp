#include "DHT.h"
#include <Arduino.h>
#include "libwifi.h"

const char * ssid = "lsgo";
const char * password = "1234abcd";
const char * host = "dweet.io";
const int puerto = 80;
String url= "https://dweet.io/dweet/for/uceva097?temperatura=33.6";

#define DHTPIN 4  //Pin de datos del sensor DHT11/22

#define DHTTYPE DHT11  //Especificamos el tipo de sensor DHT11 DHT22 DHT21

DHT dht(DHTPIN, DHTTYPE);  //Creamos el objeto que representa al sensor


void setup(){
   Serial.begin(9600);
   Serial.println("Ensayo del DHT22!");
   dht.begin();
   pinMode(2,OUTPUT); //Colocar un pin como entrada o como salida
  Serial.println("Inicializando dispositivo");
  conectarWifi(ssid, password);
}

void loop() { // creamos cliente TCP por wifi
  WiFiClient cliente;
   // conexion al servidor
   if(!cliente.connect(host, puerto)){
     Serial.println("Error conexion al host fallida");
     delay(2000);
     return;
   }
  //peticion GET al servidor HTTP (request)
   cliente.print("GET "+url+" HTTP/1.1\r\nHost: "+String(host)+"\r\n"+"Connection: close\r\n\r\n");

  //debemos darle tiempo al servidor para que responda la peticion (response)
   unsigned long milisegundos = millis(); //hora de inicio
    while (cliente.available()==0) //pregunta si hay datos disponibles
    {
      if (millis()-milisegundos > 5000){
      Serial.println("Se expiro el tiempo de la conexion");
      cliente.stop();
    }
    }
    
   while (cliente.available())
   {
    String linea = cliente.readStringUntil('\r');
    Serial.println(linea);
   }
   
   Serial.println("Fin de la conexion");
   cliente.stop();

   delay(2000);} 

void loop(){
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
}