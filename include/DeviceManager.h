/**
 * @file DeviceManager.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "OPCN3lib.h"
#include "AlphasenseToxic.h"
#include "DS3231_RTC.h"
#include "SHT85.h"
#include "UserInput.h"
#include "LedSignal.h"
#include "Store.h"
#include "GPS.h"

#define PM_MAX_VALUE 1500
#define PM_MIN_VALUE 0.001
#define HUMIDITY_MAX_VALUE 95
#define HUMIDITY_MIM_VALUE 0.001
#define TEMPERATURE_MAX_VALUE 50
#define TEMPERATURE_MIN_VALUE -10
#define NO2_MAX_VALUE
#define O3_MAX_VALUE
#define SO2_MAX_VALUE
#define CO_MAX_VALUE
// #define NO2_MAX_VALUE
// #define NO2_MAX_VALUE
// #define NO2_MAX_VALUE
// #define NO2_MAX_VALUE


enum{DEVICE_DISCONNECTED, DEVICE_CONNECTED};
enum{DEVICE_STATUS_WORKING, DEVICE_STATUS_SLEEPING};
enum{DEVICE_STATE_SLEEP, 
    DEVICE_STATE_WAKE, 
    DEVICE_STATE_GET_DATA, 
    DEVICE_STATE_LOG_DATA, 
    DEVICE_STATE_WAIT,
    DEVICE_STATE_INIT
    };

// enum{SD_CARD, als_toxic, DS3231, opcn3};
struct DeviceManagerState
{
    uint8_t currrent_state;
    uint8_t previous_state;
    uint8_t entry;
    uint8_t status;
};
struct ConnectedDevicesStatus
{
    uint8_t opcn3;
    uint8_t als_toxic_no2_o3;
    uint8_t als_toxic_co_so2;
    uint8_t ds3231;
    // uint8_t lcd1602;
    uint8_t ili9341;
    uint8_t sds011;
    uint8_t sd_card;
    uint8_t sht85;
    uint8_t gps;
};
class DeviceManager
{
private:
    DeviceManager()
    {
        //init default data;
        this->data_core.UNIXTime = 0;  
        this->data_core.pm2_5 = 0;
        this->data_core.pm10_0 = 0;
        this->data_core.pm1_0 = 0;
        this->data_core.temperature = 0;
        this->data_core.humidity = 0;
        this->data_core.pressure = 0;
        this->data_core.NO2_WmV = 0;
        this->data_core.NO2_AmV = 0;
        this->data_core.O3_WmV = 0;
        this->data_core.O3_AmV = 0;
        this->data_core.CO_WmV = 0;
        this->data_core.CO_AmV = 0;
        this->data_core.SO2_WmV = 0;
        this->data_core.SO2_AmV = 0;
        this->data_core.longitude = 0;
        this->data_core.latitude = 0;
        // this->data_core.CO2 = 0;
        //init defalt connected devices
        this->connected_devices_status = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    }
    static DeviceManager* instance;
    
    TimeNow time_now;
    DataCore data_core;
    uint8_t MACAddress[6];
    char mac_address[13];
    ConnectedDevicesStatus connected_devices_status;
    DeviceManagerState state;
public:
    OPCN3 opcn3;
    AlphasenseToxic als_toxic;
    DS3231_RTC ds3231;
    Store store;
    SHT85 sht85;
    GPS gps;

    // DeviceManager(/* args */);
    ~DeviceManager();
    void initHardware();
    void checkConnectedDevices();
    void initConnectedDevices();
    bool checkI2CDevices(uint8_t _i2c_address);
    void checkSPI();
    bool isSDcardConnected();
    void updateDataCore();
    void updateStatus();
    void updateTimeFromRTC();
    void setTimeToRTC(uint32_t _unix_time);
    void setAlarm();
    void sleep();
    void wake();
    bool checkAlarm1();
    bool checkAlarm2();
    void clearAlarm1();
    void clearAlarm2();
    bool dataError();
    void logDataCoreToSDCard();
    void setPeriodTimeSample();
    void calibrate();
    void printData();
    void initWDT();
    void deinitWDT();
    void resetWDT();
    void switchState(uint8_t _next_state);
    void waitPeriod(uint16_t _time_stamp);
    TimeNow getTime();
    DataCore getDataCore();
    DeviceManagerState getState();
    ConnectedDevicesStatus getConnectionStatus();
    char* getMAC();
    static DeviceManager* getInstance()
    {
        if(nullptr == instance)
        {
            instance = new DeviceManager();
        }
        return instance;
    }
};

// void updateDataCore();
// void sleep();
// void wakeUp();
// void waitPeriod();
// void logData();
// void checkDeviceState();