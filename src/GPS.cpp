#include "GPS.h"
#include <stdio.h>
#include <time.h>
const char *gpsStream = "$GPR";
GPS::GPS(/* args */)
{
    longitude = 0;
    latitude = 0;
    unixTime = 0;
}

GPS::~GPS()
{
}

bool GPS::getGPS(){
    // uint32_t start = millis();
    Serial1.flush();
    uint8_t n_count = 0;
    char chr_rx;
    // while(millis() < start + 3000){
    Serial.println("\nGNSS: ");
    // Serial1.setTimeout(5000);
    // Serial1.setRxBufferSize(1000);
    String buf1;
    // String buf2;
    // String buf3;
    buf1 = Serial1.readStringUntil('\n');
    buf1 = Serial1.readStringUntil('L');
    buf1 = Serial1.readStringUntil('N');
    buf1 = Serial1.readStringUntil('\n');
    // while (*gpsStream)
    // {
    //     gps.encode(*gpsStream++);
    // }
    
    // buf2 = Serial1.readStringUntil('\n');
    // Serial.print(buf1);s
    // Serial.print("hello\n");
    // Serial.println(buf2);
    // char buff1[1024];
    // char buff2[1024];
    // Serial1.readBytesUntil('L', buff1, 1024);
    // Serial.print(buff);
    // Serial1.readBytesUntil('\n', buff2, 1024);
    // Serial.print(buff);
    // Serial.print(buff1);
    // Serial.println("\n cac");
    // Serial.print(buff2);
    // while(!Serial1.findUntil("GNGLL", "L"));
    // Serial1.find("GNGLL"); 
    // Serial1.flush();
    // Serial1.setRxBufferSize();
    // Serial.println("\n->GPS get: ");
    // while(Serial1.available()&&(millis() < start + 3000));
    // start = millis();
    // while(!Serial1.available()&&(millis() < start + 3000)); 
    delay(750);
    while ( n_count < 13 && Serial1.available()){
        if(Serial1.available() > 0){
            chr_rx = (char)Serial1.read();
            if(chr_rx == '\n'){
                n_count ++;
            } 
            // Serial.print(chr_rx);
            if (gps.encode(chr_rx)){
                Serial.print(F("Location: ")); 
                if (gps.location.isValid())
                {
                    Serial.print(gps.location.lat(), 6);
                    Serial.print(F(","));
                    Serial.print(gps.location.lng(), 6);
                    longitude = gps.location.lng();
                    latitude = gps.location.lat();
                }
                else
                {
                    Serial.print(F("INVALID"));
                    return false;
                }

                Serial.print(F("  Date/Time: "));
                if (gps.date.isValid() && gps.time.isValid())
                {
                    Serial.print(gps.date.month());
                    Serial.print(F("/"));
                    Serial.print(gps.date.day());
                    Serial.print(F("/"));
                    Serial.print(gps.date.year());
                    // time_now.date = gps.date.day();
                    // time_now.date = gps.date.day();
                    // time_now.date = gps.date.day();
                    // time_now.date = gps.date.day();
                    // time_now.date = gps.date.day();
                    // time_now.date = gps.date.day();
                    // time_now.date = gps.date.age();
                    struct tm timeinfo;
                    timeinfo.tm_sec = gps.time.second();
                    timeinfo.tm_min = gps.time.minute();
                    timeinfo.tm_hour = gps.time.hour();
                    timeinfo.tm_mday = gps.date.day();
                    timeinfo.tm_mon = gps.date.month() - 1;
                    timeinfo.tm_year = gps.date.year() - 1900;
                    timeinfo.tm_isdst = 7;
                    time_t unixtime;
                    unixtime = mktime(&timeinfo);
                    unixTime = (uint32_t)unixtime;
                    Serial.printf(" Unixtime: %d", unixtime);

                    // time_t     now;
                    // struct tm  ts;
                    // char       buf[80];

                    // Get current time
                    // time(&now);

                    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
                    // ts = *localtime(&now);
                    // strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &timeinfo);
                }
                else
                {
                    Serial.print(F("INVALID"));
                    return false;
                }

                Serial.print(F(" "));
                if (gps.time.isValid())
                {
                    if (gps.time.hour() < 10) Serial.print(F("0"));
                    Serial.print(gps.time.hour());
                    Serial.print(F(":"));
                    if (gps.time.minute() < 10) Serial.print(F("0"));
                    Serial.print(gps.time.minute());
                    Serial.print(F(":"));
                    if (gps.time.second() < 10) Serial.print(F("0"));
                    Serial.print(gps.time.second());
                    Serial.print(F("."));
                    if (gps.time.centisecond() < 10) Serial.print(F("0"));
                    Serial.println(gps.time.centisecond());
                }
                else
                {
                    Serial.print(F("INVALID"));
                }
                return true;
            }
        }
    }
    return false;
}
bool GPS::isConnected(){
    if(Serial1.available() > 100){
        return true;
    }
    return false;
}
double GPS::getLatitude(){
    return latitude;
}

double GPS::getLongitude(){
    return longitude;
}
uint32_t GPS::getUnixtime(){
    return unixTime;
}