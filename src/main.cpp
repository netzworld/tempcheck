#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("BME280 Test");

  if (!bme.begin(0x76))
  {
    Serial.println("Could not find BME280 sensor. Check wiring!");
    while (1)
      ;
  }
}

void loop()
{
  Serial.print("Temp: ");
  Serial.print((bme.readTemperature() * 9/5) + 32);
  Serial.println(" °F");

  Serial.print("Pressure: ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Humidity: ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  delay(2000);
}