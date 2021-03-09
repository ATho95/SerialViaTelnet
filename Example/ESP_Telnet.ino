#include "Arduino.h"

const char* wifi_ssid = "your_ssid";
const char* wifi_pass = "your_password";

#include <SerialViaTelnet.h>
// use existing Serial.print's going out via telnet
#define Serial SerialTelnet

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

void setup() {
  // the following Serial.xxx calls are #defined to SerialTelnet.xxx
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setup start");

  Serial.println("WiFi start");

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
    Serial.HWSerial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.printf("Setup end\n");

  // print IP address to UART
  Serial.HWSerial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

void loop() {
  static uint32_t nextPrint = 10000;
#ifdef TELNETVIASERIAL_H_
  // must be called periodically
  SerialTelnet.loop();
#endif

  // alive message
  if (millis() >= nextPrint) {
    nextPrint = millis() + 10000;
    Serial.printf("running since %ld s\n", millis() / 1000);
  }

  // forward from telnet to UART
  if (SerialTelnet.available()) {
    SerialTelnet.HWSerial.print((char)SerialTelnet.read());
  }

  // forward from UART to telnet
  if (SerialTelnet.HWSerial.available()) {
    SerialTelnet.print((char)SerialTelnet.HWSerial.read());
  }
}
