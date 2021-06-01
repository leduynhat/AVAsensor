#include <Arduino.h>
#include <Store.h>

// #define PIN_NUM_MISO 21
// #define PIN_NUM_MOSI 19
// #define PIN_NUM_CLK  18
// #define PIN_CS_SD_CARD 5

#define PIN_NUM_MISO 18
#define PIN_NUM_MOSI 19
#define PIN_NUM_CLK  5
// #define PIN_NUM_CS   17

#define PIN_CS_SD_CARD 23
Store store;

void setup(){
    Serial.begin(115200);
    SPI.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, -1);
    pinMode(PIN_CS_SD_CARD, OUTPUT);
    store.init();
}   
void loop(){

}