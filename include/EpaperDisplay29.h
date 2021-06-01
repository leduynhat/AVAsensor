#include "Config.h"
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include <stdlib.h>

struct EpaperDisplay29Status
{
    SCREEN_ID current_screen;
    SCREEN_ID previous_screen;
    uint8_t current_pointer_position;
    uint8_t previous_pointer_position;
};

class EpaperDisplay29
{
private:
    EpaperDisplay29Status status;
public:
    void init();
    void progressScreen(bool _sd = false, bool _opcn3 = false, bool _sht85 = false, bool _als_toxic_no2_o3 = false, bool _als_toxic_co_so2 = false, bool _ds3231 = false, bool _neom8n = false);
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
    EpaperDisplay29Status getStatus();
    void setDisplayScreen(SCREEN_ID _screen);
    void setStatus();
    EpaperDisplay29(/* args */);
    ~EpaperDisplay29();
};

