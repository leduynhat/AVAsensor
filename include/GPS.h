/**
 * @file GPS.h
 * @author Le Duy Nhat(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Config.h"
#include "TinyGPS++.h"

class GPS
{
private:
    double latitude;
    double longitude;
    uint32_t unixTime;
    TinyGPSPlus gps;
public:
    bool getGPS();
    bool isConnected();
    double getLatitude();
    double getLongitude();
    uint32_t getUnixtime();
    GPS(/* args */);
    ~GPS();
};

