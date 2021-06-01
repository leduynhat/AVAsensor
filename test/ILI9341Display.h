/**
 * @file DeveceManager.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#pragma once
#include "TFT_eSPI.h"
#include "Config.h"

struct ILI9341Status
{
    SCREEN_ID current_screen;
    SCREEN_ID previous_screen;
    uint8_t current_pointer_position;
    uint8_t previous_pointer_position;
};
class ILI9341Display
{
private: 
    // TFT_eSPI tft = TFT_eSPI();
    ILI9341Status status;
    // DeviceManager *devices = DeviceManager::getInstance();
    // DataCore data = DeviceManager::getDataCore();
public:
    void init();
    void progressScreen(bool _sd = false, bool _opcn3 = false, bool _sht85 = false, bool _als_toxic_no2_o3 = false, bool _als_toxic_co_so2 = false, bool _ds3231 = false);
    void guiHandler();
    void mainScreen();
    void showScreen();
    void updateData(DataCore _data_core, char* _mac);
    void updateClock(uint8_t _hour, uint8_t _min);
    void showSDsymbol();
    void showNonSDsymbol();
    void showWiFiSymbol();
    void showWiFiWaitSymbol();
    void showNonWiFiSymbol();
    void showWiFiConfigSymbol();
    void showWiFiButNoInternetSymbol();
    ILI9341Status getStatus();
    void setDisplayScreen(SCREEN_ID _screen);
    void setStatus();
    ILI9341Display();
    ~ILI9341Display();
};

