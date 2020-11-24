#include <ArduinoJson.h>

const int capacity = JSON_ARRAY_SIZE(100) + 100*JSON_OBJECT_SIZE(4);    //El tamaño del array indica el numero de medidas que queremos guardar en el fichero.
//El tamaño del objeto nos dice el número de valores que habrá dentro del objeto. Al estar usando el prtocolo SenML
// vamos a utilizar 4 valores: n (nombre), u (unidades), t (tiempo), v (valor)
StaticJsonDocument<capacity> doc;


hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int interruptCounter = 0;
volatile int numeroMedidas = 0;
int numeroMedidasAnt = 0;
int i = 0;
String json;

void almacenaDatos() {
  JsonObject obj1 = doc.createNestedObject();
    obj1["n"] = "esp32";
    obj1["u"] = "C";
    obj1["t"] = "tiempo";
    obj1["v"] = random(10,30);;
}

void escribeFichero() {
  serializeJson(doc, json);          
  doc.clear(); 
}

void IRAM_ATTR onTimer() {              //Esta funión incrementa el contador de interrupciones. Al aumentar el contador, el loop principal sabe que ha ocurrido una interrupción
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  almacenaDatos();
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);        //Genera una interrupción 1 vez por cada 0,1s
  timerAlarmEnable(timer);

}

void loop() {

  if (interruptCounter == 100) {
    Serial.println("Mostrando fichero JSON:");
    escribeFichero();
    Serial.println(json);
    json.clear();
    Serial.println("");
    interruptCounter = 0;
  }
}
