#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <WiFi101.h>
#include "secrets.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

WiFiClient client;

const char *server = "10.0.0.138"; // Pi's IP on home WiFi (run `hostname -I` on Pi)
const int port = 5000;

void connectWiFi()
{
  WiFi.setPins(8, 7, 4, 2); // Feather M0: CS, IRQ, RST, EN

  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi module not found!");
    while (true)
      ;
  }

  Serial.print("Firmware version: ");
  Serial.println(WiFi.firmwareVersion());

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    attempts++;

    if (attempts >= 20)
    { // 10 seconds
      Serial.println("\nConnection timed out. Halting.");
      while (true)
        ; // halt so you can read serial output
    }
  }

  Serial.println("\nConnected!");
  Serial.print("IP: ");
  IPAddress ip = WiFi.localIP();
  Serial.print(ip[0]); Serial.print(".");
  Serial.print(ip[1]); Serial.print(".");
  Serial.print(ip[2]); Serial.print(".");
  Serial.println(ip[3]);
}

void sendData(float temperature, float pressure, float humidity)
{
  if (client.connect(server, port))
  {
    String payload = "{";
    payload += "\"temperature\":" + String(temperature, 2) + ",";
    payload += "\"humidity\":" + String(humidity, 2) + ",";
    payload += "\"pressure\":" + String(pressure, 2);
    payload += "}";

    String request = String("POST /api/data HTTP/1.1\r\n") +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + payload.length() + "\r\n\r\n" +
                     payload;

    // client.print(request);
    // Serial.println("Sent data:");
    // Serial.println(payload);
    delay(1000);
    client.stop();
  }
  else
  {
    Serial.println("Connection to server failed");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(8000);
  connectWiFi();

  if (!bme.begin(0x76))
  {
    Serial.println("Could not find BME280 sensor!");
    while (1)
      ;
  }
}

void loop()
{
  // Serial.println(WiFi.localIP());
  // Serial.println(WiFi.gatewayIP());
  float temp = (bme.readTemperature() * 9.0/5.0) + 32.0; 
  float hum = bme.readHumidity();
  float pres = bme.readPressure() / 100.0F;

    sendData(temp, pres, hum);
  delay(4000);
}