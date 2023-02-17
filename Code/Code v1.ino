    
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
int button = 2;
int Buzzerpin = 16;
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "gurgaon police"
#define WLAN_PASS       "9910162064"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Ayaan"
#define AIO_KEY         "aio_oReY77ANu2HBvhTDqKS6bEv7Dxks"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish buttonStatus = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Buzzer");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe Buzzer = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Buzzer");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(button, INPUT);
  pinMode(Buzzerpin, OUTPUT);
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe( & Buzzer);
}


void loop() {

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Buzzer) {
      Serial.print(F("Got: "));
      Serial.println((char *)Buzzer.lastread);
      if (strcmp((char *)Buzzer.lastread, "ON") == 0) {
        digitalWrite(Buzzerpin, HIGH);
      } else if (strcmp((char *)Buzzer.lastread, "OFF") == 0) {
        digitalWrite(Buzzerpin, LOW);
      }
    }

  }

int buttonVal = digitalRead(button);
// Now we can publish stuff!
Serial.print(buttonVal);
Serial.print("...");
if (! buttonStatus.publish(buttonVal)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
delay(3000);
}
