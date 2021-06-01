#include "OPCN3lib.h"

#include <Arduino.h>
#define _SERIAL_DEBUG_OPCN3_ 

SPIClass opcnSPI(HSPI);

OPCN3::OPCN3(uint8_t pinSelect, uint32_t speedSelect) {
    _ssPin = pinSelect;
    _speedMaximum = speedSelect;
}
OPCN3::OPCN3(){
    _ssPin = SPI2_CS_OPCN3;
    _speedMaximum = SPI2_CLOCK_SPEED;
}

// Alpha Sensor Functions
void OPCN3::begin() {
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.println("Initiating SPI ");
    #endif
    // set ss pin to output
    pinMode(_ssPin, OUTPUT);
    // SPI.begin();
    opcnSPI.begin(SPI2_CLK_PIN, SPI2_MISO_PIN, SPI2_MOSI_PIN, _ssPin);
    // // set speed, order and mode
    opcnSPI.beginTransaction(SPISettings(_speedMaximum, MSBFIRST, SPI_MODE1));
    delay(1000);
}

bool OPCN3::initialize() {
    // delay(1000);
    #ifdef _SERIAL_DEBUG_OPCN3_
        // Serial.println("Initilize");
    #endif
    // delay(1000);
    // begin();
    // delay(1000);
    readSerialNumber();
    Serial.print("->OPCN3 Serial Number:");
    Serial.println(getSerialNumber());
    delay(1000);
    Serial.println("->DACandPowerStatus");
    readDACandPowerStatus();
    delay(1000);
    Serial.println("->ConfigurationVariables");
    readConfigurationVariables();
    delay(4000);
    Serial.println("");
    delay(5000);
    Serial.println("->Turn Fan on");
    struct FanDigitalPotShutdownState fanState=setFanDigitalPotShutdownState(true);
    delay(1000);
    Serial.println("->Turn Laser on");
    struct LaserDigitalPotShutdownState laserState=setLaserDigitalPotShutdownState(true);
    delay(1000);
    Serial.println("->Turn Laser Switch on");
    struct LaserPowerSwitchState laserPowerState=setLaserPowerSwitchState(true);
    delay(1000);
    Serial.println("->High Gain");
    struct HighLowGainState gainState=setHighLowGainState(true);
    delay(1000);
    bool reset=resetHistogram();
    // delay(2000);
    return reset;
}

struct DACandPowerStatus OPCN3::readDACandPowerStatus() {
    DACandPowerStatus dACandPowerStatus=sendCommand<DACandPowerStatus>(0X13, 0X13, 6);
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.println(dACandPowerStatus.toString());
    #endif
    return dACandPowerStatus;
}

struct FanDigitalPotShutdownState OPCN3::setFanDigitalPotShutdownState(bool status) {
    byte commandByte=status ? 0X03: 0X02;
    FanDigitalPotShutdownState fanDigitalPotShutdownState=sendCommand<FanDigitalPotShutdownState>(0X03, commandByte, 1);
    fanDigitalPotShutdownState.fanOn=status;
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.print("  Validity: ");
        Serial.println(fanDigitalPotShutdownState.valid);
        Serial.print("  Status: ");
        Serial.println(fanDigitalPotShutdownState.fanOn);
        // Serial.print(" ");
    #endif
    return fanDigitalPotShutdownState;
}

struct LaserDigitalPotShutdownState OPCN3::setLaserDigitalPotShutdownState(bool status) {
    byte commandByte=status ? 0X05: 0X04;
    LaserDigitalPotShutdownState laserDigitalPotShutdownState=sendCommand<LaserDigitalPotShutdownState>(0X03, commandByte, 1);
    laserDigitalPotShutdownState.laserOn=status;
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.print("  Validity: ");
        Serial.println(laserDigitalPotShutdownState.valid);
        Serial.print("  Status: ");
        Serial.println(laserDigitalPotShutdownState.laserOn);
        // Serial.print(" ");
    #endif
    return laserDigitalPotShutdownState;
}

struct LaserPowerSwitchState OPCN3::setLaserPowerSwitchState(bool status) {
    byte commandByte=status ? 0X07: 0X06;
    LaserPowerSwitchState laserPowerSwitchState=sendCommand<LaserPowerSwitchState>(0X03, commandByte, 1);
    laserPowerSwitchState.laserOn=status;
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.print("  Validity: ");
        Serial.println(laserPowerSwitchState.valid);
        Serial.print("  Status: ");
        Serial.println(laserPowerSwitchState.laserOn);
        // Serial.print(" ");
    #endif  
    return laserPowerSwitchState;
}

struct HighLowGainState OPCN3::setHighLowGainState(bool status) {
    byte commandByte=status ? 0X10: 0X11;
    HighLowGainState highLowGainState=sendCommand<HighLowGainState>(0X03, commandByte, 1);
    highLowGainState.highLow=status;
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.print("  Validity: ");
        Serial.println(highLowGainState.valid);
        Serial.print("  Status: ");
        Serial.println(highLowGainState.highLow);
        // Serial.print(" ");
    #endif
    return highLowGainState;
}

struct HistogramData OPCN3::readHistogramData() {
    HistogramData histogramData=sendCommand<HistogramData>(0X30, 0X30, 86);
    #ifdef _SERIAL_DEBUG_OPCN3_
        // Serial.println(histogramData.toString());
    #endif 
    this->pm1 = histogramData.pm1;
    this->pm2p5 = histogramData.pm2_5;
    this->pm10 = histogramData.pm10;
    this->temp = histogramData.getTempC();
    this->humi = histogramData.getHumidity();

    return histogramData;
}

bool OPCN3::resetHistogram() {
    HistogramData data=readHistogramData();
    return data.valid;
}

struct SerialNumber OPCN3::readSerialNumber() {
    SerialNumber serialNumber=sendCommand<SerialNumber>(0X10, 0X10, 60);
    #ifdef _SERIAL_DEBUG_OPCN3_
        // Serial.print("Validity: ");
        // Serial.println(serialNumber.valid);
    #endif
    String info="";

    for (int i=0; i < 60; i++) {
        info+=String(serialNumber.serial[i]);
    }

    serial=info;
    return serialNumber;
}

String OPCN3::getSerialNumber() {
    return serial;
}

struct ConfigurationVariables OPCN3::readConfigurationVariables() {
    ConfigurationVariables configurationVariables=sendCommand<ConfigurationVariables>(0X3C, 0X3C, 163);
    #ifdef _SERIAL_DEBUG_OPCN3_
        Serial.println(configurationVariables.toString());
    #endif
    return configurationVariables;
}

void OPCN3::beginTransfer() {
    digitalWrite(_ssPin, LOW);
    delay(1);
}

void OPCN3::endTransfer() {
    delay(1);
    digitalWrite(_ssPin, HIGH);
}

bool OPCN3::validate(byte arrayOne[], byte arrayTwo[], int size) {
    bool valid=true;

    for (int i=0; i < size; i++) {
        if (arrayOne[i] !=arrayTwo[i]) {
            valid=false;
        }
    }

    return valid;
}

////////////////////////
void OPCN3::sleep() {
    // setFanDigitalPotShutdownState(false);
    // delay(1000);
    // setLaserPowerSwitchState(false);
}

void OPCN3::wake(){
    // setFanDigitalPotShutdownState(true);
    // delay(1000);
    // setLaserPowerSwitchState(true);
}

float OPCN3::getPM1(){
    return pm1;
}

float OPCN3::getPM2p5(){
    return pm2p5;
}

float OPCN3::getPM10(){
    return pm10;
}

float OPCN3::getTemperature(){
    return temp;
}

float OPCN3::getHumidity(){
    return humi;
}

bool OPCN3::readData(){
    return readHistogramData().valid;
}

bool OPCN3::isConnected(){
    return readSerialNumber().valid;
}