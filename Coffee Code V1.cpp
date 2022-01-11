#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID "Andre Wifi"
#define WLAN_PASS "nw3ts3cur32019"

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883                      
#define AIO_USERNAME "AndreV61"                   
#define AIO_KEY "85aa79cbaeca4578bdbe75e2d4679c21" 

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

int ledPin = 2;

//Nespresso Sleep Timer
unsigned long Sleep_timer = 560000;
unsigned long delayStart;
//bool delayRunning;

Adafruit_MQTT_Subscribe coffeemake = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/java");
Adafruit_MQTT_Publish coffeedone = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/java");

//void MQTT_connect();
//void onMQTTupdate();

void setup()
{

  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  delayStart = millis();
  //delayRunning = true;

  Serial.println(F("Adafruit MQTT demo"));

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&coffeemake);
}

void MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;

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
      Serial.print("delayStart: ");
      Serial.println(delayStart);
      Serial.print("milliseconds: ");
      Serial.println(millis());
      Serial.print("sleep_timer: ");
      Serial.println(Sleep_timer);
      Serial.print(F("Got: "));
      Serial.println((char *)coffeemake.lastread);

      if (strcmp((char *)coffeemake.lastread, "ON") == 0)
      {
        /*Serial.print("delayStart: ");
        Serial.println(delayStart);
        Serial.print("milliseconds: ");
        Serial.println(millis());
        Serial.print("sleep_timer: ");
        Serial.println(Sleep_timer);*/

        if (delayStart - millis() >= Sleep_timer)
        { 
          /*Serial.print("delayStart: ");
          Serial.println(delayStart);
          Serial.print("milliseconds: ");
          Serial.println(millis());
          Serial.print("sleep_timer: ");
          Serial.println(Sleep_timer);*/
          digitalWrite(ledPin, 0);
          delay(500);
          digitalWrite(ledPin, 1);
          delay(500);
          digitalWrite(ledPin, 0);
          delay(500);
          digitalWrite(ledPin, 1);
          coffeedone.publish("OFF");
          delayStart = Sleep_timer + millis();
        }
         
        
        else 
        {
          /*Serial.print("delayStart: ");
          Serial.println(delayStart);
          Serial.print("milliseconds: ");
          Serial.println(millis());
          Serial.print("sleep_timer: ");
          Serial.println(Sleep_timer);*/
          digitalWrite(ledPin, 0);
          delay(500);
          digitalWrite(ledPin, 1);
          coffeedone.publish("OFF");
          delayStart = Sleep_timer + millis();
        }
      }
    }
  }
}


void loop()
{
  MQTT_connect();

  onMQTTupdate();

  /*if (!mqtt.ping())
  {
    mqtt.disconnect();
  }*/
}