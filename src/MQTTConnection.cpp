#include "MQTTConnection.h"
#include "WiFi.h"

#define _SERIAL_DEBUG_

static unsigned long last_check_queue = 0;
static unsigned long last_check_mqtt_connection = 0;

WiFiClient esp_wifi;
MQTTClient mqttClient(MQTT_BUFFER_SIZE);

float round2(float value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
float round3(float value) {
   return (int)(value * 1000 + 0.5) / 1000.0;
}
double round6(double value){
    return (int)(value * 1000000 + 0.5)/1000000.0;
}
MQTTConnection::MQTTConnection()
{

}
MQTTConnection::~MQTTConnection()
{
    
}
void messageReceived(String &topic, String &payload) {
    Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
    if(payload == "rst"){
        ESP.restart();
    }
    else if (payload == "repub"){

    }
}

// void MQTTCallBack(char* _topic, byte* _mes, unsigned int _len)
// {
  
// }
void MQTTConnection::switchState(uint8_t _next_state)
{
    this->state.previous_state = this->state.current_state;
    this->state.current_state = _next_state;
}
void MQTTConnection::init()
{
    info.broker = MQTT_BROKER;
    info.port = MQTT_PORT;
    // info.authenString = "";
    String _mac = WiFi.macAddress();
    info.mqttTopicPub = String("/AVA/") + _mac;
    info.mqttTopicSub = String("/cmd/") + _mac;
    #ifdef _SERIAL_DEBUG_MQTT_
        Serial.print("->Publish topic: ");
        Serial.println(info.mqttTopicPub);
    #endif
    info.mqttUser = MQTT_USER;
    info.mqttPWD = MQTT_PASSWORD;
    info.mqtt_ID = _mac;
    
    mqttClient.setKeepAlive(KEEP_ALIVE_PERIOD);
    
    // mqttClient.subscribe(info.mqttTopicPub.c_str());
    // mqttClient.setCallback(MQTTCallBack);
    // mqttClient.setServer("23.89.159.119", 1884);
    mqttClient.begin(MQTT_BROKER, MQTT_PORT, esp_wifi);
    
    // mqttClient.begin(esp_wifi);
    // mqttClient.setHost(&info.broker[0], info.port);

    // state machine things
    this->state.current_state = MQTT_STATE_INIT;
    this->state.status = MQTT_STATUS_DISCONNECTED;
    this->switchState(MQTT_STATE_CHECK_QUEUE_AND_WAIT);
}

bool MQTTConnection::isConnected()
{
    return mqttClient.connected();
}

bool MQTTConnection::connect()
{
    bool ret = mqttClient.connect(info.mqtt_ID.c_str(), MQTT_USER, MQTT_PASSWORD);
    if(ret == true)
    {
        mqttClient.subscribe(info.mqttTopicSub);
        this->publish(true, messageStautus, info.mqttTopicPub);
        // mqttClient.publish("/hello/mqtt", "cacban");
        // mqttClient.loop();
        this->state.status = MQTT_STATUS_CONNECTED;
    }
    else
    {
        this->state.status = MQTT_STATUS_DISCONNECTED;
    }
    return ret;
}

void MQTTConnection::disconnect()
{
    mqttClient.disconnect();
}
void MQTTConnection::keepConnection()
{
    mqttClient.loop();
}
void MQTTConnection::subcribe(const char *topic)
{
    mqttClient.subscribe(topic);
    mqttClient.loop();
}

void MQTTConnection::unSubcribe(const char *topic)
{
    mqttClient.unsubscribe(topic);
    mqttClient.loop();
}

bool MQTTConnection::publish(bool _retained, String _mes, String _topic)
{
    if(_topic == "" && _mes == "" && messageData != "")
    {
        // Serial.println(info.mqttTopicPub.c_str());
        // Serial.println(messageData.c_str());
        return mqttClient.publish(info.mqttTopicPub, messageData, _retained, 0);
        // return mqttClient.publish(info.mqttTopicPub, "hello");
    }
    else
    {
        return mqttClient.publish(_topic, _mes, _retained, 0);
    }
    WiFi.macAddress();
}

void MQTTConnection::encryptDataToJSON(DataCore _data_core, char* _mac)
{
    // sprintf(device_name, "%2X%2X%2X%2X%2X%2X", _data_core.MACAddress[0], _data_core.MACAddress[1], _data_core.MACAddress[2], 
    //                                             _data_core.MACAddress[3], _data_core.MACAddress[4], _data_core.MACAddress[5]);
    // sprintf(device_name, "%2X%2X%2X%2X%2X%2X", _mac[0], _mac[1], _mac[2], 
    //                                             _mac[3], _mac[4], _mac[5]);
    DynamicJsonDocument doc(300);
    this->messageData = "";
    /**
     * @brief JSON
     * {
     * "StationId":012345678912
     *  
     * }
     */
    // JsonObject DATA = doc.createNestedObject("DATA");
    // DATA["CO"]        = 0; 
    // DATA["Hum"]       = _data_core.humidity;
    // DATA["Pm1"]       = _data_core.pm1_0;
    // DATA["Pm10"]      = _data_core.pm10_0;
    // DATA["Pm2p5"]     = _data_core.pm2_5;
    // DATA["Time"]      = _data_core.UNIXTime;
    // DATA["Tem"]       = _data_core.temperature;
    // doc["NodeId"]     = device_name;

    doc["StationId"]    = _mac;
    doc["Time"]         = _data_core.UNIXTime;
    doc["PM2p5"]        = round2(_data_core.pm2_5);
    doc["PM10"]         = round2(_data_core.pm10_0);
    doc["PM1"]          = round2(_data_core.pm1_0);
    doc["Temperature"]  = round2(_data_core.temperature);
    doc["Humidity"]     = round2(_data_core.humidity);
    // doc["Pressure"]     = round2(_data_core.pressure);
    doc["NO2W"]          = round3(_data_core.NO2_WmV);
    doc["NO2A"]          = round3(_data_core.NO2_AmV);
    doc["O3W"]           = round3(_data_core.O3_WmV);
    doc["O3A"]           = round3(_data_core.O3_AmV);
    doc["COW"]           = round3(_data_core.CO_WmV);
    doc["COA"]           = round3(_data_core.CO_AmV);
    doc["SO2W"]          = round3(_data_core.SO2_WmV);
    doc["SO2A"]          = round3(_data_core.SO2_AmV);
    doc["Lon"]          = round6(_data_core.longitude);
    doc["Lat"]          = round6(_data_core.latitude);
    // doc["CO2"]          = round2(_data_core.CO2);
    serializeJson(doc, messageData);
    #ifdef _SERIAL_DEBUG_MQTT_
        Serial.println("->mqtt message" + messageData);
    #endif
    // return messageData;
}
void MQTTConnection::encryptStatusToJSon(uint32_t _unix_time, bool _sd , bool _opcn3, bool _sht85, bool _als_toxic_no2_o3, bool _als_toxic_co_so2 , bool _ds3231 , bool _neom8n)
{
    this->messageStautus = "";
    bool is_connected = WiFi.isConnected();
    DynamicJsonDocument doc(300);
    doc["Device"] = DEVICE_NAME;
    doc["Firmware"] = FIRMWARE_VERSION;
    doc["SD"] = _sd?1:0;
    doc["AlsNO2O3"] = _als_toxic_no2_o3?1:0;
    doc["AlsCOSO2"] = _als_toxic_co_so2?1:0;
    doc["SHT85"] = _sht85?1:0;
    doc["DS3231"] = _ds3231?1:0;
    doc["opcn3"] = _opcn3?1:0;
    doc["NeoM8N"] = _neom8n?1:0;
    doc["Time"] = _unix_time;
    doc["SSID"] = is_connected?WiFi.SSID():"none";
    doc["Password"] = is_connected?WiFi.psk():"none";
    serializeJson(doc, messageStautus);
    // return messageStautus;
}
MQTTInfo *MQTTConnection::getInfo()
{
    return &info;
}
MQTTState MQTTConnection::getState()
{
    return state;
}
void MQTTConnection::setStatusMessage(String _mes)
{
    this->messageStautus = _mes;
}
void MQTTConnection::decryptJSON(DataCore* _data_core, String _payload)
{
}
void MQTTConnection::reconnect()
{
    #ifdef _SERIAL_DEBUG_MQTT_
        Serial.println("->MQTT reconnecting...");
    #endif
    this->connect();
}
// bool MQTTConnection::timeToCheckMqttConnection()
// {
//     if(millis() > last_check_mqtt_connection + 5000)
//     {
//         last_check_mqtt_connection = millis();
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }
// bool MQTTConnection::timeToCheckQueue()
// {
//     if(millis() > last_check_queue + 200)
//     {
//         last_check_queue = millis();
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }
void MQTTConnection::mqttCheckAndWait(uint8_t _is_internet, uint8_t _is_queue)
{
    if(millis() > (last_check_queue + 300))
    {        
        if((this->state.previous_state == MQTT_STATE_PUSH_BACK_DATA_QUEUE 
        || this->state.previous_state == MQTT_STATE_INIT) && _is_internet)
        {
            if(millis() > last_check_mqtt_connection + 5000)
            {
                if(this->isConnected())
                {
                    this->state.status = MQTT_STATUS_CONNECTED;
                }
                else
                {
                    this->state.status = MQTT_STATUS_DISCONNECTED;
                    this->reconnect();
                }
                last_check_mqtt_connection = millis();   
            }   
        }
        if(_is_queue && this->state.status == MQTT_STATUS_CONNECTED)
        {
            this->switchState(MQTT_STATE_CREATE_MESSAGE);
        }
        
        // if(this->state.previous_state == MQTT_STATE_INIT)
        // {
        //     this->publish(true, this->messageStautus, this->info.mqttTopicPub);
        // }
        last_check_queue = millis();  
    }
}
void MQTTConnection::publishFailed()
{
    this->state.status = MQTT_STATUS_DISCONNECTED;
}