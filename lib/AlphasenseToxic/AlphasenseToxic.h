#ifndef _ALPHASENSE_TOXIC_H_
#define _ALPHASENSE_TOXIC_H_

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_ADS1X15.h"
 

#define MULTIPLIER (0.1875F)
#define NO2_O3_I2C_ADDRESS 0x48
#define CO_SO2_I2C_ADDRESS 0x49

struct electrodeVoltage
{
    float AE_mV;
    float WE_mV;
};

class AlphasenseToxic
{
private:
    Adafruit_ADS1115 no2_o3;
    Adafruit_ADS1115 co_so2;

    electrodeVoltage no2_mv;
    electrodeVoltage o3_mv;
    electrodeVoltage so2_mv;
    electrodeVoltage co_mv;
public:
    bool init();
    void readNO2RawData();
    void readO3RawData();
    void readCORawData();
    void readSO2RawData();
    
    void readData();
    float getNO2WEVoltage();
    float getNO2AEVoltage();
    float getO3WEVoltage();
    float getO3AEVoltage();
    float getSO2WEVoltage();
    float getSO2AEVoltage();
    float getCOWEVoltage();
    float getCOAEVoltage();

    // float get

    electrodeVoltage getNO2Voltage();
    AlphasenseToxic(/* args */);
    ~AlphasenseToxic();
};



#endif