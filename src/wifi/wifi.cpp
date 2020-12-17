#include "../global/global.h"
#include "../sensors/sensors.h"

#include <Wifi.h>
#include <PubSubClient.h>

// D21   = comunicación led rojo
// D22   = comunicación led azul
// D23   = comunicación led verde
// D34   = comunicación led amarillo
const uint8_t PIN_LED_COM_RED = 21;
const uint8_t PIN_LED_COM_AZUL = 22;
const uint8_t PIN_LED_COM_VERDE = 23;
const uint8_t PIN_LED_COM_AMARILLO = 32;

// Your WiFi connection credentials, if applicable
const char *ssid = "cuartillo";
const char *password = "cuartillo";

WiFiClient modem;
PubSubClient mqtt(modem);

const uint8_t WIFI_ATTEMPTS_MAX = 3;

// ==================================================
// TODO MQTT VAR DECLARATIONS
// ==================================================
const char *broker = "emqx.rubenfgr.com";
const char *topicSub = "s/u/h/1"; // server/unit/hydrant/id
const char *topicPub = "n/u/h/1"; // node/unit/hydrant/id

char UNIT_HYDRANT_ID[] = "uh1";
uint16_t MQTT_RECONNECT_ATTEMPTS_MAX = 5;
uint32_t RECCONECT_TIME = 10000;

uint32_t lastMQTTReconnectAttempt = 0;
uint8_t mqttReconnectAttempts = 0;

bool publishedInit = false;
bool publishedCommunication = true;
bool publishedData = true;
bool publishedWIFIData = true;

TickType_t xLastWakeTime;

// ==================================================
// TODO WIFI FUNCTIONS DECLARATIONS
// ==================================================
void setupWIFI(TickType_t xLastWakeTime);
void loopWIFI();
void loopMQTT();

// ==================================================
// TODO MQTT FUNCTIONS DECLARATIONS
// ==================================================
void subscribeOrder(String payload[]);
void mqttCallback(char *topic, byte *payload, unsigned int len);
void publish();
boolean mqttConnect();
void readOrders(String payload[]);
void readConfiguration(String payload[]);
void publishInit();
void publishCommunication();
void publishData();
void publishWIFIData();

// ==================================================
//  TODO WIFI FUNCTIONS
// ==================================================
void setupWIFI(TickType_t xLastWakeTimeP)
{
    printDebug((String)xLastWakeTimeP);
    xLastWakeTime = xLastWakeTimeP;
    printDebug("setupWIFI()");
    pinMode(PIN_LED_COM_RED, OUTPUT);
    pinMode(PIN_LED_COM_AZUL, OUTPUT);
    pinMode(PIN_LED_COM_VERDE, OUTPUT);
    pinMode(PIN_LED_COM_AMARILLO, OUTPUT);

    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);
}

void loopWIFI()
{
    loopMQTT();
    if (getCambioBoyas() == 1)
    {
        publishedData = false;
        setCambioBoyas(0);
    }

    if (getCambioCaudal() == 1)
    {
        publishedData = false;
        setCambioCaudal(0);
    }

    if (getCambioElectrovalvula() == 1)
    {
        publishedData = false;
        setCambioElectrovalvula(0);
    }
}

void initWIFI()
{
    digitalWrite(PIN_LED_COM_AMARILLO, LOW);
    digitalWrite(PIN_LED_COM_VERDE, LOW);
    digitalWrite(PIN_LED_COM_AZUL, LOW);

    digitalWrite(PIN_LED_COM_RED, HIGH);
    // vTaskDelayUntil(&xLastWakeTime, 5000);

    // Nos conectamos a nuestra red Wifi
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    digitalWrite(PIN_LED_COM_RED, LOW);
    digitalWrite(PIN_LED_COM_AMARILLO, HIGH);
    // vTaskDelayUntil(&xLastWakeTime, 2000);
    digitalWrite(PIN_LED_COM_AMARILLO, LOW);

    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(PIN_LED_COM_VERDE, HIGH);
        // vTaskDelayUntil(&xLastWakeTime, 1000);
        digitalWrite(PIN_LED_COM_VERDE, LOW);
        // vTaskDelayUntil(&xLastWakeTime, 1000);
        Serial.print(".");
    }

    digitalWrite(PIN_LED_COM_VERDE, HIGH);

    Serial.println("");
    Serial.println("Conectado a red WiFi!");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void loopMQTT()
{

    if (!mqtt.connected())
    {
        digitalWrite(PIN_LED_COM_AZUL, LOW);
        if (!WiFi.isConnected())
        {
            initWIFI();
        }
        printDebug("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastMQTTReconnectAttempt > RECCONECT_TIME)
        {
            lastMQTTReconnectAttempt = t;
            if (mqttConnect())
            {
                lastMQTTReconnectAttempt = 0;
            }
        }
        return;
    }
    else
    {
        digitalWrite(PIN_LED_COM_AZUL, HIGH);
        publish();
    }

    mqtt.loop();
}

boolean
mqttConnect()
{
    if (mqttReconnectAttempts == MQTT_RECONNECT_ATTEMPTS_MAX)
    {
        mqttReconnectAttempts = 0;
        initWIFI();
    }
    mqttReconnectAttempts++;

    printDebug("mqttConnect()...");

    boolean status = mqtt.connect(UNIT_HYDRANT_ID);

    // Con autenticación
    //boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

    if (status == false)
    {
        printDebug("mqttConnect() return false");
        return false;
    }
    printDebug("mqttConnect() return true");

    mqtt.subscribe(topicSub);
    return mqtt.connected();
}

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    printDebug("mqttCallback()");

    unsigned int commas = 1;

    for (uint8_t i = 0; i < len; i++)
    {
        if ((char)payload[i] == ',')
        {
            commas++;
        }
    }

    String payloadS[commas + 1];
    int pos = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        if ((char)payload[i] != ',')
        {
            payloadS[pos] += (char)payload[i];
        }
        else
        {
            pos++;
        }
    }

    for (uint8_t i = 0; i < commas; i++)
    {
        printDebug(payloadS[i]);
    }

    subscribeOrder(payloadS);
}

void subscribeOrder(String payloadS[])
{
    if (payloadS[0] == "1")
    {
        publishedCommunication = false;
    }
    if (payloadS[0] == "2")
    {
        publishedData = false;
    }
    if (payloadS[0] == "3")
    {
        publishedWIFIData = false;
    }
    if (payloadS[0] == "8")
    {
        readOrders(payloadS);
    }
    if (payloadS[0] == "9")
    {
        readConfiguration(payloadS);
    }
}

void publish()
{
    if (!publishedInit)
    {
        publishInit();
        publishedInit = true;
    }
    if (!publishedCommunication)
    {
        publishCommunication();
        publishedCommunication = true;
    }
    if (!publishedData)
    {
        publishData();
        publishedData = true;
    }
    if (!publishedWIFIData)
    {
        publishWIFIData();
        publishedWIFIData = true;
    }
}

void publishInit()
{
    char msg[] = "0";
    mqtt.publish(topicPub, msg);
    printDebug("publicado");
}

void publishCommunication()
{
    char msg[] = "1";
    mqtt.publish(topicPub, msg);
    printDebug("publicado");
}

void publishData()
{
    String payloadString = "2," + (String)getElectrovalvula() + "," + (String)getCaudal() + "," + (String)getLectura() + "," + (String)getBoyaBaja() + "," + (String)getBoyaMedia() + "," + (String)getBoyaAlta() + "," + (String)getPressure();
    printDebug(payloadString);
    char payloadCharArray[payloadString.length()];
    payloadString.toCharArray(payloadCharArray, payloadString.length() + 1);
    mqtt.publish(topicPub, payloadCharArray);
    publishedData = true;
}

void readOrders(String payload[])
{
    int electrovalvula = payload[1].toInt();
    setElectrovalvula(electrovalvula);

    printDebug("ordenes recibidas");
}

void readConfiguration(String payload[])
{
    setLectura(payload[1].toInt());
    printDebug("configuración recibida");
}

void publishWIFIData()
{
    if (mqtt.connected())
    {
        String wifiLocalIP = (String)WiFi.localIP();
        String payloadString = "8," + wifiLocalIP;
        printDebug(payloadString);
        char payload[payloadString.length()];
        payloadString.toCharArray(payload, payloadString.length() + 1);
        mqtt.publish(topicPub, payload);
    }
}