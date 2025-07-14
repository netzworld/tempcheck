#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <WiFi101.h>
#include "secrets.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

WiFiClient client;

const char *server = "192.168.4.1"; // Flask server IP
const int port = 5000;

void connectWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
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

    String request = String("POST /log HTTP/1.1\r\n") +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + payload.length() + "\r\n\r\n" +
                     payload;

    client.print(request);
    Serial.println("Sent data:");
    Serial.println(payload);
    delay(100);
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
  delay(2000);
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
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  float temp = (bme.readTemperature() * 9.0/5.0) + 32.0; 
  float hum = bme.readHumidity();
  float pres = bme.readPressure() / 100.0F;

  sendData(temp, hum, pres);
  delay(10000);
}