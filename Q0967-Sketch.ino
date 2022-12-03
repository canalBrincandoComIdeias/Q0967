/*
 * This is a basic example on how to use Espalexa and its device declaration methods.
 */
#include "secret.h"

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

#define pinLED 2
#define pinLuz2 4
#define pinLuz3 5

//PWM do ESP32
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

//Controle Dimmer
uint8_t brilhoLED;
uint8_t brilhoLEDTarget;
#define timeChange 25
unsigned long lastChange;

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);
void secondLightChanged(uint8_t brightness);
void thirdLightChanged(uint8_t brightness);

// Change this!!
//const char* ssid = "...";
//const char* password = "wifipassword";

boolean wifiConnected = false;

Espalexa espalexa;

EspalexaDevice* device3;  //this is optional

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinLuz2, OUTPUT);
  pinMode(pinLuz3, OUTPUT);

  //PWM do ESP32
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(pinLED, ledChannel);

  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected) {

    // Define your devices here.
    espalexa.addDevice("LED Bancada", firstLightChanged);       //simplest definition, default state off
    espalexa.addDevice("Luz Quarto", secondLightChanged, 255);  //third parameter is beginning state (here fully on)

    device3 = new EspalexaDevice("Luz Sala", thirdLightChanged);  //you can also create the Device objects yourself like here
    espalexa.addDevice(device3);                                  //and then add them
    device3->setValue(128);                                       //this allows you to e.g. update their state value at any time!

    espalexa.begin();

  } else {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}

void loop() {
  espalexa.loop();

  if (brilhoLEDTarget != brilhoLED) {

    if ((millis() - lastChange) >= timeChange) {

      if (brilhoLED < brilhoLEDTarget) brilhoLED++;
      if (brilhoLED > brilhoLEDTarget) brilhoLED--;

      ledcWrite(ledChannel, brilhoLED);
      lastChange = millis();
    }
  }

  delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
  Serial.print("LED Bancada changed to ");

  //do what you need to do here
  brilhoLEDTarget = brightness;

  //EXAMPLE
  if (brightness) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);
  } else {
    Serial.println("OFF");
  }
}

void secondLightChanged(uint8_t brightness) {
  Serial.print("Luz Quarto changed to ");

  //do what you need to do here
  digitalWrite(pinLuz2, brightness);

  //EXAMPLE
  if (brightness) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);
  } else {
    Serial.println("OFF");
  }
}

void thirdLightChanged(uint8_t brightness) {
  Serial.print("Luz Sala changed to ");

  //do what you need to do here
  digitalWrite(pinLuz3, brightness);

  //EXAMPLE
  if (brightness) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);
  } else {
    Serial.println("OFF");
  }
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection failed.");
  }
  return state;
}
