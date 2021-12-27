#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

// Optionally include custom images
//#include "images.h"

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, 5, 4);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h


/* 1. Define the WiFi credentials */
#define WIFI_SSID "************"
#define WIFI_PASSWORD "**********"
#define WIFI_HOSTNAME "iot-meeting"

#define FIREBASE_HOST "https://**************.firebaseio.com"
#define FIREBASE_AUTH "******************"

boolean bypass;
String pos;
String HostnameDNS;

unsigned long lastSampleTime = 0;
const long sampleDelay = 600000; // 10 minutes Loops (in milliseconds)
const long controlSampleDelay = 60000; // (in milliseconds) - For Actions keys

FirebaseData firebaseData;

void connectWifi() {
  HostnameDNS = String(WIFI_HOSTNAME);
  HostnameDNS.toUpperCase();
  WiFi.hostname(HostnameDNS);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.softAPdisconnect (true); // Force disable AP Mode

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".......");
  Serial.println("WiFi Connected....IP Address:");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  connectWifi();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  String macaddr  = WiFi.macAddress().c_str();
  String ipaddr   = WiFi.localIP().toString().c_str();
  int32_t rssi    = WiFi.RSSI();
  String bssid    = WiFi.BSSIDstr();

  Firebase.setString(firebaseData, String("/office") + String("/control/mac"), macaddr);
  Firebase.setString(firebaseData, String("/office") + String("/control/ip"), ipaddr);
  Firebase.setString(firebaseData, String("/office") + String("/control/signal"), String(rssi));
  Firebase.setString(firebaseData, String("/office") + String("/control/ssid"), WIFI_SSID);
  Firebase.setString(firebaseData, String("/office") + String("/control/bssid"), bssid);
  Firebase.setString(firebaseData, String("/office") + String("/control/action"), "0");

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();

}

void busy() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.drawStringMaxWidth(0, 0, 128,
  display.drawString(10, 0, "EM REUNIÃO");
  display.drawString(20, 20, "Entrar em");
  display.setFont(ArialMT_Plain_24);
  display.drawString(10, 40, "Silêncio!!!" );
}

void free() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.drawStringMaxWidth(0, 0, 128,
  display.drawString(10, 0, "");
  display.setFont(ArialMT_Plain_24);
  display.drawString(10, 20, "Entre =D" );
}

void loop() {
  unsigned long currentMillis = millis();
  if (lastSampleTime == 0) {
    bypass = true;
  } else {
    bypass = false;
  }


  /* Control Code */
  if (currentMillis - lastSampleTime >= controlSampleDelay) {
    lastSampleTime = currentMillis;

    if (Firebase.get(firebaseData, String("/office/") + String("control/action"))) {
      String b_fir = firebaseData.stringData();
      //int int_force_val = b_fir.toInt();

      Serial.print("Force Control Value: ");
      Serial.println(String(b_fir));
      if (b_fir == "\"f\"") {
        Serial.println("Forcing board update");
        Firebase.setString(firebaseData, String("/office/") + String("control/action"), "0");
        bypass = true;
      } else if (b_fir == "\"r\"") {
        ESP.restart();
      }
    }
  }
    /* Control Code */
  if (currentMillis - lastSampleTime >= sampleDelay || bypass == true) {
    lastSampleTime = currentMillis;
    /* Operation */
    if (Firebase.get(firebaseData, String("/office/") + String("meeting"))) {

      String b_fir = firebaseData.stringData();
      //int int_val = b_fir.toInt();

      Serial.print("Stored Value: ");
      Serial.println(String(b_fir));
      pos = b_fir;
    }
    bypass == false;
  }

  // clear the display
  display.clear();

  if (pos == "\"0\"") {
    free();
  } else if (pos == "\"1\"") {
    busy();
  }

  // write the buffer to the display
  display.display();
}
