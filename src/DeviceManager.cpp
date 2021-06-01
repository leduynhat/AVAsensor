#include "DeviceManager.h"
#include "esp_task_wdt.h"
#include "WiFi.h"
#include "Queue.h"

#define _SERIAL_DEBUG_ 
#define WDT_TIMEOUT 62 
static unsigned long last_switch_state=0;
DeviceManager * DeviceManager::instance=nullptr;

// this is for checking error data when there is duplicated data or zero data
static uint8_t err_data_count=0;
static DataCore pre_data_core;

// Returns true if x is in range [low..high], else false
bool inRange(float low, float high, float x)
{
    return ((x-high)*(x-low) <= 0);
}

DeviceManager::~DeviceManager() {

}

void DeviceManager::switchState(uint8_t _next_state) {
    this->state.previous_state=this->state.currrent_state;
    this->state.currrent_state=_next_state;
}

void DeviceManager::initHardware() {
    Serial.begin(SERIAL_DEBUG_BAUDRATE);
    Serial1.begin(SERIAL_SIM_BAUDRATE, SERIAL_8N1, SIM_RX_PIN, SIM_TX_PIN);
    Serial1.setRxBufferSize(2048);
    Serial1.setTimeout(2000);
    // pinMode(SPI_MISO_PIN, INPUT_PULLUP);
    SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
    SPI.begin(SPI_CLK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_SD_CARD);
    // pinMode(SPI_CS_SD_CARD, OUTPUT);
    
    opcnSPI.begin(SPI2_CLK_PIN, SPI2_MISO_PIN, SPI2_MOSI_PIN, SPI2_CS_OPCN3);
    opcnSPI.beginTransaction(SPISettings(SPI2_CLOCK_SPEED, MSBFIRST, SPI_MODE1));
    pinMode(SPI2_CS_OPCN3, OUTPUT);

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_CLOCK_SPEED);
    Wire1.begin(I2C_2_SDA_PIN, I2C_2_SCL_PIN, I2C_CLOCK_SPEED);

    WiFi.macAddress(this->MACAddress);
    sprintf(this->mac_address, "%02X%02X%02X%02X%02X%02X", MACAddress[0], MACAddress[1], MACAddress[2],
        MACAddress[3], MACAddress[4], MACAddress[5]);
    // // this->checkConnectedDevices();
   
    delay(500); 
    #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
        Serial.print("->MAC: ");
        Serial.println(mac_address);
        Serial.println("->InitHardware successfully");
    #endif
}

void DeviceManager::checkConnectedDevices() {
    // if(this->checkI2CDevices(SHT85_ADDRESS)){
    Wire1.beginTransmission(SHT85_ADDRESS);
    if(Wire1.endTransmission() == 0){
        this->connected_devices_status.sht85 = DEVICE_CONNECTED;
                #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->SHT85 is connected");
        #endif
    }
    else {
        this->connected_devices_status.sht85 = DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            log_e("SHT85 no found!");
        #endif
        // delete sht85;
    }
    if(this->checkI2CDevices(NO2_O3_I2C_ADDRESS)) {
        this->connected_devices_status.als_toxic_no2_o3 = DEVICE_CONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->Alphasense NO2 & O3 is connected");
        #endif
    }
    else {
        this->connected_devices_status.als_toxic_no2_o3 = DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            log_e("Alphasense NO2 & O3 no found!");
        #endif
    }

    if(this->checkI2CDevices(CO_SO2_I2C_ADDRESS)){
        this->connected_devices_status.als_toxic_co_so2 = DEVICE_CONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->Alphasense CO & SO2 is connected");
        #endif
    }
    else{
        this->connected_devices_status.als_toxic_co_so2 = DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            log_e("Alphasense CO & SO2 no found!");
        #endif
    }

    if(this->checkI2CDevices(DS3231_ADDRESS)) {
        this->connected_devices_status.ds3231=DEVICE_CONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->DS3231 is connected!");
        #endif
    }

    else {
        this->connected_devices_status.ds3231=DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            ESP_LOGE(DEVICETAG, "DS3231 no found!");
        #endif
    }

    if(store.init()) {
        this->connected_devices_status.sd_card=DEVICE_CONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->SD card is connected");
        #endif 
        // store.end();
    }

    else {
        this->connected_devices_status.sd_card=DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            ESP_LOGE(DEVICETAG, "SD card no found!");
        #endif
    }


    if(opcn3.isConnected()) {
        this->connected_devices_status.opcn3=DEVICE_CONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->OPCN3 is connected");
        #endif
    }

    else {
        this->connected_devices_status.opcn3=DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            ESP_LOGE(DEVICETAG, "OPCN3 no found!");
        #endif
    }
    if(gps.isConnected()){
        this->connected_devices_status.gps=DEVICE_CONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->GPS is connected");
        #endif
    }

    else {
        this->connected_devices_status.gps=DEVICE_DISCONNECTED;
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            ESP_LOGE(DEVICETAG, "GPS no found!");
        #endif
    }
}

bool DeviceManager::checkI2CDevices(uint8_t _i2c_address) {
    uint8_t error;
    Wire.beginTransmission(_i2c_address);
    error=Wire.endTransmission();
    if(error==0) 
        return true;
    return false;
}

void DeviceManager::initConnectedDevices() {
    if(this->connected_devices_status.sht85) 
        if(sht85.begin(SHT85_ADDRESS, &Wire1)) {
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->SHT85 initialized completely!");
        #endif
        }
    if(this->connected_devices_status.als_toxic_no2_o3) 
        if(als_toxic.init()) {
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->Alphasense toxic initialized completely!");
        #endif
        }

    if(this->connected_devices_status.ds3231) 
        if(ds3231.init()) {
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->DS3231 initialized completely!");
        #endif
    }

    if(this->connected_devices_status.opcn3) {
        opcn3.initialize();
        Serial.println("->OPCN3 initialized completely!");
    }

    if(this->connected_devices_status.sd_card){
        if(store.init()) {
        #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
            Serial.println("->SD initialized completely");
        #endif
        }
    }
        
    #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
        Serial.println("->Init devices successfully!!");
    #endif // reset state to get data mode
    state.status=DEVICE_STATUS_WORKING;
    state.entry=0;
    state.currrent_state=DEVICE_STATE_INIT;
    this->switchState(DEVICE_STATE_GET_DATA);
}

void DeviceManager::updateDataCore() {
    // turn current value to the previous one
    pre_data_core=this->data_core;
    float new_pm1;
    float new_pm2p5;
    float new_pm10;
    float new_temp;
    float new_humi;
    // float new_pres;
    float new_no2_wmv;
    float new_no2_amv;
    float new_o3_wmv;
    float new_o3_amv;
    float new_co_wmv;
    float new_co_amv;
    float new_so2_wmv;
    float new_so2_amv;

    // now update new data
    updateTimeFromRTC();
    this->data_core.UNIXTime=this->time_now.unix_time - 3600*7;

    if(this->connected_devices_status.als_toxic_no2_o3){
        this->als_toxic.readNO2RawData();
        this->als_toxic.readO3RawData();

        new_no2_wmv = als_toxic.getNO2WEVoltage();
        new_no2_amv = als_toxic.getNO2AEVoltage();
        new_o3_wmv = als_toxic.getO3WEVoltage();
        new_o3_amv = als_toxic.getO3AEVoltage();

        //check data
        this->data_core.NO2_WmV = new_no2_wmv;
        this->data_core.NO2_AmV = new_no2_amv;
        this->data_core.O3_WmV = new_o3_wmv;
        this->data_core.O3_AmV = new_o3_amv;
    }

    if(this->connected_devices_status.als_toxic_co_so2) {
        this->als_toxic.readCORawData();
        this->als_toxic.readSO2RawData();

        new_co_wmv  = als_toxic.getCOWEVoltage();
        new_co_amv = als_toxic.getCOAEVoltage();
        new_so2_wmv = als_toxic.getSO2WEVoltage();
        new_so2_amv = als_toxic.getSO2AEVoltage();

        //check data
        this->data_core.CO_WmV = new_co_wmv;
        this->data_core.CO_AmV = new_co_amv;
        this->data_core.SO2_WmV = new_so2_wmv;
        this->data_core.SO2_AmV = new_so2_amv;

        // if(new_no2 > 0 && new_o3 > 0 ){
        //     this->data_core.NO2_WmV = new_no2_wmv;
        //     this->data_core.O3_WmV = new_o3;
        //     this->data_core.SO2_WmV = new_so2;
        //     this->data_core.CO_AmV = new_co;
        // }
        // else{

        // }
    }

    if(this->connected_devices_status.opcn3) {
        if(this->opcn3.readData()){
            new_pm1 = this->opcn3.getPM1();
            new_pm2p5 = this->opcn3.getPM2p5();
            new_pm10 = this->opcn3.getPM10();
            new_temp = this->opcn3.getTemperature();
            new_humi = this->opcn3.getHumidity();
            if(inRange(PM_MIN_VALUE, PM_MAX_VALUE, new_pm10) ) {
                this->data_core.pm1_0=(float)new_pm1;
                this->data_core.pm2_5=(float)new_pm2p5;
                this->data_core.pm10_0=(float)new_pm10;
            }
            if(inRange(TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, new_temp) && 
                inRange(HUMIDITY_MIM_VALUE, HUMIDITY_MAX_VALUE, new_humi)){
                this->data_core.temperature = new_temp;
                this->data_core.humidity = new_humi;
            }
        }
        else{
            
        }
    }
    if(this->connected_devices_status.sht85){
        // Serial.print("\n->>>>>>>>>>>>>>>>a");
        if(sht85.read()){
            // Serial.print("\n->>>>>>>>>>>>>>>>b");
            new_temp = sht85.getTemperature();
            new_humi = sht85.getHumidity();
            if(inRange(TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, new_temp) && 
                inRange(HUMIDITY_MIM_VALUE, HUMIDITY_MAX_VALUE, new_humi)){
                this->data_core.temperature = new_temp;
                // Serial.print("\n->>>>>>>>>>>>>>>>c");
                this->data_core.humidity = new_humi;
            }
        }
        else{

        }
    }

    if(this->connected_devices_status.gps){
        if(gps.getGPS()){
            this->data_core.longitude = gps.getLongitude();
            this->data_core.latitude =  gps.getLatitude();
        }
        else{

        }
    }

    this->state.entry++;

    // this is for error data check
    if((pre_data_core.pm10_0==this->data_core.pm10_0 && pre_data_core.pm2_5==this->data_core.pm2_5)){
        err_data_count++;
        #ifdef _SERIAL_DEBUG_
            Serial.printf("!!!Data duplicated %d\n", err_data_count);
        #endif
    }
    else {
        err_data_count=0;
    }

}

bool DeviceManager::isSDcardConnected() {
    if(store.isExist()) {
        this->connected_devices_status.sd_card=DEVICE_CONNECTED;
        return true;
    }

    else {
        this->connected_devices_status.sd_card=DEVICE_DISCONNECTED;
        return false;
    }
}

void DeviceManager::updateTimeFromRTC() {
    if(this->connected_devices_status.ds3231) {
        this->ds3231.updateTime();
        this->time_now=this->ds3231.getTime();
        // this->data_core.UNIXTime = this->time_now.unix_time - 3600*7;
    }

    else {
        this->time_now.hour=0;
        this->time_now.minute=0;
    }

}

void DeviceManager::setTimeToRTC(uint32_t _unix_time) {
    if(this->connected_devices_status.ds3231 && _unix_time > 1605666264) {
        this->ds3231.setTime(_unix_time);
    }
}

void DeviceManager::setAlarm() {
    this->ds3231.setAlarm();
}

void DeviceManager::sleep() {
    if(this->connected_devices_status.opcn3) {
        this->opcn3.sleep();
    }

    //other sensors
    if(this->connected_devices_status.als_toxic_co_so2) {

    }
    
    if(this->connected_devices_status.als_toxic_no2_o3) {

    }

    this->state.status=DEVICE_STATUS_SLEEPING;
}

void DeviceManager::wake() {
    if(this->connected_devices_status.opcn3) {
        this->opcn3.wake();
    }

    this->state.status=DEVICE_STATUS_WORKING;
    //other sensors

}

bool DeviceManager::checkAlarm1() {
    return this->ds3231.isAlarm1Fired();
}

bool DeviceManager::checkAlarm2() {
    return this->ds3231.isAlarm2Fired();
}

void DeviceManager::clearAlarm1() {
    this->ds3231.clearAlarm1();
}

void DeviceManager::clearAlarm2() {
    this->ds3231.clearAlarm2();
}

void DeviceManager::logDataCoreToSDCard() {
    if(this->connected_devices_status.sd_card) {
        char file_name[36];
        sprintf(file_name, "/%s %4d-%02d-%02d.txt", this->mac_address,
            this->time_now.year, this->time_now.month, this->time_now.date);
        // sprintf(file_name, "/%02X%02X%02X%02X%02X%02X %4d-%02d-%02d.txt", this->MACAddress[0], this->MACAddress[1],this->MACAddress[2],
        //         this->MACAddress[3], this->MACAddress[4], this->MACAddress[5],
        //         this->time_now.year, this->time_now.month, this->time_now.date);
        if(!store.logData(file_name, this->data_core, this->mac_address, this->time_now)){
            #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
                Serial.println("->try to write to a new file: ");
            #endif
            sprintf(file_name, "/%s %4d-%02d-%02d_s.txt", this->mac_address,
                this->time_now.year, this->time_now.month, this->time_now.date);
            if(store.logData(file_name, this->data_core, this->mac_address, this->time_now)){
                #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
                    Serial.println("SUCCESS");
                #endif
            }
            else{
                #ifdef _SERIAL_DEBUG_DEVICE_MANAGER_
                    Serial.println("FAILED");
                #endif  
            }
        }
        else{
            
        }
        // store.logData("/hello.txt", data_core);        
    }

    this->state.entry=0;
}

void DeviceManager::setPeriodTimeSample() {}

void DeviceManager::calibrate() {}

void DeviceManager::printData() {
    // Serial.print("\nCO: ");
    // Serial.print(data_core.CO);
    Serial.print("->PM1: ");
    Serial.print(data_core.pm1_0);
    Serial.print("\n  PM2.5: ");
    Serial.print(data_core.pm2_5);
    Serial.print("\n  PM10: ");
    Serial.print(data_core.pm10_0);
    Serial.print("\n  Temperature: ");
    Serial.print(data_core.temperature);
    Serial.print("\n  Humidity: ");
    Serial.print(data_core.humidity);
    Serial.print("\n  Pressure: ");
    Serial.print(data_core.pressure);
    Serial.print("\n  NO2: ");
    Serial.print(data_core.NO2_WmV);
    Serial.print("\n  O3: ");
    Serial.print(data_core.O3_WmV);
    Serial.print("\n  UNIX time: ");
    Serial.println(data_core.UNIXTime);
    // Serial.println(time_now.hour);
    // Serial.println(time_now.date);
    // Serial.println(time_now.month);
}

void DeviceManager::waitPeriod(uint16_t _time_stamp) {
    if(millis() > last_switch_state + _time_stamp) {
        if(this->state.previous_state==DEVICE_STATE_WAKE) {
            switchState(DEVICE_STATE_GET_DATA);
        }

        else if(this->state.previous_state==DEVICE_STATE_SLEEP) {
            switchState(DEVICE_STATE_WAKE);
        }

        last_switch_state=millis();
    }
}

DataCore DeviceManager::getDataCore() {
    return this->data_core;
}

TimeNow DeviceManager::getTime() {
    return this->time_now;
}

DeviceManagerState DeviceManager::getState() {
    return this->state;
}

ConnectedDevicesStatus DeviceManager::getConnectionStatus() {
    return this->connected_devices_status;
}

char* DeviceManager::getMAC() {
    return this->mac_address;
}

void DeviceManager::initWDT() {
    // Set watchdog timer 
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);
}

void DeviceManager::deinitWDT() {
    // Set watchdog timer 
    esp_task_wdt_deinit();
}

void DeviceManager::resetWDT() {
    // Reset watchdog timer
    esp_task_wdt_reset();
}

bool DeviceManager::dataError() {

    // if this count exceeds ten, this device should be restart
    if(err_data_count > 10 && this->connected_devices_status.opcn3) {
        return true;
    }

    else {
        return false;
    }
    return false;

}

///state machine thing

// DeviceManager * device_managers = DeviceManager::getInstance();
// void updateDataCore()
// {
//     device_managers->updateDataCore();
//     device_managers->printData();
// }
// void sleep()
// {
//     device_managers->sleep();
//     #ifdef _SERIAL_DEBUG_
//         Serial.println("Devices sleep");
//     #endif
// }
// void wakeUp()
// {
//     device_managers->wake();
//     #ifdef _SERIAL_DEBUG_
//         Serial.println("Devices wake");
//     #endif
// }
// void waitPeriod()
// {   
//     device_managers->waitPeriod();
// }
// void logData()
// {
//     #ifdef _SERIAL_DEBUG_
//         Serial.println("Devices log data");
//     #endif
//     device_managers->logDataCoreToSDCard();
//     Queue * queue = Queue::getInstance();
//     queue->enQueue(device_managers->getDataCore());
// }
// ///function pointer////////////
// void (*action_function_ptr[])(void) = { &sleep, &wakeUp, &updateDataCore, &logData, &waitPeriod};
// void checkDeviceState()
// {
//     action_function_ptr[device_managers->getState().currrent_state]();
// }