#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#ifndef STASSID
#define STASSID "Visser WiFi "
#define STAPSK  "Pies@ng2022"
#endif

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "AndreV61"
#define AIO_KEY "85aa79cbaeca4578bdbe75e2d4679c21"
#define MQTT_CONN_KEEPALIVE 300

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

int ledPin = D1;

//Nespresso Sleep Timer
unsigned long Sleep_timer = 560000;
unsigned long delayStart = 0;

Adafruit_MQTT_Subscribe coffeemake = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/java");
Adafruit_MQTT_Publish coffeedone = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/java");

void MQTT_connect();
void onMQTTupdate();

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }



  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    Serial.println("Start updating " + type);
  });



  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });



  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });



  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });



  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);



  mqtt.subscribe(&coffeemake);
}

void loop() {
 
  ArduinoOTA.handle();

  MQTT_connect();

  onMQTTupdate();



  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }

}



void MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 5;

  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0)
    {
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}



void onMQTTupdate()
{

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &coffeemake)
    {

      if (strcmp((char *)coffeemake.lastread, "ON") == 0)
      {
        /*Serial.print("delayStart: ");
        Serial.println(delayStart);
        Serial.print("milliseconds: ");
        Serial.println(millis());
        Serial.print("sleep_timer: ");
        Serial.println(Sleep_timer);*/

        if (millis() >= delayStart)
        { 
          digitalWrite(ledPin, HIGH);
          delay(500);
          digitalWrite(ledPin, LOW);
          delay(500);
          digitalWrite(ledPin, HIGH);
          delay(500);
          digitalWrite(ledPin, LOW);
          coffeedone.publish("OFF");
          delayStart = millis() + Sleep_timer;
        }
         
        
        else 
        {
          digitalWrite(ledPin, HIGH);
          delay(500);
          digitalWrite(ledPin, LOW);
          coffeedone.publish("OFF");
          delayStart = millis() + Sleep_timer;
        }
      }
    }
  }
}
