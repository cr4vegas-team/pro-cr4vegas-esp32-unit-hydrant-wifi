
#include <Arduino.h>
#include "../../global/global.h"

// PIN CONTADOR PULSOS
const uint8_t PIN_COUNTER = 33;
const uint32_t CHANGE_INTERVAL = 30000; // Intervalo para registrar un cambio (30 segundos)
const uint8_t MIN_CHANGE = 5;           // Valor mínimo en el cambio del caudal para lanzar un evento

/*
    t1 = marca de tiempo actual para cada iteración
    t2 = última marca de tiempo cuando existe pulso y no se ha registrado
    t3 = última marca de tiempo para el intervalo de resta de caudal
    interval = intervalo de tiempo entre t1 y t2
    lastChange = última marca de tiempo de un cambio en el caudal mayor a 5
    readed = se ha registrado o no el último pulso
    pulse = lectura del pulso por pin digital
    restarInterval = intervalo para restar el caudal
    restar = cantidad a restar en cada iteración del intervalo para restar
    caudalNew = caudal actual en cada iteración
*/
unsigned long t1, t2, t3, interval, lastChange = 0;
bool readed = false;
int pulse = LOW;
float restarInterval = 0.00;
float restar = 0.00;
float caudalNew = 0.00;

/*
    Inicio del pin del contador como entrada

*/
void setupContador()
{
    pinMode(PIN_COUNTER, INPUT);
    t1 = millis();
    t2 = t1;
}

void loopContador(float &caudal, uint32_t &lectura, int &cambioCaudal)
{
    t1 = millis();
    pulse = digitalRead(PIN_COUNTER);

    /*
        Si existe pulso y no se ha registrado:
            - Se suma la lectura en 1
            - Se cambio el estado de "leída" a "true"
            - Se calcula el intervalo desde el último registro hasta el actual
            - Si el intervalo es distinto de 0 se calcula el caudal
                1 pulso son 1000 litros
                Utilizo la regla de tres para pasar de litros/intervalo a litros/segundo
                1000 l * 1000 ms (1 segundo) / intervalo
            - Didivo la cantidad a restar de caudal e intervalo entre 10
            - Asigno el tiempo del pulso actual al tiempo del último pulso
        Si NO existe pulso cambio el estado de "leída" a "false"
    */
    if (pulse == HIGH)
    {
        if (!readed)
        {
            lectura++;
            readed = true;
            interval = (t1 - t2);
            if (interval != 0)
            {
                caudalNew = (float)1000 * 1000 / interval;
                restar = caudalNew / 10;
                restarInterval = interval / 10;
                printDebug("Caudal: " + (String)caudalNew + " l/s, Lectura: " + (String)lectura + " m3");
            }
            t2 = t1;
        }
    }
    else
    {
        readed = false;
    }

    /* 
        Si la marca de tiempo actual "menos" la marca de tiempo del último pulso es mayor
        que el intervalo y 
        La marca de tiempo actual "menos" la marca de tiempo del ultimo restar intervalo es
        mayor que el intervalo para restar y
        el intervalo para restar es mayor a 1 (el 1 es para evitar los microsegundos entre pulsos)
            - Se iguala la marca de tiempo del intervalo para restar a la actual
            - Si el caudal es mayor o igual a 0
                - Si el caudal - restar es menor o igual a 0
                    - el caudal se iguala a 0
                        Si el caudal actual es diferente al anterior:
                        Si la marca de tiempo actual menos la última marca de tiempo de cambio es
                        mayor al intervalo de cambio se iguala esta última a la actual y se 
                        registra un evento de cambio
                        (eso se hace para evitar lanzar eventos mientras el caudal este a 0)
                - Si no se resta el caudal
            - Si el caudal es menor a 0 se iguala a 0
        
    */
    if (t1 - t2 > interval && t1 - t3 > restarInterval && restarInterval > 1)
    {
        t3 = t1;
        if (caudalNew >= 0)
        {
            if ((caudalNew - restar) <= 0)
            {
                caudalNew = 0;
                if (caudalNew != caudal)
                {
                    if (t1 - lastChange > CHANGE_INTERVAL)
                    {
                        lastChange = t1;
                        cambioCaudal = 1;
                        // Actualizo el último caudal registrado al actual
                        caudal = caudalNew;
                    }
                }
            }
            else
            {
                caudalNew -= restar;
            }
        }
        else
        {
            caudalNew = 0;
        }
    }

    // Si el valor absoluto del caudal actual menos el anterior es mayor a 4.00 o
    // el caudal es igual a 0 y el nuevo caudal es mayor a 0
    if (abs(caudalNew - caudal) > MIN_CHANGE || (caudal == 0 && caudalNew > 0))
    {
        // y si la marca actual menos la marca del último cambio registrado es mayor que el intervalo de cambio
        if (t1 - lastChange > CHANGE_INTERVAL)
        {
            lastChange = t1;
            cambioCaudal = 1;
            // Actualizo el último caudal registrado al actual
            caudal = caudalNew;
        }
    }
}
