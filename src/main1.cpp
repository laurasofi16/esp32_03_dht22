#include <Arduino.h>
#include <WiFi.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>

#define DHTPIN 4 // Pin de datos del sensor DHT11/22

#define DHTTYPE DHT11 // Especificamos el tipo de sensor DHT11 DHT22 DHT21

DHT dht(DHTPIN, DHTTYPE); // Creamos el objeto que representa al sensor
const char *ssid = "lsgo";
const char *password = "1234abcd";
const char *host = "dweet.io";

String getTipoCifrado(wifi_auth_mode_t tipoCifrado)
{

  switch (tipoCifrado)
  {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA_PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2_PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA_WPA2_PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2_ENTERPRISE";
  }
}

void scanNetworks()
{

  int numberOfNetworks = WiFi.scanNetworks();

  Serial.print("Numero de redes encontradas: ");
  Serial.println(numberOfNetworks);

  for (int i = 0; i < numberOfNetworks; i++)
  {

    Serial.print("Nombre de red: ");
    Serial.println(WiFi.SSID(i));

    Serial.print("Fuerza de la señal: ");
    Serial.println(WiFi.RSSI(i));

    Serial.print("Direccion MAC: ");
    Serial.println(WiFi.BSSIDstr(i));

    Serial.print("Tipo de cifrado: ");
    String tipoCifradoDescription = getTipoCifrado(WiFi.encryptionType(i));
    Serial.println(tipoCifradoDescription);
    Serial.println("-----------------------");
  }
}

void connectToNetwork()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Estableciendo conexion WiFi..");
  }

  Serial.println("Conectado a la red");
}

void setup()
{

  Serial.begin(115200);
  dht.begin();

  scanNetworks();     // Escanear las redes WiFi disponibles
  connectToNetwork(); // Conectarse a la red que especificamos arriba

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  //  WiFi.disconnect(true);
  //  Serial.println(WiFi.localIP());

  Serial.println("Ensayo del DHT22!");
}

void loop()
{
  Serial.println("Conectandose a:");
  Serial.println(host);

  WiFiClient client;

  const int httpPort = 80;

  if (!client.connect(host, httpPort))
  {
    Serial.println("Conexion Fallida");
    delay(1000);
    return;
  }

  delay(2000);
  float humedad = dht.readHumidity();        // Leemos la humedad en % de humedad relativa (0 al 100%)
  float temperatura = dht.readTemperature(); // Leemos la temperatura en °C

  // Verificamos las lecturas del sensor
  if (isnan(humedad) || isnan(temperatura))
  {
    Serial.println("Fallo la lectura del sensor");
    return;
  }

  // Calculamos la sensacion termica o indice de calor en celsius
  float indice = dht.computeHeatIndex(temperatura, humedad, false);

  Serial.print(F("Humedad: ")); // La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
  Serial.print(humedad);
  Serial.print(F("%  Temperatura: ")); // La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
  Serial.print(temperatura);
  Serial.print(F("°C  Indice de calor: ")); // La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
  Serial.print(indice);

  // String cadena = "hola"+String("mundo")+String(variable);
  String url = "/dweet/for/uceva097?temperatura=" + String(temperatura) + "&humedad=" + String(humedad) + "&indice=" + String(indice);

  // Se pueden leer los dweets en  https://dweet.io/get/latest/dweet/for/uceva097

  Serial.print("Solicitando el recurso: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis(); // Leo la hora
  while (client.available() == 0)
  { // Mientras hayan cero letras disponibles...
    if (millis() - timeout > 5000)
    { // Resto la hora actual a la hora inicial, si es mas de 5s
      Serial.println("Expiro el tiempo de espera");
      client.stop();
      return;
    }
  }

  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }

  Serial.println("Fin de conexion");
  client.stop();
  delay(20000);
}