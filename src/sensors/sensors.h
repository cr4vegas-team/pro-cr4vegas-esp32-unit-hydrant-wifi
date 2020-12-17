
#include <Arduino.h>

void setupSensors();
void loopSensors();

float getCaudal();
uint32_t getLectura();
int getElectrovalvula();
void setElectrovalvula(int electrovalvula);
uint32_t getBoyaBaja();
uint32_t getBoyaMedia();
uint32_t getBoyaAlta();
uint32_t getTemperature();
uint32_t getHumidity();

void setLectura(uint32_t lecturaP);

int getCambioBoyas();
void setCambioBoyas(int cambioBoyas);
int getCambioCaudal();
void setCambioCaudal(int cambioCaudalP);
int getCambioElectrovalvula();
void setCambioElectrovalvula(int cambioElectrovalvulaP);

float getPressure();