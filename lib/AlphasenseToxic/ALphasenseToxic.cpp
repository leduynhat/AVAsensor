#include "AlphasenseToxic.h"
AlphasenseToxic::AlphasenseToxic(/* args */)
{
}

AlphasenseToxic::~AlphasenseToxic()
{
}

bool AlphasenseToxic::init(){
    no2_o3.setGain(GAIN_TWOTHIRDS);
    co_so2.setGain(GAIN_TWOTHIRDS);

    no2_o3.begin(NO2_O3_I2C_ADDRESS);
    co_so2.begin(CO_SO2_I2C_ADDRESS);

    return true;
}

void AlphasenseToxic::readNO2RawData(){
    this->no2_mv.AE_mV = (no2_o3.readADC_SingleEnded((1))*MULTIPLIER) / 1000;
    this->no2_mv.WE_mV = (no2_o3.readADC_SingleEnded((0))*MULTIPLIER) / 1000;
}
void AlphasenseToxic::readO3RawData(){
    this->o3_mv.AE_mV = (no2_o3.readADC_SingleEnded((3))*MULTIPLIER) / 1000;
    this->o3_mv.WE_mV = (no2_o3.readADC_SingleEnded((2))*MULTIPLIER) / 1000;
}
void AlphasenseToxic::readCORawData(){
    this->co_mv.AE_mV = (co_so2.readADC_SingleEnded((1))*MULTIPLIER) / 1000;
    this->co_mv.WE_mV = (co_so2.readADC_SingleEnded((0))*MULTIPLIER) / 1000;
}
void AlphasenseToxic::readSO2RawData(){
    this->so2_mv.AE_mV = (co_so2.readADC_SingleEnded((3))*MULTIPLIER) / 1000;
    this->so2_mv.WE_mV = (co_so2.readADC_SingleEnded((2))*MULTIPLIER) / 1000;
}
void AlphasenseToxic::readData(){
    readNO2RawData();
    readO3RawData();
    readSO2RawData();
    readCORawData();
}
float AlphasenseToxic::getNO2AEVoltage(){
    return no2_mv.AE_mV;
}
float AlphasenseToxic::getNO2WEVoltage(){
    return no2_mv.WE_mV;
}
float AlphasenseToxic::getO3AEVoltage(){
    return o3_mv.AE_mV;
}
float AlphasenseToxic::getO3WEVoltage(){
    return o3_mv.WE_mV;
}
float AlphasenseToxic::getSO2AEVoltage(){
    return so2_mv.AE_mV;
}
float AlphasenseToxic::getSO2WEVoltage(){
    return so2_mv.WE_mV;
}
float AlphasenseToxic::getCOAEVoltage(){
    return co_mv.AE_mV;
}
float AlphasenseToxic::getCOWEVoltage(){
    return co_mv.WE_mV;
}