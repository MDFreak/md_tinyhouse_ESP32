/* Copyright (c) 2023 Martin Dorfner (for md_PZEM017)

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the “Software”), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


 */
#ifndef _MAIN_H_
  #define _MAIN_H_

  // standard system includes
    #include <Arduino.h>
    #include <unity.h>
    #include <string.h>
    #include <stdio.h>                                                        // Biblioteca STDIO
    #include <iostream>
  // standard md_library
    #include <md_defines.h>
    #include <md_util.h>
    #include <md_filter.h>
    #include <md_projects.h>
  // check entries from platformio.ini
    #if (PROJECT > 0)
         #define HALLO 1
      #endif
  // project system includes
    #if (USE_I2C > OFF)
        #include <Wire.h>
      #endif // USE_I2C
  // standard project includes
    #if (PROJECT == PRJ_TINY_BASE_DEVKIT)
        #include <prj_tiny_base_devkit.h>
      #endif
    //#include <md_time.h>
  // project devices md_library
    // timer
    // I2C
      #ifdef USE_I2C
          #if (OLED_DRV == OLED_DRV_1306)
              #include <SSD1306Wire.h>
            #endif // OLED_DRV
          #if (OLED_DRV == OLED_DRV_1106)
              #include <SH1106Wire.h>
            #endif // OLED_DRV
        #endif // USE_OLED_I2C
    // SPI
      #ifdef USE_SPI
          #include <SPI.h>
        #endif
    // displays
      #ifdef USE_TFT
          #include <TFT_eSPI.h>
        #endif
    // sensors
      #if defined(USE_BME280_I2C)
          #include <md_BME280.h>
        #endif
      #if defined(USE_INA3221_I2C)
          #include <SDL_Arduino_INA3221.h>
        #endif
      #if defined(USE_PZEM017_RS485)
          #include <md_PZEM017.h>
        #endif
  // --- memory
    #if (USE_FLASH_MEM > OFF)
        #include <FS.h>
        //#include <SPIFFS.h>
        #include <md_spiffs.h>
      #endif
    #if defined(USE_FRAM_I2C)
        #include <md_FRAM.h>
      #endif
    #if defined(USE_SD_SPI)
        //#include "sdmmc_cmd.h"
        #include <FS.h>
        #include <sd.h>
        #include <spi.h>
      #endif
  // --- network
    #if defined(USE_WIFI)
        #include <AsyncTCP.h>
        #include <ESPAsyncWebServer.h>
        #include <md_webserver.h>
        #include <md_ip_list.h>
        #if (USE_MQTT > OFF)
            #include <Network/Clients/MQTT.hpp>
          #endif
      #endif
  // ---------------------------------------
  // --- prototypes
    // ------ system -------------------------
      // --- heap ------------------------
        void heapFree(const char* text);
    // ------ user interface -----------------
      // --- user output
        // --- display
          #ifdef USE_DISP
              void clearDisp();
              void dispStatus(String msg, bool direct = false);
              void dispStatus(const char* msg, bool direct = false);
              void dispText(char* msg, uint8_t col, uint8_t row, uint8_t len = 0);
              void dispText(String msg, uint8_t col, uint8_t row, uint8_t len = 0);
              void startDisp();
            #endif
        // --- LED output
          #if (USE_RGBLED_PWM > OFF)
              void initRGBLED();
            #endif
        // --- passive buzzer
          #ifdef PLAY_MUSIC
              void playSong(int8_t songIdx);
              void playSong();
            #endif
        // --- traffic Light of gas sensor
          #if (USE_MQ135_GAS_ANA > OFF)
              int16_t showTrafficLight(int16_t inval, int16_t inthres);
            #endif
          #if (USE_MQ3_ALK_ANA > OFF)
              int16_t showTrafficLight(int16_t inval, int16_t inthres);
            #endif
        // WS2812 LED
          #if (USE_WS2812_LINE_OUT > OFF)
              void initWS2812Line();
              void FillLEDsFromPaletteColors(uint8_t lineNo, uint8_t colorIndex);
              void ChangePalettePeriodically(uint8_t lineNo);
              #ifdef XXXX
                  void SetupTotallyRandomPalette();
                  void SetupBlackAndWhiteStripedPalette();
                  void SetupPurpleAndGreenPalette();
                #endif
            #endif
          #if (USE_WS2812_MATRIX_OUT > OFF)
              void initWS2812Matrix();
            #endif
      // --- user input
        // --- keypad
          #if defined(KEYS)
              void startKeys();
              uint8_t getKey();
            #endif
        // --- digital input
          #if (USE_DIG_INP > OFF)
              void getDIGIn();
            #endif
          #if (USE_CTRL_POTI > OFF)
              void getADCIn();
            #endif
        // --- counter input
          #if (USE_CNT_INP > OFF)
              static void initPCNT();
              void getCNTIn();
            #endif
      // memory
          #if defined(USE_FLASH_MEM)
              #include <FS.h>
              //#include <SPIFFS.h>
              #include <md_spiffs.h>
            #endif
          #if defined(USE_FRAM_I2C)
              #include <md_FRAM.h>
            #endif
          #if defined(USE_SD_SPI)
              //#include "sdmmc_cmd.h"
              #include <FS.h>
              #include <sd.h>
              #include <spi.h>
            #endif
      // --- sensors
        // --- BME280
          #if defined(USE_BME280_I2C)
              static void initBME280();
            #endif
        // --- ADS1115
          #if defined(USE_ADC1115_I2C)
              static void initADS1115();
              static void startADS1115();
            #endif
        // --- CCS811
          #if (USE_CCS811_I2C > OFF)
              void initCCS811();
            #endif
          #if (USE_INA3221_I2C > OFF)
              void initINA3221();
            #endif
        // --- DS18B20
          #if (USE_DS18B20_1W_IO > OFF)
              String getDS18D20Str();
            #endif
        // --- MQ135 gas sensor
          #if (USE_MQ135_GAS_ANA > OFF)
              int16_t getGasValue();
              int16_t getGasThres();
            #endif
        // --- T-element type K
        // --- photo sensor
          #if (USE_PHOTO_SENS_ANA > OFF)
              void initPhoto();
            #endif
        // poti measure
          #if (USE_POTI_ANA > OFF)
              void initPoti();
            #endif
        // vcc measure
          #if (USE_VCC50_ANA > OFF)
              void initVCC50();
            #endif
          #if (USE_VCC50_ANA > OFF)
              void initVCC33();
            #endif
        // ACS712 current measurement
          #if defined(USE_ACS712_ANA)
              void initACS712();
            #endif
    // ------ memory ---------------------------
          #if (USE_FLASH_MEM > OFF)
              void testFlash();
            #endif
    // ------ network -------------------------
      // --- WIFI
        #if defined(USE_WIFI)
            uint8_t startWIFI(bool startup);
            #if (USE_NTP_SERVER > OFF)
                void    initNTPTime();
              #endif
          #endif
      // --- webserver
        #if defined(USE_WEBSERVER)
            //void handlingIncomingData(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);
            //void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
            //             void * arg, uint8_t *data, size_t len);
            //void configWebsite();
            void startWebServer();
            void readWebMessage();
            void sendMessage();
          #endif
      // --- MQTT
        #if (USE_MQTT > OFF)
            void startMQTT();
            void connectMQTT();
            void soutMQTTerr(String text, int8_t errMQTT);
            void readMQTTmsg();
          #endif
    // ----- projects
      #if (PROJECT == PRJ_TEST_LIB_OLED)
          void drawLines();
          void drawRect();
          void fillRect();
          void drawCircle();
          void printBuffer();
          void drawFontFaceDemo();
          void drawTextFlowDemo();
          void drawTextAlignmentDemo();
          void drawRectDemo();
          void drawCircleDemo();
          void drawProgressBarDemo();
          void writeTextDemo();
          //void drawImageDemo()
        #endif
      #if (PROJECT == PRJ_TEST_LIB_TFT)
          void draw_Julia(float c_r, float c_i, float zoom);
        #endif
      #if defined(USE_BME280_I2C)
          void initBME280();
        #endif
      #if defined(USE_INA3221_I2C)
          void initINA3221();
        #endif
      #if defined(USE_PZEM017_RS485)
          void initPZEM017();
          void listPZEMaddr();
        #endif
#endif // _MAIN_H_
