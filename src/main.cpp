
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