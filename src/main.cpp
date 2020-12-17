/**************************************************************
 *  Proyecto: pro-cr4v-esp32-unit-generic-sim
 *  Autor: Rubén Francisco Gazquez Rosales
 *  Description: Nodo final para las unidades de control de hidrantes.
 *    Su uso es exclusivo para WIFI.
 *  Enlaces de las librerías utilizadas:
 *    - https://github.com/knolleary/pubsubclient
 *    - http://librarymanager/all#PubSubClient
 *    - https://tiny.cc/tinygsm-readme
 *  Placas: ESP32
 * 
 *  CONFIGURACIÓN:
 *  Tópico de subscripción: "server/unit/hydrant/xxx" --> "s/u/h/xxx"
 *  Tópico de publicación:  "client/unit/hydrant/xxx" --> "n/u/h/xxx"
 *  Server MQTT:            "mqtt.rubenfgr.com"
 *  APN Things-Mobile:      "TM"
 * 
 *  MQTT
 *  p = propiedad
 *  o = orden
 *  Arrays de publicación soportados:
 *    - 1 --> pub[status,comunicación]                      (publicar comunicación)
 *    - 2 --> pub[status,valve,flow,reading,bouylow,bouymedium,bouyhight,pressure]       (publicar datos de sensores)
 *    - 7 --> pub[7,velocidad]                   (publicar velocidad de envio de datos)
 *    - 8 --> pub[8,operador,señal,ip]    (publicar datos de la sim)
 *  Arrays de subscripción soportados:
 *    - 1 --> sub[1]                -->   pub(1)  (subcripción de comunicación)
 *    - 2 --> sub[2]                -->   pub(2)  (subcripción de datos de sensores)
 *    - 3 --> sub[3,valvula]    -->   pub(2)  (subcripción de ordenes a actuadores)
 *    - 7 --> sub[7,ss]             -->   pub(7)  (subcripción de configuración de velocidad de envio de datos)
 *    - 8 --> sub[8]                -->   pub(8)  (subcripción de datos de la sim)
 *    - 9 --> sub[9,p1,p2,p3,p4,p5] -->   pub(2)  (subcripción de configuración)
 * 
 *  PINES:
 *    ENTRADAS (outputs):
 *      - D2    = electroválvula
 * 
 *      - D4    = balsa led rojo
 *      - D5    = balsa led azul
 *      - D18   = balsa led verde
 *      - D19   = balsa led amarillo
 *      
 *      - D21   = comunicación led rojo
 *      - D22   = comunicación led azul
 *      - D23   = comunicación led verde
 *      - D34   = comunicación led amarillo
 * 
 *    SALIDAS (inputs):
 *      - D15   = caudalímetro
 **************************************************************/
#include <Arduino.h>
#include "global/global.h"
#include "sensors/sensors.h"
#include "wifi/wifi.h"

TaskHandle_t taskSensors;
TaskHandle_t taskWIFI;

void runTaskSensors(void *pvParameters);
void runTaskWIFI(void *pvParameters);

void setup()
{
  SerialMon.begin(115200);
  SerialMon.println("setup()..");

  xTaskCreatePinnedToCore(runTaskWIFI, "Task WIFI", 20048, NULL, 1, &taskWIFI, 1);
  xTaskCreatePinnedToCore(runTaskSensors, "Task Sensors", 20048, NULL, 1, &taskSensors, 0);
}

void runTaskWIFI(void *pvParameters)
{
  printDebug("runTaskWIFI running on core: " + (String)xPortGetCoreID());

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 1000;

  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  setupWIFI(xLastWakeTime);

  vTaskDelayUntil(&xLastWakeTime, xFrequency);
  /*     char *pcWriteBuffer;
    vTaskGetRunTimeStats(pcWriteBuffer);
    printDebug(pcWriteBuffer); */
  for (;;)
  {
    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Perform action here.
    loopWIFI();
  }
}

void runTaskSensors(void *pvParameters)
{
  printDebug("runTaskSensors running on core: " + (String)xPortGetCoreID());

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 10;

  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  setupSensors();

  vTaskDelayUntil(&xLastWakeTime, xFrequency);

  for (;;)
  {
    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Perform action here.
    loopSensors();
  }
}

void loop()
{
  // This must be empty
}