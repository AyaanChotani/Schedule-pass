
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
int button = 2;
int Buzzer = 16;
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
Adafruit_MQTT_Publish buzzerControl = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/buzzer");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe buzzerStauts = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buzzer");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
int flag = 0;
int buzzer = 2;
unsigned long previousMillis = 0;
const long interval = 10000;
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(buzzer, OUTPUT);
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
  mqtt.subscribe(&buzzerStauts);
}

uint32_t x = 0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &buzzerStauts) {
      Serial.print(F("Got: "));
      Serial.println((char *)buzzerStauts.lastread);
      if (strcmp((char *)buzzerStauts.lastread, "1") == 0) {
        flag = 1;
        tone(buzzer, 500);
        Serial.println("value is 1");
      } else {
        flag = 0;
        noTone(buzzer);
      }
    }
  }

  if (flag == 1) {
    // Now we can publish stuff!
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      noTone(buzzer);
      if (!buzzerControl.publish(0)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
    }

  }

}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
