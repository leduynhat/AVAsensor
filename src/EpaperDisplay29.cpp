#include "EpaperDisplay29.h"
//Create a new image cache
UBYTE BlackImage[(EPD_2IN9_WIDTH / 8 )* EPD_2IN9_HEIGHT];
const unsigned char gImage_wifi[124] = { /* 0X01,0X01,0X1F,0X00,0X1F,0X00, */
0XFF,0XC7,0XFF,0XFE,0XFF,0X83,0XFF,0XFE,0XFF,0X03,0XFF,0XFE,0XFE,0X07,0XFF,0XFE,
0XFC,0X0E,0X3F,0XFE,0XFC,0X1C,0X1F,0XFE,0XF8,0X38,0X1F,0XFE,0XF8,0X30,0X3F,0XFE,
0XF8,0X70,0X73,0XFE,0XF0,0X60,0XE1,0XFE,0XF0,0XE0,0XC1,0XFE,0XF0,0XC1,0XC1,0XFE,
0XF0,0XC1,0X83,0XFE,0XF0,0XC3,0X87,0X9E,0XF0,0XC3,0X87,0X0E,0XF0,0XC3,0X8E,0X0E,
0XF0,0XC3,0X87,0X0E,0XF0,0XC3,0X87,0X9E,0XF0,0XC1,0X83,0XFE,0XF0,0XC1,0XC1,0XFE,
0XF0,0XE0,0XC1,0XFE,0XF0,0X60,0XE1,0XFE,0XF8,0X70,0X73,0XFE,0XF8,0X30,0X3F,0XFE,
0XF8,0X38,0X1F,0XFE,0XFC,0X1C,0X1F,0XFE,0XFC,0X0E,0X3F,0XFE,0XFE,0X07,0XFF,0XFE,
0XFF,0X03,0XFF,0XFE,0XFF,0X83,0XFF,0XFE,0XFF,0XC7,0XFF,0XFE,};
const unsigned char gImage_configwifi[124] = { /* 0X01,0X01,0X1F,0X00,0X1F,0X00, */
0XFF,0X8F,0XFF,0XFE,0XFF,0X27,0XFF,0XFE,0XFE,0X67,0XFF,0XFE,0XFC,0XCF,0XFF,0XFE,
0XF9,0X9C,0X7F,0XFE,0XFB,0X39,0X3F,0XFE,0XF2,0X73,0X3F,0XFE,0XF6,0X66,0X7F,0XFE,
0XF4,0XE4,0XE7,0XFE,0XE4,0XCD,0XC3,0XFE,0XED,0XD9,0X9B,0XFE,0XED,0X93,0XB3,0XFE,
0XED,0X93,0X27,0XFE,0XED,0XB7,0X6F,0X3E,0XED,0XB7,0X4E,0X9E,0XED,0XB7,0X5C,0XDE,
0XED,0XB7,0X4E,0X9E,0XED,0XB7,0X6F,0X3E,0XED,0X93,0X27,0XFE,0XED,0X9B,0XB3,0XFE,
0XED,0XC9,0X93,0XFE,0XE4,0XCD,0XC3,0XFE,0XF4,0XE6,0XE7,0XFE,0XF6,0X62,0X7F,0XFE,
0XF2,0X73,0X3F,0XFE,0XFB,0X39,0X3F,0XFE,0XF9,0X9C,0X7F,0XFE,0XFC,0XCF,0XFF,0XFE,
0XFE,0X67,0XFF,0XFE,0XFF,0X27,0XFF,0XFE,0XFF,0X8F,0XFF,0XFE,};
const unsigned char gImage_nowifi[124] = { /* 0X01,0X01,0X1F,0X00,0X1F,0X00, */
0XFF,0X8F,0XFF,0XFE,0XFF,0X07,0XFF,0XFE,0XFE,0X07,0XFF,0XFE,0XFC,0X0F,0XFE,0X7E,
0XF8,0X1C,0X7C,0X7E,0XF8,0X38,0X38,0XFE,0XF0,0X70,0X31,0XFE,0XF0,0X60,0X63,0XFE,
0XF0,0XE0,0XC7,0XFE,0XE0,0XC1,0X83,0XFE,0XE1,0XC1,0X03,0XFE,0XE1,0X82,0X03,0XFE,
0XE1,0X82,0X07,0XFE,0XE1,0X84,0X0F,0X3E,0XE1,0X80,0X0E,0X1E,0XE1,0X80,0X1C,0X1E,
0XE1,0X81,0X0E,0X1E,0XE1,0X83,0X0F,0X3E,0XE1,0X83,0X07,0XFE,0XE1,0X03,0X83,0XFE,
0XE1,0X01,0X83,0XFE,0XE0,0X01,0XC3,0XFE,0XF0,0X20,0XE7,0XFE,0XF0,0X60,0X7F,0XFE,
0XF0,0X70,0X3F,0XFE,0XF8,0X38,0X3F,0XFE,0XF0,0X1C,0X7F,0XFE,0XE0,0X0F,0XFF,0XFE,
0XE6,0X07,0XFF,0XFE,0XEF,0X07,0XFF,0XFE,0XFF,0X8F,0XFF,0XFE,};
const unsigned char gImage_sd[104] = { /* 0X01,0X01,0X1A,0X00,0X1A,0X00, */
0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,
0XFF,0XC6,0X00,0X00,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,
0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,};
const unsigned char gImage_nosd[104] = { /* 0X01,0X01,0X1A,0X00,0X1A,0X00, */
0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,
0XFF,0XC6,0X00,0X00,0XFF,0X90,0XFF,0X00,0X00,0X3F,0XFE,0X00,0X7E,0XFF,0XFC,0X80,
0X00,0XFF,0XF9,0X80,0X3E,0XFF,0XF3,0X80,0X00,0XFF,0XE7,0X80,0X7E,0XFF,0XCF,0X80,
0X00,0XFF,0X9F,0X80,0X3E,0XFF,0X3F,0X80,0X00,0XFE,0X7F,0X80,0X7E,0XFC,0XFF,0X80,
0X00,0XF9,0XFF,0X80,0X3E,0XF3,0XFF,0X80,0X00,0XE7,0XFF,0X80,0X7E,0XCF,0XFF,0X80,
0X00,0X9F,0XFF,0X80,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,
0XFF,0XFF,0XFF,0XC0,0XFF,0XFF,0XFF,0XC0,};
const unsigned char gImage_nointernet[104] = { /* 0X01,0X01,0X1A,0X00,0X1A,0X00, */
0XFF,0XC0,0X7E,0X40,0XFE,0X12,0X1C,0X40,0XF8,0XF3,0XC0,0XC0,0XF3,0XF3,0XE1,0XC0,
0XE1,0XF3,0XE1,0XC0,0XCC,0XF3,0XC4,0XC0,0X9E,0X80,0X06,0X40,0X98,0X33,0X0F,0X40,
0X32,0X73,0X03,0X00,0X27,0X72,0X19,0X00,0X0F,0X30,0X1C,0X00,0X1F,0XB0,0X9E,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0X83,0X9E,0X00,0X0F,0X03,0X9C,0X00,
0X27,0X13,0XD9,0X00,0X22,0X33,0XC3,0X00,0XB8,0X13,0X0F,0X40,0X98,0XC0,0X6E,0X40,
0XC0,0XF3,0XE4,0XC0,0XC1,0XF3,0XF1,0XC0,0XC3,0XF3,0XF3,0XC0,0X80,0XF3,0XC7,0XC0,
0X1C,0X12,0X0F,0XC0,0X3F,0X80,0X7F,0XC0,};
const unsigned char gImage_internet[104] = { /* 0X01,0X01,0X1A,0X00,0X1A,0X00, */
0XFF,0XC0,0X7F,0XC0,0XFE,0X12,0X1F,0XC0,0XF8,0XF3,0XC7,0XC0,0XF3,0XF3,0XE3,0XC0,
0XE1,0XF3,0XF1,0XC0,0XCC,0XF3,0XE4,0XC0,0X9E,0X80,0X6E,0X40,0X98,0X33,0X0F,0X40,
0X32,0X73,0XC3,0X00,0X27,0X73,0XD9,0X00,0X0F,0X33,0X9C,0X00,0X1F,0XB3,0X9E,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0XB3,0X9E,0X00,0X0F,0X33,0X9C,0X00,
0X27,0X73,0XD9,0X00,0X22,0X73,0XC3,0X00,0XB8,0X13,0X0F,0X40,0X9E,0XC0,0X6E,0X40,
0XDC,0XF3,0XE4,0XC0,0XC9,0XF3,0XF1,0XC0,0XE3,0XF3,0XF3,0XC0,0XF0,0XF3,0XC7,0XC0,
0XFC,0X12,0X0F,0XC0,0XFF,0X80,0X7F,0XC0,};

EpaperDisplay29::EpaperDisplay29(/* args */)
{
}

EpaperDisplay29::~EpaperDisplay29()
{
}

void EpaperDisplay29::init(){
    // printf("->Init Epaper\r\n");
    DEV_Module_Init();

    // printf("e-Paper Init and Clear...\r\n");
    EPD_2IN9_Init(EPD_2IN9_FULL);
    // printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 270, WHITE);
    Paint_Clear(WHITE);

#if 1   //show image for array  
    // printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_DrawBitMap(gImage_ava);

    EPD_2IN9_Display(BlackImage);
    DEV_Delay_ms(1000);
#endif
}
void EpaperDisplay29::progressScreen(bool _sd, bool _opcn3, bool _sht85, bool _als_toxic_no2_o3, bool _als_toxic_co_so2, bool _ds3231, bool _neom8n){
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 0, DEVICE_NAME, &Font12, WHITE, BLACK);
    Paint_DrawString_EN(110, 0, FIRMWARE_VERSION, &Font12, WHITE, BLACK);
    if (_als_toxic_no2_o3)
    {
        Paint_DrawString_EN(10, 13, "Als NO2&O3 connected", &Font12, WHITE, BLACK);
    }
    else{
        Paint_DrawString_EN(10, 13, "Als NO2&O3 disconnected", &Font12, WHITE, BLACK);
    }
    if (_als_toxic_co_so2)
    {
        Paint_DrawString_EN(10, 26, "Als CO&SO2 connected", &Font12, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(10, 26, "Als CO&SO2 disconnected", &Font12, WHITE, BLACK);;
    }
    if (_sht85)
    {
        Paint_DrawString_EN(10, 39, "SHT85      connected", &Font12, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(10, 39, "SHT85      disconnected", &Font12, WHITE, BLACK);
    }
    if (_ds3231)
    {
        Paint_DrawString_EN(10, 52, "DS3231     connected", &Font12, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(10, 52, "DS3231     disconnected", &Font12, WHITE, BLACK);
    }
    if (_opcn3)
    {
        Paint_DrawString_EN(10, 65, "OPCN3      connected", &Font12, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(10, 65, "OPCN3      disconnected", &Font12, WHITE, BLACK);
    }
    if (_sd)
    {
        Paint_DrawString_EN(10, 78, "SD         connected", &Font12, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(10, 78, "SD         disconnected", &Font12, WHITE, BLACK);
    }
    if (_neom8n)
    {
        Paint_DrawString_EN(10, 91, "NEO M8N    connected", &Font12, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(10, 91, "NEO M8N    disconnected", &Font12, WHITE, BLACK);
    }
    Paint_DrawString_EN(10, 104, "Getting ready ...", &Font12, WHITE, BLACK);
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::mainScreen(){
    EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(gImage_2in9);
    EPD_2IN9_Display(BlackImage);
    // DEV_Delay_ms(2000);
}
void EpaperDisplay29::showScreen(){

}
void EpaperDisplay29::updateData(DataCore _data_core, char* _mac){
    Paint_SelectImage(BlackImage);
    char value[7];
    sprintf(value, "%.1f", _data_core.pm2_5);
    Paint_ClearWindows(60, 57, 60 + Font20.Width * 5, 57 + Font20.Height, WHITE);
    Paint_DrawString_EN(60,60, value, &Font20, WHITE, BLACK);
  
    sprintf(value, "%.1f", _data_core.temperature);
    Paint_ClearWindows(20, 105, 20 + Font16.Width * 4, 105 + Font16.Height, WHITE);
    Paint_DrawString_EN(20,107, value, &Font16, WHITE, BLACK);
  
    sprintf(value, "%.1f", _data_core.humidity);
    Paint_ClearWindows(91, 105, 91 + Font16.Width * 4, 105 + Font16.Height, WHITE);
    Paint_DrawString_EN(91 ,105 , value, &Font16, WHITE, BLACK);
  
    sprintf(value, "%.3f", _data_core.SO2_WmV);
    Paint_ClearWindows(195, 13, 195 + Font16.Width * 5, 13 + Font16.Height, WHITE);
    Paint_DrawString_EN(195 ,13 , value, &Font16, WHITE, BLACK);
  
    sprintf(value, "%.3f", _data_core.NO2_WmV);
    Paint_ClearWindows(195, 42, 195 + Font16.Width * 5, 42 + Font16.Height, WHITE);
    Paint_DrawString_EN(195 ,42 , value, &Font16, WHITE, BLACK);
  
    sprintf(value, "%.3f", _data_core.O3_WmV);
    Paint_ClearWindows(195, 76,195 + Font16.Width * 5, 76 + Font16.Height, WHITE);
    Paint_DrawString_EN(195 ,76 , value, &Font16, WHITE, BLACK);
  
    sprintf(value, "%.3f", _data_core.CO_WmV);
    Paint_ClearWindows(195, 110, 195 + Font16.Width * 5, 110 + Font16.Height, WHITE);
    Paint_DrawString_EN(195 ,110 , value, &Font16, WHITE, BLACK);
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::updateClock(uint8_t _hour, uint8_t _min){
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = _hour;
    sPaint_time.Min = _min;
    Paint_ClearWindows(255, 5, 255 + Font16.Width * 5, 5 + Font16.Height, WHITE);
    // char time[5];
    Paint_DrawTime_HourMin(260, 5, &sPaint_time, &Font12, WHITE, BLACK);

    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::showSDsymbol(){
    Paint_DrawImage(gImage_sd, 28, 8, 26,26);
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::showNonSDsymbol(){
Paint_DrawImage(gImage_nosd, 28, 8, 26,26);
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::showWiFiSymbol(){
    Paint_DrawImage(gImage_wifi, 62, 6, 31,31);
    Paint_DrawImage(gImage_internet, 97, 9, 26,26); 
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::showWiFiWaitSymbol(){
    Paint_DrawImage(gImage_wifi, 62, 6, 31,31);
    EPD_2IN9_Display(BlackImage);

}
void EpaperDisplay29::showNonWiFiSymbol(){
    Paint_DrawImage(gImage_nowifi, 62, 6, 31,31);
    Paint_DrawImage(gImage_nointernet, 97, 9, 26,26);    
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::showWiFiConfigSymbol(){
    Paint_DrawImage(gImage_configwifi, 62, 6, 31,31);
    Paint_DrawImage(gImage_nointernet, 97, 9, 26,26);    
    EPD_2IN9_Display(BlackImage);
}
void EpaperDisplay29::showWiFiButNoInternetSymbol(){
    Paint_DrawImage(gImage_wifi, 62, 6, 31,31);
    Paint_DrawImage(gImage_nointernet, 97, 9, 26,26);    
    EPD_2IN9_Display(BlackImage);

}
EpaperDisplay29Status EpaperDisplay29::getStatus(){
    return this->status;
}
void EpaperDisplay29::setDisplayScreen(SCREEN_ID _screen){

}
void EpaperDisplay29::setStatus(){

}