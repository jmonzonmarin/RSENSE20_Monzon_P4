
#include <WiFi.h>
#include "time.h"

const char* ssid;
const char* password;

const char* ssidMovil       = "AndroidAP_4359";
const char* passwordMovil   = "RSENSE-2020";

const char* ssidPC       = "PC-JORGE";
const char* passwordPC   = "01234567";

const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const uint16_t port = 21;
const char * host = "192.168.137.1";

String dataSockt;
boolean enviaHora = false;

tm timeinfo;

WiFiClient client;

void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void conexionWifi(const char* ssid, const char* password){
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
}

void recibeData(){
  char c[5];
  int i = 0;
  if (client){                      //client devuelve un valor booleano True si esta conectado
    while (client.available()>0) {
      c[i] = client.read();
      i++;
    }
    //Serial.println("c:");
    //Serial.println(c);
    dataSockt = String(c);
    //Serial.println("dataSockt:");
    //Serial.println(dataSockt);
  }
}

void setup()
{
  Serial.begin(115200);
 
  //Conexión Wifi con Internet
  conexionWifi(ssidMovil, passwordMovil);
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  //Me desconecto de la red Wifi del movil
  WiFi.disconnect(true);
  
  //Conexión Wifi en red local con el PC
  conexionWifi(ssidPC, passwordPC);
  
  client.connect(host, port);
  while (!client){
    Serial.println("Conectando al socket");
    client.connect(host, port);
  }
  if (client) {
    Serial.println("Socket abierto");
  }
  //WiFi.mode(WIFI_OFF);
}

void loop(){
  
  recibeData();
  if (dataSockt.startsWith("start")){
    enviaHora = true;
  } else if (dataSockt.startsWith("stop")){
    enviaHora = false;
  }

  if (enviaHora){
    delay(1000);
    getLocalTime(&timeinfo);
    client.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}
