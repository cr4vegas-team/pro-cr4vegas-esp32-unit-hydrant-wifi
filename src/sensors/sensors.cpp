#include <EEPROM.h>
#include "./contador/contador.h"
#include "./electrovalvula/electrovalvula.h"
#include "./boyas/boyas.h"
#include "../global/global.h"

float caudal = 0;
uint32_t lectura = 0;

int electrovalvula = 0;

int boyaBaja = 0;
int boyaMedia = 0;
int boyaAlta = 0;

int cambioBoyas = 0;
int cambioCaudal = 0;
int cambioElectrovalvula = 0;

float pressure = 0;

uint8_t FLASH_SIZE = 50;

uint8_t LECTURA_FLASH_POSITION = 0;

uint32_t lastSaved;
uint16_t TIME_TO_SAVED = 1000;

void loopSave();

void setupSensors()
{
    EEPROM.begin(FLASH_SIZE);
    lectura = EEPROM.readLong(LECTURA_FLASH_POSITION);
    setupContador();
    setupElectrovalvula();
    setupBoyas();
    lastSaved = 0;
}

void loopSensors()
{
    loopBoyas(boyaBaja, boyaMedia, boyaAlta, cambioBoyas);
    if (boyaMedia == 1 || boyaAlta == 1)
    {
        electrovalvula = 0;
    }
    loopElectrovalvula(electrovalvula, cambioElectrovalvula);
    loopContador(caudal, lectura, cambioCaudal);
    loopSave();
}

void loopSave()
{
    uint32_t t = millis();
    if (t - lastSaved > TIME_TO_SAVED)
    {
        EEPROM.writeLong(LECTURA_FLASH_POSITION, lectura);
        EEPROM.commit();
        lastSaved = t;
    }
}

float getCaudal()
{
    return caudal;
}

uint32_t getLectura()
{
    return lectura;
}

int getElectrovalvula()
{
    return electrovalvula;
}

void setElectrovalvula(int electrovalvulaP)
{
    electrovalvula = electrovalvulaP;
}

uint32_t getBoyaBaja()
{
    return boyaBaja;
}

uint32_t getBoyaMedia()
{
    return boyaMedia;
}

uint32_t getBoyaAlta()
{
    return boyaAlta;
}

void setLectura(uint32_t lecturaP)
{
    lectura = lecturaP;
}

int getCambioBoyas()
{
    return cambioBoyas;
}

void setCambioBoyas(int cambioBoyasP)
{
    cambioBoyas = cambioBoyasP;
}

int getCambioCaudal()
{
    return cambioCaudal;
}

void setCambioCaudal(int cambioCaudalP)
{
    cambioCaudal = cambioCaudalP;
}

int getCambioElectrovalvula()
{
    return cambioElectrovalvula;
}

void setCambioElectrovalvula(int cambioElectrovalvulaP)
{
    cambioElectrovalvula = cambioElectrovalvulaP;
}

float getPressure()
{
    return pressure;
}