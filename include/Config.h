/**
 * @file Config.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief This file contains configuration parameters including hardware pin definition, 
 * calibaraions of sensors and server things. Those can be adjusted by either user interface 
 * or server mqtt.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

// #define _DEMO_VERSION_
#define _VERSION_1_0_0_
#ifdef _DEMO_VERSION_
/**
 * @brief SPI configuration
 * 
 */
#define SPI_MISO_PIN (21)
#define SPI_MOSI_PIN (19)
#define SPI_CLK_PIN  (18)

#define SPI2_MISO_PIN (17)
#define SPI2_MOSI_PIN (16)
#define SPI2_CLK_PIN   (4)

// #define SPI2_MISO_PIN (23)
// #define SPI2_MOSI_PIN (22)
// #define SPI2_CLK_PIN   (25)

#define SPI2_CLOCK_SPEED (500000)

#define SPI_CS_SD_CARD (5)
#define SPI2_CS_OPCN3 (2)   

#define PIN_TFT_RST     (15)
#define PIN_TFT_LED     (14)
#define PIN_TFT_DC      (12)
#define PIN_TFT_CS      (13)

/**
 * @brief I2C configuration
 * 
 */
#define I2C_SDA_PIN (26)
#define I2C_SCL_PIN (27)
#define I2C_CLOCK_SPEED (100000)

/**
 * @brief LED pin configuration
 * 
 */
#define PIN_LED_RED     (25)
#define PIN_LED_GREEN     (33)
#define PIN_LED_BLUE     (32)

#define PIN_NEO_PIXEL   (23)

/**
 * @brief Button pin configuration
 * 
 */
#define PIN_BUTTON_1    (35)
#define PIN_BUTTON_2    (34)

#define PIN_OUTA        (4)
#define PIN_OUTB        (2)
#define PIN_SWITCH      (15)

#endif

#ifdef _VERSION_1_0_0_
/**
 * @brief SPI configuration
 * 
 */
#define SPI2_MISO_PIN (33)
#define SPI2_MOSI_PIN (25)
#define SPI2_CLK_PIN   (32)
#define SPI2_CLOCK_SPEED (500000)

#define SPI2_CS_OPCN3 (26)   

#define SPI_MISO_PIN (18)
#define SPI_MOSI_PIN (19)
#define SPI_CLK_PIN  (5)
#define SPI_CLOCK_SPEED  (4000000)

#define SPI_CS_SD_CARD (23)


#define SPI_CS_TFT      (2)
#define PIN_TFT_RST     (22)
#define PIN_TFT_LED     (4)
#define PIN_TFT_DC      (21)

#define SPI_CS_EPD      (2)
#define PIN_EPD_RST     (22)
#define PIN_EPD_BUSY    (4)
#define PIN_EPD_DC      (21)

/**
 * @brief I2C configuration
 * 
 */
#define I2C_SDA_PIN (15)
#define I2C_SCL_PIN (13)
#define I2C_CLOCK_SPEED (100000)

/**
 * @brief LED pin configuration
 * 
 */
#define PIN_LED_BLUE     (12)


/**
 * @brief Button pin configuration
 * 
 */
#define PIN_BUTTON_1    (35)
#define PIN_BUTTON_2    (34)
/**
 * @brief SIM808
 * 
 */
#define SIM_TX_PIN      (27)
#define SIM_RX_PIN      (34)
#define SIM_PWR_PIN     (14)
#define SERIAL_SIM_BAUDRATE (9600)
#endif
/**
 * @brief New I2C
 * 
 */
#define I2C_2_SDA_PIN 14
#define I2C_2_SCL_PIN 27
/**
 * @brief Devive parametter
 * 
 */
#define RESET_PERIOD (2592000000) //30 days
#define SAMPLING_TIME (10000) // 10 seconds
#define DEVICE_NAME "AVA sensor V1"
#define FIRMWARE_VERSION "1.3.3"

/**
 * @brief Serial debug enable 
 * 
 */
#define SERIAL_DEBUG_BAUDRATE (115200)
#define _SERIAL_DEBUG_ALL_
#define _SERIAL_DEBUG_DEVICE_MANAGER_
#define _SERIAL_DEBUG_ILI9341_DISPLAY_
#define _SERIAL_DEBUG_MQTT_
// #define _SERIAL_DEBUG_QUEUE_
#define _SERIAL_DEBUG_STORE_
#define _SERIAL_DEBUG_UI_MANAGER_
#define _SERIAL_DEBUG_WIFI_MANAGER_
#define _SERIAL_DEBUG_MAIN_
/**
 * @brief Main raw data struct 
 * 
 */
struct DataCore{
   // uint8_t MACAddress[6];
   unsigned long UNIXTime;
   float pm2_5;
   float pm10_0;
   float pm1_0;
   float temperature;
   float humidity;
   float pressure;
   float SO2_WmV;
   float SO2_AmV;
   float NO2_WmV;
   float NO2_AmV;
   // float CO2;
   float CO_WmV;
   float CO_AmV;
   float O3_WmV;
   float O3_AmV;
   double longitude;
   double latitude;
};

/**
 * @brief Main time struct
 * 
 */
struct TimeNow{
    u_char second;
    u_char minute;
    u_char hour;
    u_char date;
    u_char month;
    u_int year;
    uint32_t unix_time;
};
enum SCREEN_ID {PROGRESS_SCREEN, 
    MAIN_SCREEN, 
    MENU_SCREEN, 
    SETUP_SCREEN, 
    WIFI_SCREEN, 
    CONFIG_SCREEN};


#endif