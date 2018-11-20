#include "Altimeter.hpp"
#include "../../config.h"
#include "../DataLogger.hpp"
#include <Wire.h>

void Altimeter::start()
{
  //scanI2cBus();
  if (barometer.begin())
  { //Omit the parameter for adafruit
#ifdef STATUS_PIN_LEVEL
    analogWrite(STATUS_PIN, STATUS_PIN_LEVEL);
#else
    digitalWrite(STATUS_PIN, HIGH);
#endif
    digitalWrite(MESSAGE_PIN, LOW);
    DataLogger::log("Barometer Started");
    barometerReady = true;
    reset();
  }
  else
  {
    //If the unit starts with the status pin off and the message pin on,
    //the barometer failed to initialize
    DataLogger::log("Barometer Init Fail");
  }
}

void Altimeter::reset()
{
  filter.reset(1, 1, 0.001);
  velocityFilter.reset(1, 1, 0.001);
  for (int i = 0; i < 4; i++)
  {
    filter.step(0);
    velocityFilter.step(0);
  }

  baselinePressure = getPressure();
  lastRefreshTime = 0;
  DataLogger::log(String("Barometer reset: ") + String(baselinePressure));
}

double Altimeter::referenceAltitude()
{
  return refAltitude;
}

double Altimeter::getAltitude()
{
  //   double pressure = getPressure();
  //   if(pressure == 0) {
  //     return 0;
  //   }
  //   double relativeAlt = barometer.altitude(pressure, baselinePressure);
  //   return relativeAlt;

  return filter.lastEstimate();
}

double Altimeter::verticalVelocity()
{
  velocityFilter.lastEstimate();
}

void Altimeter::update()
{
  double pressure = getPressure();
  if (pressure == 0)
  {
    return;
  }
  double relativeAlt = barometer.altitude(pressure, baselinePressure);

  long time = millis();
  if (lastRefreshTime)
  {
    long delta = time - lastRefreshTime;
    double velocity = (relativeAlt - filter.lastEstimate()) / delta * 1000;
    velocityFilter.step(velocity);
  }
  lastRefreshTime = time;

  filter.step(relativeAlt);
  //DataLogger::log(String("Pressure:") + String(pressure));
}

double Altimeter::getPressure()
{
  char status;
  double T, P, p0, a;
  status = barometer.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = barometer.getTemperature(T);
    if (status != 0)
    {
      status = barometer.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = barometer.getPressure(P, T);
        if (status != 0)
        {
          return (P);
        }
      }
    }
  }
  return 0;
}

bool Altimeter::isReady()
{
  return barometerReady;
}

void Altimeter::scanI2cBus()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
