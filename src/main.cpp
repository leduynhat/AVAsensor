/**
 * @file main.c
 * @author Le Duy Nhat
 * @brief This main stuff made off 
 * @date 23/10/2020
 */
#include <Arduino.h>
#include "DeviceManager.h"
#include "MQTTConnection.h"
#include "WiFiConnectionManager.h"
#include "UserInterfaceManager.h"
#include "Queue.h"

//////////////////////////////////////////////////////////DEFINE////////////////////////////////////////////////////////////////
#define DEVICE_MANAGER_LABEL "DEVICE"
#define WIFI_MANAGER_LABEL "WIFI"
#define MQTT_LABEL  "MQTT"
#define USER_INTERFACE_MANAGER "UI"

//////////////////////////////////////////////////////GLOBAL VARABLE/////////////////////////////////////////////////////////////
DeviceManager *device_manager =  DeviceManager::getInstance();
Queue *queue = Queue::getInstance();
MQTTConnection mqtt = MQTTConnection();
WiFiConnectionManager wifi_manager = WiFiConnectionManager();
UserInterfaceManager user_interface = UserInterfaceManager();

//Data sum and count data collected
DataCore data_core_sum;
DataCore data_to_publish;
/**
 * If there is no Internet connection when first start device, so time to reconnect will be double 
 * every time WiFi can not be connected and stop at maximun 32 minutes. This value will be reset 
 * to defaut evey time the devices is connected. This will save enegy in case there is no WiFi around 
 * in addition to performing display.
*/
// unsigned long time_to_reconnect_wifi = TIME_TO_RECONNECT_WIFI_DEFAULT;

//static tick
static unsigned long last_check_update_ota = 0;
static unsigned long last_update_screen = 0;

/**
 * function
 */

////////////////////////////FSM//////////////////////////////////////////////////////////////////////////////////
/**********************************************
 * @brief Device State machine
 **********************************************/
void DeviceUpdateDataCore()
{    
    #ifdef _SERIAL_DEBUG_MAIN_
      Serial.println("DEVICE[2] update DataCore");
    #endif
    device_manager->updateDataCore();
    device_manager->printData();
    //switch state
    if(device_manager->getState().entry == 1)
    {
      device_manager->switchState(DEVICE_STATE_LOG_DATA); //log data to SD card
    }
    else
    {
      device_manager->switchState(DEVICE_STATE_SLEEP); // go to sleep
    }
}
void DeviceSleep()
{
  device_manager->sleep();
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("DEIVCE[0] sleep");
  #endif
  device_manager->switchState(DEVICE_STATE_WAIT);
}
void DeviceWakeUp()
{
  device_manager->wake();
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("DEVICE[1] wake");
  #endif
  device_manager->switchState(DEVICE_STATE_WAIT);
}
void DeviceWaitPeriod()
{   
  device_manager->waitPeriod(SAMPLING_TIME);//config->getTimeStamp());
}
void DeviceLogData()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("DEVICE[3] log data to SD");
  #endif
  device_manager->logDataCoreToSDCard();
  queue->enQueue(device_manager->getDataCore());
  device_manager->switchState(DEVICE_STATE_SLEEP);
}
///function pointer////////////
void (*device_function_ptr[])(void) = {&DeviceSleep, &DeviceWakeUp, &DeviceUpdateDataCore, &DeviceLogData, &DeviceWaitPeriod};
void checkDeviceState()
{
    device_function_ptr[device_manager->getState().currrent_state]();
}

/*******************************************
 * @brief WiFi state machine
 *******************************************/
void connectOldWiFi()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("WIFI[0] Connect old WiFi");       
  #endif
  wifi_manager.connect();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void storeWiFiInfoToSD()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("WIFI[1] log info to WIFI.txt");
    Serial.println("->SSID: " + WiFi.SSID());
    Serial.println("->Password: " + WiFi.psk());
  #endif
  // update time to RTC
  device_manager->setTimeToRTC(wifi_manager.updateUnixTime());
  device_manager->updateTimeFromRTC();

  // store WiFi info
  device_manager->store.logWiFiInfo(WiFi.SSID(), WiFi.psk(), WIFI_INFO_FILE);
  // wifi_manager.getWiFiMul()->addAP(&WiFi.SSID()[0], &WiFi.psk()[0]);
  wifi_manager.switchState(WIFI_STATE_GET_WIFI_INFO_FROM_SD);
}
void wifiCheckAndWait()
{
  wifi_manager.waitAndCheckEvent();
}
void wifiSmartConfig()
{
  /**
   * This function is now AutoConnect from WiFiManager
   */
  #ifdef _SERIAL_DEBUG_MAIN_
      // Serial.println("WiFi [4] smartconfig start...");
    Serial.println("WIFI[4] AP starting ...");
  #endif
  // wifi_manager.smartConfig();
  // device_manager->resetWDT();
  user_interface.led_signal.setBlueLED();
  // device_manager->deinitWDT();
  device_manager->resetWDT();
  wifi_manager.startAP();
  device_manager->resetWDT();
  // device_manager->initWDT();
  user_interface.led_signal.resetBlueLED();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void wifiReconnect()
{
  #ifdef _SERIAL_DEBUG_MAIN_
      // Serial.println("WiFi [4] smartconfig start...");
    Serial.println("WIFI[5] Reconnect sequence");
  #endif
  wifi_manager.reconnectSequence();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void wifiConnectBestWifi()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("WIFI[3] Connect best WiFi");       
  #endif   
  wifi_manager.connectBestWiFi();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void wifiGetWifiInfoFromSd()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("WIFI[2] Get WiFi info from SD");       
  #endif   
  device_manager->store.readWiFiInfo(wifi_manager.getWiFiMul());
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void (*wifi_function_ptr[])(void) = {&connectOldWiFi, &storeWiFiInfoToSD, &wifiGetWifiInfoFromSd,
                                      &wifiConnectBestWifi, &wifiSmartConfig, &wifiReconnect, &wifiCheckAndWait};
void checkWiFiState()
{
  wifi_function_ptr[wifi_manager.getState().current_state]();
}


/**********************************************
 * @brief mqtt state machine
 **********************************************/

void mqttCheckAndWait()
{
  mqtt.mqttCheckAndWait(wifi_manager.getState().status == WIFI_STATUS_INTERNET_CONNECTED, !queue->isEmpty());
}
void mqttCreateMessage()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.println("MQTT[1] create message");       
  #endif   
  data_to_publish = queue->deQueue();
  mqtt.encryptDataToJSON(data_to_publish, device_manager->getMAC());
  mqtt.encryptStatusToJSon(device_manager->getTime().unix_time, 
                          device_manager->getConnectionStatus().sd_card,
                          device_manager->getConnectionStatus().opcn3,
                          device_manager->getConnectionStatus().sht85,
                          device_manager->getConnectionStatus().als_toxic_no2_o3,
                          device_manager->getConnectionStatus().als_toxic_co_so2,
                          device_manager->getConnectionStatus().ds3231,
                          device_manager->getConnectionStatus().gps);
                          
  mqtt.switchState(MQTT_STATE_PUBLISH_MESSAGE);
}
void mqttPublishMessage()
{
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.print("MQTT[2] publish message");       
  #endif  
  if(mqtt.publish(true))
  {
    #ifdef _SERIAL_DEBUG_MAIN_
      Serial.println(" successful");       
    #endif  
    mqtt.switchState(MQTT_STATE_CHECK_QUEUE_AND_WAIT);
    //if this is the last message that published successfully, blink the BLUE LED
    if(queue->isEmpty()){
      user_interface.led_signal.blinkLedBlue(5,100);
    }
  }
  else
  {
    #ifdef _SERIAL_DEBUG_MAIN_
      Serial.println(" failed");       
    #endif  
    mqtt.publishFailed();
    mqtt.switchState(MQTT_STATE_PUSH_BACK_DATA_QUEUE);
  }
  
}
void mqttPushBackDataQueue()
{
  #ifdef _SERIAL_DEBUG_MAIN_
      Serial.println("MQTT[3] push back data core");       
  #endif  
  queue->enQueue(data_to_publish);
  mqtt.switchState(MQTT_STATE_CHECK_QUEUE_AND_WAIT);
}

void (*mqtt_function_ptr[])(void) = {&mqttCheckAndWait, &mqttCreateMessage, &mqttPublishMessage, &mqttPushBackDataQueue};

void checkmqttstate()
{
  mqtt_function_ptr[mqtt.getState().current_state]();
  // printf("hello");
}
/**
 * @brief error check
 */
/**
 * @brief ui state machine
 */
void checkUIState(){
  if(millis() > last_update_screen + 15000)
  {
    #ifdef _SERIAL_DEBUG_MAIN_
      Serial.println("UI[0] Display");
    #endif
    last_update_screen = millis();
    user_interface.display.updateData(device_manager->getDataCore(), device_manager->getMAC());
    user_interface.display.updateClock(device_manager->getTime().hour, device_manager->getTime().minute);
    if(device_manager->getConnectionStatus().sd_card)
    {
      user_interface.display.showSDsymbol();
    }
    else
    {
      user_interface.display.showNonSDsymbol();
    }
    if(wifi_manager.getState().status == WIFI_STATUS_INTERNET_CONNECTED)
    {
      user_interface.display.showWiFiSymbol();
    }
    else if(wifi_manager.getState().status ==  WIFI_STATUS_WIFI_CONNECTED_BUT_NO_INTERNET)
    {
      user_interface.display.showWiFiButNoInternetSymbol();
    }
    else
    {
      user_interface.display.showNonWiFiSymbol();
    }
  }
  user_interface.user_input.buttonScan();
}
//////////////////////////////////////////////////////////////SET UP////////////////////////////////////////////////////////////////
void setup() {   
  //initialize hardware
  device_manager->initHardware();
  //set watchdog timer
  user_interface.init();
  //Show up firmware version
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.print("Device name: ");
    Serial.println(DEVICE_NAME);
    Serial.print("Firmware version: ");            
    Serial.println(FIRMWARE_VERSION);            
  #endif

  //Start WiFi mode by Station
  wifi_manager.init();
  wifi_manager.connect();
  delay(2000);
  //Check connected device_manager
  device_manager->checkConnectedDevices();

  delay(1000);
  //initialize user interface
  user_interface.display.progressScreen(device_manager->getConnectionStatus().sd_card,
                                        device_manager->getConnectionStatus().opcn3, 
                                        device_manager->getConnectionStatus().sht85, 
                                        device_manager->getConnectionStatus().als_toxic_no2_o3,
                                        device_manager->getConnectionStatus().als_toxic_co_so2,
                                        device_manager->getConnectionStatus().ds3231);
  // delay(3000);
  // initialize connected devices
  device_manager->initConnectedDevices();

  device_manager->store.readWiFiInfo(wifi_manager.getWiFiMul());
  // delay(1000);
  // if(!wifi_manager.isConnected()){
  //   wifi_manager.connectBestWiFi();
  // }
  //Show main screen including data that just collected and clock.
  user_interface.display.mainScreen();

  if(wifi_manager.isConnected())
    device_manager->setTimeToRTC(wifi_manager.updateUnixTime());
  else if(device_manager->getConnectionStatus().gps){
    device_manager->setTimeToRTC(device_manager->gps.getUnixtime());
  }
  device_manager->updateDataCore();

  user_interface.display.updateData(device_manager->getDataCore(), device_manager->getMAC());
  user_interface.display.updateClock(device_manager->getTime().hour, device_manager->getTime().minute);
  //mqtt thing
  mqtt.init();
  // Serial.println("hasdaskkdhajshdkjashd");
  mqtt.encryptStatusToJSon(device_manager->getTime().unix_time, 
                          device_manager->getConnectionStatus().sd_card,
                          device_manager->getConnectionStatus().opcn3,
                          device_manager->getConnectionStatus().sht85,
                          device_manager->getConnectionStatus().als_toxic_no2_o3,
                          device_manager->getConnectionStatus().als_toxic_co_so2,
                          device_manager->getConnectionStatus().ds3231,
                          device_manager->getConnectionStatus().gps);
  mqtt.connect();
  //if device can't reach mqtt station, show green non wifi symbol
  device_manager->initWDT();
  // xTaskCreate(checkDeviceState, "Devices", 2048, NULL, 1, NULL);
}
void loop() 
{
  /**
   * This is the state machine include 2 state wait and get data.
   * In state one, device collect data then let sensors sleep to save energy.
   * Then data will be display and increase data count by one.
   * If data count equal to three (three data has been collected) the device will log data to SD card 
   * and queue it for publish to mqtt server.
   * In state zero, device wake the sensors up for state one to get data.
   * Interval between two state is store in config class
   */
  // user_interface.display.guiHandler();
  checkDeviceState();
  checkWiFiState();
  checkmqttstate();
  checkUIState();
  // user_interface.display.guiHandler();

  //check for update firmware
  // if(device_manager->getTime().hour == 23 && (millis() > (last_check_update_ota + 300000)))
  if((millis() > (last_check_update_ota + 120000)))
  {
    wifi_manager.updateOTA();
    last_check_update_ota = millis();
  }  
  /**
   * This makes the device restart every 30 days
   * and if there is error from data, we should do restart 
   * to reinitilize sensor thing
   */
  if((millis() > RESET_PERIOD) || (device_manager->dataError()))// && device_manager->getConnectionStatus().opcn3))
  {
    #ifdef _SERIAL_DEBUG_MAIN_
      Serial.println("Resetting device...");
    #endif
    ESP.restart();
  }     
    /**
   * Smartconfig thing
   */
  if(user_interface.user_input.getButtonStatus(1).currentState == BUTTON_HELD)
  {
    wifi_manager.switchState(WIFI_STATE_SMARTCONFIG);
    user_interface.display.showWiFiConfigSymbol();
    // user_interface.led_signal.setBlueLED();
  }
  /**
   * Watchdog timer things
   */
  device_manager->resetWDT();
}
