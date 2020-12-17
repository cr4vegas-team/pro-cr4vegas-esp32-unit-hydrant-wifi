
# Proyecto: pro-cr4vegas-esp32-unit-hydrant-wifi

**Autor:** Rubén Francisco Gazquez Rosales
**Description:** Nodo final para las unidades de control de tipo hidrante
**Tipo de comunicación:** WiFi
**Placa:** ESP32

- [Proyecto: pro-cr4vegas-esp32-unit-hydrant-wifi](#proyecto-pro-cr4vegas-esp32-unit-hydrant-wifi)
  - [Librerías](#librerías)
  - [Configuración](#configuración)
  - [MQTT](#mqtt)
    - [Arrays de publicación soportados](#arrays-de-publicación-soportados)
    - [Arrays de subscripción soportados](#arrays-de-subscripción-soportados)
  - [PINES](#pines)
    - [Entradas](#entradas)
    - [Salidas](#salidas)

## Librerías

- [PubSubClient.h](https://github.com/knolleary/pubsubclient)
- [WiFi.h](https://www.arduino.cc/en/Reference/WiFi)

## Configuración

- **Tópico de subscripción:** server/unit/hydrant/xxx" --> "s/u/h/xxx
- **Tópico de publicación:** client/unit/hydrant/xxx" --> "n/u/h/xxx
- **Server MQTT:** mqtt.rubenfgr.com
- **Server MQTT puerto:** 1883
- **APN**
  - **Things-Mobile:** TM
  - **Movistar:** movistar.ts

## MQTT

p = propiedad
o = orden

### Arrays de publicación soportados

| cadena                                                              | descripción                               |
| ------------------------------------------------------------------- | ----------------------------------------- |
| 1                                                                   | publicar comunicación                     |
| 2,electrovalvula,caudal,lectura,boyaBaja,boyaMedia,boyaAlta,presión | publicar datos del estado de los sensores |
| 3,operador,señal,ip                                                 | publicar datos del estado de la SIM       |

### Arrays de subscripción soportados

|cadena|descripción|publicación|
|---|---|---|
|1|Test de comunicación|1|
|2|Envío del estado de los sensores|2|
|3|Envío del estado de la SIM|3|
|8,electroválvula|Recibir orden de apertura o cierre de la electroválvula||
|9,lectura|Configuración de la lectura||

## PINES

### Entradas

|Pin|Descripción|
|---|---|
|D25|Boya baja|
|D26|Boya media|
|D27|Boya alta|
|D33|captador contador|

### Salidas

|Pin|Descripción|
|---|---|
|D13|Electroválvula|
|D22|Sin conexión a la red (led rojo)|
|D23|Conectado a la red (led verde)|
