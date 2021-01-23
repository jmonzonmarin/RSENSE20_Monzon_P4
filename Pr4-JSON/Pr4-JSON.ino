#include <ArduinoJson.h>
#include <SD.h>
#include "WiFi.h"
#include <ESP32_FTPClient.h>

//variables necesarias JSON
const int capacity = JSON_ARRAY_SIZE(100) + 100*JSON_OBJECT_SIZE(4);    
StaticJsonDocument<capacity> doc;

//variables necesarias Timer
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//variables Wifi
const char* ssid       = "AndroidAP_4359";
const char* password   = "RSENSE-2020";

//Variables FTP
char ftp_server[] = "155.210.150.77";
char ftp_user[]   = "rsense";
char ftp_pass[]   = "rsense";
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 2);

//variables auxiliares
int interruptCounter = 0;
volatile int numeroMedidas = 0;
int numeroMedidasAnt = 0;
int i = 0;
String datos;

char datosChar[capacity];

void almacenaDatos() {
  JsonObject obj1 = doc.createNestedObject();
    obj1["n"] = "esp32";
    obj1["u"] = "C";
    obj1["t"] = "tiempo";
    obj1["v"] = random(10,30);;
}

void escribeFichero() {
  serializeJson(doc, datosChar);          
  doc.clear(); 
}

void mandaFichero(){
  ftp.OpenConnection();
  //cambiando al directorio
    ftp.ChangeWorkDir("rsense/jmonzon");
    ftp.InitFile("Type A");
    ftp.NewFile("dataTemp.json");
    ftp.Write(datosChar);
  ftp.CloseFile();
  ftp.CloseConnection();
}

void IRAM_ATTR onTimer() {              //Esta función incrementa el contador de interrupciones. Al aumentar el contador, el loop principal sabe que ha ocurrido una interrupción
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  almacenaDatos();
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);

  //lineas necesarias para el timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);        //Genera una interrupción 1 vez por cada 0,1s
  timerAlarmEnable(timer);

  //conexión a wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  
}

void loop() {
  if (interruptCounter == 100) {
    Serial.println("Mostrando fichero JSON:");
    escribeFichero();
    Serial.println(datosChar);
    mandaFichero();
    Serial.println("");
    interruptCounter = 0;
  }
}
