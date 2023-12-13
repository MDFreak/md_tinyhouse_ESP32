#include <Arduino.h>
#include <main.h>

#if (WORKSPACE == ESP32_TEST_MD_STDLIB)
  // ----------------------------------------------------------------
  // --- tests for temporary usage
  // ----------------------------------------------------------------
    #define SCAN_I2C
    //#define TEST_OLED
  // ----------------------------------------------------------------
  // --- declarations
  // ----------------------------------------------------------------
  // --- system global control parameters
    static uint64_t anzUsCycles = 0ul;
    static uint64_t usLast      = 0ul;
    static uint64_t usTmp       = 0ul;
    static uint64_t usPerCycle  = 0ul;
    static uint32_t freeHeap    = 10000000;
    static uint8_t  firstrun    = true;
  // --- system local usable values
    static int8_t   tmp_i8      = 0;
    static int16_t  tmp_i16     = 0;
    static int32_t  tmp_i32     = 0;
    static String   outStr      = "";
    static char     tmp_c32[33] = "";
  // --- system devices
    // i2C devices
    #ifdef USE_I2C
        #ifdef I2C1
            TwoWire i2c1 = TwoWire(0);
          #endif
        #ifdef I2C2
            TwoWire i2c2 = TwoWire(1);
          #endif
      #endif // USE_I2C
    // SPI devices
    #if ( DEV_VSPI > OFF )
        //SPIClass pVSPI(VSPI);
      #endif
    #if ( DEV_HSPI > OFF )
        //SPIClass pHSPI(HSPI);
      #endif
  // --- system services
    #ifdef USE_STATUS
        msTimer     statT  = msTimer(STAT_DELTIME_MS);
        msTimer     statN  = msTimer(STAT_NEWTIME_MS);
        char        statOut[60 + 1] = "";
        bool        statOn = false;
        bool        statDate = false;
          //char        timeOut[STAT_LINELEN + 1] = "";
      #endif
  // --- project devices
    // oled
      #ifdef USE_OLED_I2C
          #if (OLED_I2C == DEV_I2C1)                //#define OLED_I2C_SCL       PIN_I2C1_SCL
              #define OLED_I2C_SCL       PIN_I2C1_SCL
              #define OLED_I2C_SDA       PIN_I2C1_SDA
            #endif
          #if (OLED_I2C == DEV_I2C2)                //#define OLED_I2C_SCL       PIN_I2C1_SCL
              #define OLED_I2C_SCL       PIN_I2C2_SCL
              #define OLED_I2C_SDA       PIN_I2C2_SDA
            #endif
          #if (OLED_DRV == OLED_DRV_1306)
              //#define OLED_I2C_SDA       PIN_I2C1_SDA
              SSD1306Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED_GEO);
            #else
              //SH1106Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED_GEO);
              SH1106Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED_GEO);
            #endif // OLED_DRV
          char outBuf[DISP_MAXCOLS + 1] = "";
          #ifdef TEST_OLED
              int oledCnt = 1;
            #endif
        #endif // USE_OLED_I2C
      #ifdef USE_TFT
          TFT_eSPI          tft            = TFT_eSPI();  // Invoke custom library
          const   uint16_t  MAX_ITERATION  = 300; // Nombre de couleurs
          #define SCREEN_WIDTH               tft.width()  //
          #define SCREEN_HEIGHT              tft.height() // Taille de l'écran
          static  float     zoom           = 0.5;
          char              outBuf[DISP_MAXCOLS + 1] = "";
        #endif
  // ------ user input ---------------
  // ------ user output ---------------
  // --- system cycles ---------------
    #ifdef USE_INPUT_CYCLE
        static msTimer inputT           = msTimer(INPUT_CYCLE_MS);
        static uint8_t inpIdx   = 0;
      #endif
    #ifdef USE_OUTPUT_CYCLE
        static msTimer outpT            = msTimer(OUTPUT_CYCLE_MS);
        static uint8_t outpIdx  = 0;
      #endif
    #ifdef USE_DISP
        static msTimer dispT            = msTimer(DISP_CYCLE_MS);
        static uint8_t dispIdx  = 0;
        uint32_t      ze     = 1;      // aktuelle Schreibzeile
      #endif
    // memory
      #if (USE_FRAM_I2C > OFF)
          #define FRAM1_I2C_ADDR      I2C_FRAM_50
          #define FRAM1_SIZE          0x8000
          #define FRAM1_I2C           I2C1
        #endif // USE_FRAM_I2C
    // sensors
      #if (USE_CCS811_I2C > OFF )
          #define CCS811_I2C_ADDR     I2C_CSS811_
          #define CCS811_I2C          I2C1
        #endif // USE_CCS811_I2C
      #if (USE_BME680_I2C > OFF )
          #define BME680_I2C         I2C1
        #endif // USE_BME680_I2C
      #if (USE_BME280_I2C > OFF) // 1
          static md_BME280  bme280_1;
          //static md_BME280* pbme280_1 = &bme280_1;
          #if (BME280_I2C == DEV_I2C1)
              static TwoWire* pbme280i2c  = &Wire;
            #else
              static TwoWire* pbme280i2c  = &Wire1;
            #endif
            //md_val<float> bme280TVal;
            //md_val<float> bme280PVal;
            //md_val<float> bme280HVal;
          float         bme280T       = 0;
          float         bme280P       = 0;
          float         bme280H       = 0;
          float         bme280Told    = MD_F_MAX;
          float         bme280Pold    = MD_F_MAX;
          float         bme280Hold    = MD_F_MAX;
          static String valBME280T    = "";
          static String valBME280P    = "";
          static String valBME280H    = "";
          //static int8_t pubBME280T    = OFF;
          //static int8_t pubBME280H    = OFF;
          //static int8_t pubBME280P    = OFF;
          static int8_t bme280da      = FALSE;
          #if (USE_MQTT > OFF)
              static String topBME280t = MQTT_BME280T;
              static String topBME280p = MQTT_BME280P;
              static String topBME280h = MQTT_BME280H;
            #endif
        #endif // USE_BME280_I2C
    #ifdef USE_INA3221_I2C
        SDL_Arduino_INA3221 ina3221(INA3221_ADDR); // def 0.1 ohm
        #if (INA3221_I2C == DEV_I2C1)
            TwoWire* ina1i2c = &i2c1;
          #else
            TwoWire* ina1i2c = &i2c2;
          #endif
        //md_val<float>    inaIVal[USE_INA3221_I2C][3];
        //md_scale<float>  inaISkal[USE_INA3221_I2C][3];
        //md_val<float>    inaUVal[USE_INA3221_I2C][3];
        //md_scale<float>  inaUSkal[USE_INA3221_I2C][3];
        static float     inaI   [USE_INA3221_I2C][3];
        static float     inaU   [USE_INA3221_I2C][3];
        static float     inaP   [USE_INA3221_I2C][3];
        static float     inaIold[USE_INA3221_I2C][3];
        static float     inaUold[USE_INA3221_I2C][3];
        static float     inaPold[USE_INA3221_I2C][3];
        static String    valINA3221i[USE_INA3221_I2C][3];
        static String    valINA3221u[USE_INA3221_I2C][3];
        static String    valINA3221p[USE_INA3221_I2C][3];
        static int8_t    pubINA3221i[USE_INA3221_I2C][3];
        static int8_t    pubINA3221u[USE_INA3221_I2C][3];
        static int8_t    pubINA3221p[USE_INA3221_I2C][3];
        #if (USE_MQTT > OFF)
            static String topINA32211i[3] = { MQTT_INA32211I1, MQTT_INA32211I2, MQTT_INA32211I3 };
            static String topINA32211u[3] = { MQTT_INA32211U1, MQTT_INA32211U2, MQTT_INA32211U3 };
            static String topINA32211p[3] = { MQTT_INA32211P1, MQTT_INA32211P2, MQTT_INA32211P3 };
          #endif
      #endif
// ----------------------------------------------------------------
// --- system setup -----------------------------------
// ----------------------------------------------------------------
  void setup()
    {
      // start system
        Serial.begin(SER_BAUDRATE);
        usleep(3000); // power-up safety delay
        STXT();
        STXT(" setup start ...");
      // disable watchdog
        STXT(" ... disable WD ...");
        disableCore0WDT();
        //disableCore1WDT();
        disableLoopWDT();
      // start I2C
        #if defined(USE_I2C)
                Wire.setPins(PIN_I2C1_SDA, PIN_I2C1_SCL);
            #ifdef SCAN_I2C
                scanI2C(&Wire, PIN_I2C1_SDA, PIN_I2C1_SCL);
              #endif
          #endif
      // start display
        #ifdef USE_DISP
            startDisp();
          #endif
      // BME280 temperature, pessure, humidity
        #if (USE_BME280_I2C > OFF)
            initBME280();
          #endif
      // temp. current sensor INA3221
        #if (USE_INA3221_I2C > OFF)
            initINA3221();
          #endif
     STXT(" ... setup finished");
    }
// ----------------------------------------------------------------
// --- system loop  ---------------
// ----------------------------------------------------------------
  void loop()
    {
      // system loop intro
        anzUsCycles++;
        tmp_i32 = ESP.getFreeHeap();
        //heapFree("+loop");
        if (tmp_i32 < freeHeap)
          {
            freeHeap = tmp_i32;
            heapFree(" loop ");
          }
        tmp_i32 = ESP.getFreeHeap();
        //heapFree("+loop");
        if (tmp_i32 < freeHeap)
          {
            freeHeap = tmp_i32;
            heapFree(" loop ");
          }
        if (firstrun == true)
          {
            SVAL("loop task running on core ", xPortGetCoreID());
            usLast = micros();
            //firstrun = false;
          }
        #ifdef USE_DISP
            outStr   = "";
          #endif
      // library tests
        #if (PROJECT == PRJ_TEST_LIB_OLED)
            if (firstrun == true) { STXT(" run TEST_LIB_OLED "); }
            oled.clear(); drawLines();
            oled.clear(); drawRect();
            oled.clear(); fillRect();
            oled.clear(); drawCircle();
                //oled.clear(); printBuffer();
            oled.clear(); drawFontFaceDemo();
                //oled.clear(); drawTextFlowDemo();
                //oled.clear(); drawTextAlignmentDemo();
                //oled.clear(); drawRectDemo();
                //oled.clear(); drawCircleDemo();
            oled.clear(); drawProgressBarDemo();
            oled.clear(); writeTextDemo();
          #endif
        #if (PROJECT == PRJ_TEST_LIB_BME280)
            if (firstrun == true)
              {
                STXT(" run TEST_LIB_BME280 ");
                oled.clearUser();
              }
            dispStatus("PRJ_TEST_LIB_BME280 läuft", TRUE);
            bme280T = round( bme280_1.readTemperature() * 10 ) / 10;
            sprintf(tmp_c32, "BME280 T %.1f %%", bme280T);
            dispText(tmp_c32, 0, 1);
            bme280H = round( bme280_1.readHumidity() * 10 ) / 10;
            sprintf(tmp_c32, "BME280 H %.0f %%", bme280H);
            dispText(tmp_c32, 0, 2);
            bme280P = round( bme280_1.readPressure() / 100 );
            sprintf(tmp_c32, "BME280 P %.0f %%", bme280P);
            dispText(tmp_c32, 0, 3);
            Serial.printf(" BME280  T = %.1f°C  P = %.0fmbar  H = %.0f%% \n", bme280T, bme280P, bme280H);
            bme280_1.takeForcedMeasurement();
          #endif
        #if (PROJECT == PRJ_TEST_LIB_TFT)
            if (firstrun == true)
              {
                STXT(" run TEST_LIB_TFT ");
                //oled.clearUser();
              }
            draw_Julia(-0.8,+0.156,zoom);
            tft.fillRect(0, 0, 150, 20, TFT_BLACK);
            //tft.setcursor(0,15);
            tft.setTextColor(TFT_WHITE);
            tft.print(" Zoom = ");
            tft.println(zoom);
            delay(2000);
            zoom *= 1.5;
            if (zoom > 100) zoom = 0.5;
          #endif
        #if (PROJECT == PRJ_TEST_LIB_INA3221)
          #if (USE_INA3221_I2C > OFF)
              // U 3.3V supply
                inaU[0][0] = ina3221.getBusVoltage_V(1);
                  //SVAL(" U 3.3    new ", inaU[0][0]);
                  sprintf(outBuf, "U33 %.1fV", inaU[0][0]);
                  STXT(outBuf);
                  dispText(outBuf,0,1);
                #if (INA3221I1_FILT > OFF)
                    inaU[0][0] = ccsCVal.doVal(inaU[0][0]);
                  #endif
                    //S2VAL(" incycle 3221 ina[0][0] inaUold[0][0] ", inaUold[0][0], inaU[0][0]);
              // I 3.3V not used
                #ifdef NOTUSED
                    inaI[0][0] = ina3221.getCurrent_mA(1);
                  #endif
              // P 3.3V not used
              // U 5V supply
                inaU[0][1] = ina3221.getBusVoltage_V(2);
                  //SVAL(" U 5.0    new ", inaU[0][1]);
              #if (INA3221I1_FILT > OFF)
                    inaU[0][1] = ccsCVal.doVal(inaU[0][1]);
                  #endif
              // I 5V supply
                inaI[0][1] = -ina3221.getCurrent_mA(2);
                  //SVAL(" I 5.0    new ", inaI[0][1]);
              // P 5V supply
                inaP[0][1] = (inaU[0][1] * inaI[0][1]);
                  //SVAL(" P 5.0    new ", inaP[0][1]);
                  sprintf(outBuf, "U50 %.1fV %.1fmA %.1fmW", inaU[0][1], inaI[0][1], inaP[0][1]);
                  STXT(outBuf);
                  dispText(outBuf,0,2);
              // U main supply 12V/19V supply
                inaU[0][2] = ina3221.getBusVoltage_V(3);
                  //SVAL(" U supply new ", inaU[0][2]);
                #if (INA3221U3_FILT > OFF)
                    inaU[0][2] = ccsCVal.doVal(inaU[0][2]);
                  #endif
              // I main supply 12V/19V supply
                inaI[0][2] = -ina3221.getCurrent_mA(3);
                  //SVAL(" I supply new ", inaI[0][2]);
                #if (INA3221I3_FILT > OFF)
                    inaI[0][2] = ccsCVal.doVal(inaI[0][2]);
                  #endif
              // P main supply
                inaP[0][2] = (inaU[0][2] * inaI[0][2]);
                  //SVAL(" P supply new ", inaP[0][2]);
                  sprintf(outBuf, "UIn %.1fV %.1fmA %.1fmW", inaU[0][2], inaI[0][2], inaP[0][2]);
                  STXT(outBuf);
                  dispText(outBuf,0,3);
            #endif
        #endif
      // sensoren
        #if (USE_BME280_I2C > OFF)

          #endif
      // --- Display -------------------
        #ifdef USE_DISP
        #ifdef USE_OUTPUT_CYCLE
            if (outpT.TOut())
              {
                outpIdx++;
                outpT.startT();
                switch(outpIdx)
                  {
                    case 1:  // BME280_I2C / BME680
                        #if (USE_BME280_I2C > OFF)
                            if (bmeT != bmeTold)
                              {
                                    //SVAL(" 280readT  new ", bmeT);
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME280t = bmeT;
                                        errMQTT = (int8_t) mqtt.publish(topBME280t.c_str(), (uint8_t*) valBME280t.c_str(), valBME280t.length());
                                        soutMQTTerr(topBME280t.c_str(), errMQTT);
                                            //SVAL(topBME280t, valBME280t);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA0";
                                    tmpval16 = (int16_t) (bmeT+ 0.5);
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bmeTold = bmeT;
                              }
                            if (bmeP != bmePold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME280p = bmeP;
                                        errMQTT = (int8_t) mqtt.publish(topBME280p.c_str(), (uint8_t*) valBME280p.c_str(), valBME280p.length());
                                        soutMQTTerr(topBME280p.c_str(), errMQTT);
                                            //SVAL(topBME280p, valBME280p);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA1";
                                    tmpval16 = (uint16_t) bmeP;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bmePold = bmeP;
                              }
                            if (bmeH != bmeHold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME280h = bmeH;
                                        errMQTT = (int8_t) mqtt.publish(topBME280h.c_str(), (uint8_t*) valBME280h.c_str(), valBME280h.length());
                                        soutMQTTerr(topBME280h.c_str(), errMQTT);
                                            //SVAL(topBME280h, valBME280h);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA2";
                                    tmpval16 = (int16_t) bmeH;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bmeHold = bmeH;
                              }
                          #endif
                        #if (USE_BME680_I2C > OFF)
                            if (bme680T != bme680Told)
                              {
                                    //SVAL(" 680readT  new ", bmeT);
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680t = bme680T;
                                        errMQTT = (int8_t) mqtt.publish(topBME680t.c_str(), (uint8_t*) valBME680t.c_str(), valBME680t.length());
                                        soutMQTTerr(topBME680t.c_str(), errMQTT);
                                            //SVAL(topBME280t, valBME280t);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA0";
                                    tmpval16 = (int16_t) (bme680T+ 0.5);
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Told = bme680T;
                              }
                            if (bme680P != bme680Pold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680p = bme680P;
                                        errMQTT = (int8_t) mqtt.publish(topBME680p.c_str(), (uint8_t*) valBME680p.c_str(), valBME680p.length());
                                        soutMQTTerr(topBME680p.c_str(), errMQTT);
                                            //SVAL(topBME680p, valBME680p);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA1";
                                    tmpval16 = (uint16_t) bme680P;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Pold = bme680P;
                              }
                            if (bme680H != bme680Hold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680h = bme680H;
                                        errMQTT = (int8_t) mqtt.publish(topBME680h.c_str(), (uint8_t*) valBME680h.c_str(), valBME680h.length());
                                        soutMQTTerr(topBME680h.c_str(), errMQTT);
                                            //SVAL(topBME680h, valBME680h);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA2";
                                    tmpval16 = (int16_t) bme680H;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Hold = bme680H;
                              }
                            if (bme680G != bme680Gold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valBME680g = bme680G;
                                        errMQTT = (int8_t) mqtt.publish(topBME680g.c_str(), (uint8_t*) valBME680g.c_str(), valBME680g.length());
                                        soutMQTTerr(topBME680g.c_str(), errMQTT);
                                            //SVAL(topBME680g, valBME680g);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA2";
                                    tmpval16 = (int16_t) bme680G;
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                bme680Gold = bme680G;
                              }
                          #endif
                          //outpIdx++;
                      break;
                    case 2:  // CCS811_I2C
                        #if (USE_CCS811_I2C > OFF)
                            if (ccsC != ccsCold)
                              {
                                valCCS811c = ccsC;
                                //pubCCS811c = TRUE;
                                    //SVAL(" 811readC  new ", ccsC);
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        errMQTT = (int8_t) mqtt.publish(topCCS811c.c_str(), (uint8_t*) valCCS811c.c_str(), valCCS811c.length());
                                        soutMQTTerr(topCCS811c.c_str(), errMQTT);
                                            //SVAL(topCCS811c.c_str(), valCCS811c);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                ccsCold    = ccsC;
                              }

                            if (ccsT != ccsTold)
                              {
                                valCCS811t = ccsT;
                                //pubCCS811t = TRUE;
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        errMQTT = (int8_t) mqtt.publish(topCCS811t.c_str(), (uint8_t*) valCCS811t.c_str(), valCCS811t.length());
                                        soutMQTTerr(topCCS811t.c_str(), errMQTT);
                                            //SVAL(topCCS811t.c_str(), valCCS811t);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                ccsTold    = ccsT;
                                    //SVAL(" 811readT  new ", ccsT);
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 3:  // INA3221_I2C 3x U+I measures
                        #if (USE_INA3221_I2C > OFF)
                            // U 3.3V supply
                              if (inaU[0][0] != inaUold[0][0])
                                {
                                  valINA3221u[0][0] = inaU[0][0];
                                      //S2VAL(" incycle 3221 ina[0][0] inaUold[0][0] ", inaUold[0][0], inaU[0][0]);
                                      //SVAL(" U 3.3    new ", inaU[0][0]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211u[0].c_str(),
                                                                           (uint8_t*) valINA3221u[0][0].c_str(),
                                                                          valINA3221u[0][0].length());
                                          soutMQTTerr(topINA32211u[0].c_str(), errMQTT);
                                              //SVAL(topINA32211u[0].c_str(), valINA3221u[0][0]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaUold[0][0]     = inaU[0][0];
                                }
                            // I 3.3V not used
                              #ifdef NOTUSED
                                  if (inaI[0][0] != inaIold[0][0])
                                    {
                                      valINA3221i[0][0] = inaI[0][0];
                                          SVAL(" I 3.3 new ", inaI[0][0]);
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topINA32211i[0].c_str(),
                                                                              (uint8_t*) valINA3221i[0][0].c_str(),
                                                                              valINA3221i[0][0].length());
                                              soutMQTTerr(topINA32211i[0].c_str(), errMQTT);
                                                  SVAL(topINA32211i[0].c_str(), valINA3221i[0][0]);
                                            }
                                        #endif
                                    }
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaIold[0][0]     = inaI[0][0];
                                #endif
                            // P 3.3V not used
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                            // U 5V supply
                              if (inaU[0][1] != inaUold[0][1])
                                {
                                  valINA3221u[0][1] = inaU[0][1];
                                  //pubINA3221u[0][1] = TRUE;
                                      //SVAL(" U 5.0    new ", inaU[0][1]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211u[1].c_str(),
                                                                          (uint8_t*) valINA3221u[0][1].c_str(),
                                                                          valINA3221u[0][1].length());
                                          soutMQTTerr(topINA32211u[1].c_str(), errMQTT);
                                              //SVAL(topINA32211u[1].c_str(), valINA3221u[0][1]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaUold[0][1]     = inaU[0][1];
                                }
                            // I 5V supply
                              if (inaI[0][1] != inaIold[0][1])
                                {
                                  valINA3221i[0][1] = inaI[0][1];
                                  //pubINA3221i[0][1] = TRUE;
                                      //SVAL(" I 5.0    new ", inaI[0][1]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211i[1].c_str(),
                                                                          (uint8_t*) valINA3221i[0][1].c_str(),
                                                                          valINA3221i[0][1].length());
                                          soutMQTTerr(topINA32211i[1].c_str(), errMQTT);
                                              //SVAL(topINA32211i[1].c_str(), valINA3221i[0][1]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaIold[0][1]     = inaI[0][1];
                                }
                            // P 5V supply
                              if (inaP[0][1] != inaPold[0][1])
                                {
                                  valINA3221p[0][1] = inaP[0][1];
                                  //pubINA3221p[0][1] = TRUE;
                                      //SVAL(" P 5.0    new ", inaP[0][1]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211p[1].c_str(),
                                                                          (uint8_t*) valINA3221p[0][1].c_str(),
                                                                          valINA3221p[0][1].length());
                                          soutMQTTerr(topINA32211p[1].c_str(), errMQTT);
                                              //SVAL(topINA32211p[1].c_str(), valINA3221p[0][1]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                }
                              inaPold[0][1]     = inaP[0][1];
                            // U main supply 12V/19V supply
                              if (inaU[0][2] != inaUold[0][2])
                                {
                                  valINA3221u[0][2] = inaU[0][2];
                                  //pubINA3221u[0][2] = TRUE;
                                      //SVAL(" U supply new ", inaU[0][2]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211u[2].c_str(),
                                                                          (uint8_t*) valINA3221u[0][2].c_str(),
                                                                          valINA3221u[0][2].length());
                                          soutMQTTerr(topINA32211u[2].c_str(), errMQTT);
                                              //SVAL(topINA32211u[2].c_str(), valINA3221u[0][2]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaUold[0][2]     = inaU[0][2];
                                }
                            // I main supply 12V/19V supply
                              if (inaI[0][2] != inaIold[0][2])
                                {
                                  valINA3221i[0][2] = inaI[0][2];
                                  //pubINA3221i[0][2] = TRUE;
                                  inaIold[0][2]     = inaI[0][2];
                                      //SVAL(" I supply new ", inaI[0][2]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211i[2].c_str(),
                                                                          (uint8_t*) valINA3221i[0][2].c_str(),
                                                                          valINA3221i[0][2].length());
                                          soutMQTTerr(topINA32211i[2].c_str(), errMQTT);
                                              //SVAL(topINA32211i[2].c_str(), valINA3221i[0][2]);
                                        }
                                    #endif
                                  #if (USE_WEBSERVER > OFF)
                                    #endif
                                  inaIold[0][2]     = inaI[0][2];
                                }
                            // P main supply
                              if (inaP[0][2] != inaPold[0][2])
                                {
                                  valINA3221p[0][2] = inaP[0][2];
                                  //pubINA3221p[0][2] = TRUE;
                                  inaPold[0][2]     = inaP[0][2];
                                      //SVAL(" P supply new ", inaP[0][2]);
                                  #if (USE_MQTT > OFF)
                                      if (errMQTT == MD_OK)
                                        {
                                          errMQTT = (int8_t) mqtt.publish(topINA32211p[2].c_str(),
                                                                          (uint8_t*) valINA3221p[0][2].c_str(),
                                                                          valINA3221p[0][2].length());
                                          soutMQTTerr(topINA32211p[2].c_str(), errMQTT);
                                              //SVAL(topINA32211p[2].c_str(), valINA3221p[0][2]);
                                        }
                                    #endif
                                }
                          #endif
                      //outpIdx++;
                      break;
                    case 4:  // DS18B20_1W temperature
                        #if (USE_DS18B20_1W_IO > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 5:  // MQ135_GAS_ANA
                        #if (USE_MQ135_GAS_ANA > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 6:  // MQ3_ALK_ANA
                        #if (USE_MQ3_ALK_ANA > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 7:  // PHOTO_SENS_ANA
                        #if (USE_PHOTO_SENS_ANA > OFF)
                            if (photof[0] != photofold[0])
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valPhoto[0]  = photof[0];
                                        errMQTT = (int8_t) mqtt.publish(topPhoto1.c_str(), (uint8_t*) valPhoto[0].c_str(), valPhoto[0].length());
                                        soutMQTTerr(topPhoto1.c_str(), errMQTT);
                                            //SVAL(topPhoto1, valPhoto[0]);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                    tmpStr = "SVA3";
                                    tmpval16 = valPhoto[0].toInt();
                                    tmpStr.concat(tmpval16);
                                    pmdServ->updateAll(tmpStr);
                                  #endif
                                photofold[0] = photof[0];
                                    //SVAL(" photo1  new ", photof[0]);
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 8:  // POTI_ANA
                        #if (USE_POTI_ANA > OFF)
                            if (potif[0] != potifold[0])
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valPoti[0]  = potif[0];
                                        errMQTT = (int8_t) mqtt.publish(topPoti1.c_str(),
                                                                        (uint8_t*) valPoti[0].c_str(),
                                                                        valPoti[0].length());
                                        soutMQTTerr(topPoti1.c_str(), errMQTT);
                                            //SVAL(topINA32211u[1].c_str(), valINA3221u[0][1]);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                potifold[0] = potif[0];
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 9:  // VCC50_ANA
                        #if (USE_VCC50_ANA > OFF)
                            if (vcc50f != vcc50fold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valVCC50  = vcc50f;
                                        errMQTT = (int8_t) mqtt.publish(topVCC50.c_str(),
                                                                        (uint8_t*) valVCC50.c_str(),
                                                                        valVCC50.length());
                                        soutMQTTerr(topVCC50.c_str(), errMQTT);
                                            //SVAL(topVCC50.c_str(), valVCC50);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                vcc50fold = vcc50f;
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 10: // VCC33_ANA
                        #if (USE_VCC33_ANA > OFF)
                            if (vcc33f != vcc33fold)
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        valVCC33  = vcc33f;
                                        errMQTT = (int8_t) mqtt.publish(topVCC33.c_str(),
                                                                        (uint8_t*) valVCC33.c_str(),
                                                                        valVCC33.length());
                                        soutMQTTerr(topVCC33.c_str(), errMQTT);
                                            //SVAL(topVCC33.c_str(), valVCC33);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                vcc33fold = vcc33f;
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 11: // ACS712_ANA
                        #if (USE_ACS712_ANA > OFF)
                            if (i712f[0] != i712fold[0])
                              {
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                        vali712[0]  = i712f[0];
                                        errMQTT = (int8_t) mqtt.publish(topi7121.c_str(),
                                                                        (uint8_t*) vali712[0].c_str(),
                                                                        vali712[0].length());
                                        soutMQTTerr(topi7121.c_str(), errMQTT);
                                            //SVAL(topi7121.c_str(), vali712[0]);
                                      }
                                  #endif
                                #if (USE_WEBSERVER > OFF)
                                  #endif
                                i712fold[0] = i712f[0];
                              }
                          #endif
                        outpIdx++;
                      //break;
                    case 12: // TYPE_K_SPI
                        #if (USE_TYPE_K_SPI > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 13: // CNT_INP
                        #if (USE_CNT_INP > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 14: // DIG_INP
                        #if (USE_DIG_INP > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 15: // ESPHALL
                        #if (USE_ESPHALL > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 16: // MCPWM
                        #if (USE_MCPWM > OFF)
                            #if (USE_MQTT > OFF)
                                if (errMQTT == MD_OK)
                                  {
                                  }
                              #endif
                            #if (USE_WEBSERVER > OFF)
                              #endif
                          #endif
                        outpIdx++;
                      //break;
                    case 17: // RGBLED_PWM
                      #if (USE_RGBLED_PWM > OFF)
                          if (rgbledT.TOut())
                            {
                                  //STXT(" # Out RGBLED");
                              rgbledT.startT();
                              #if (TEST_RGBLED_PWM > OFF)
                                /*
                                  switch (colRGBLED)
                                    {
                                      case 0:
                                        if (RGBLED_rt >= 254)
                                          {
                                            RGBLED_rt = 0;
                                            RGBLED_gr += incRGBLED;
                                            colRGBLED++;
                                          }
                                          else
                                          { RGBLED_rt += incRGBLED; }
                                        break;
                                      case 1:
                                        if (RGBLED_gr >= 254)
                                          {
                                            RGBLED_gr = 0;
                                            RGBLED_bl += incRGBLED;
                                            colRGBLED++;
                                          }
                                          else
                                          { RGBLED_gr += incRGBLED; }
                                        break;
                                      case 2:
                                        if (RGBLED_bl >= 254)
                                          {
                                            RGBLED_bl = 0;
                                            RGBLED_rt += incRGBLED;
                                            colRGBLED = 0;
                                          }
                                          else
                                          { RGBLED_bl += incRGBLED; }
                                        break;
                                      default:
                                        break;
                                    }
                                  */

                                  #if (USE_WEBCTRL_RGB > OFF)
                                      _tmp += 4;
                                      if (_tmp > 50)
                                        { _tmp = 0; }
                                          //SVAL(" _tmp = ", _tmp);
                                      ledcWrite(PWM_RGB_RED,   webMD.getDutyCycle(0));
                                      ledcWrite(PWM_RGB_GREEN, webMD.getDutyCycle(1));
                                      ledcWrite(PWM_RGB_BLUE,  webMD.getDutyCycle(2));
                                    #endif


                                  if(*RGBLED[0] == *RGBLED[1]) {}
                                    else
                                    {
                                      SOUT(" RGBLED changed 0/1 "); SOUTHEX((uint32_t) *RGBLED[0]);
                                      SOUT(" / "); SOUTHEXLN((uint32_t) *RGBLED[1]);
                                      *RGBLED[0] = *RGBLED[1];
                                      //ledcWrite(PWM_RGB_RED,   BrightCol(RGBLED[0][LED_RED],RGBLED[0][LED_BRIGHT]));
                                      //ledcWrite(PWM_RGB_GREEN, BrightCol(RGBLED[0][LED_GREEN],RGBLED[0][LED_BRIGHT]));
                                      //ledcWrite(PWM_RGB_BLUE, BrightCol()  BrightCol(RGBLED[0][LED_BLUE],RGBLED[0][LED_BRIGHT]));
                                      ledcWrite(PWM_RGB_RED,   Bright_x_Col(Red(RGBLED[0]->col24()),   RGBLED[0]->bright()));
                                      ledcWrite(PWM_RGB_GREEN, Bright_x_Col(Green(RGBLED[0]->col24()), RGBLED[0]->bright()));
                                      ledcWrite(PWM_RGB_BLUE,  Bright_x_Col(Blue(RGBLED[0]->col24()),  RGBLED[0]->bright()));
                                    }
                                #endif
                                    //S2HEXVAL(" outcycle vor udate RGBLEDold RGBLED ", RGBLEDold->toInt(), RGBLED->toInt());
                              if (RGBLED->toInt() != RGBLEDold->toInt())
                                {
                                  // update HW
                                    LEDout = (uint8_t) map(RGBLED->bright(), 0, 255, 0, Green(RGBLED->col24()));
                                    ledcWrite(PWM_RGB_GREEN, LEDout);
                                    LEDout = (uint8_t) map(RGBLED->bright(), 0, 255, 0, Red(RGBLED->col24()));
                                    ledcWrite(PWM_RGB_RED, LEDout);
                                    LEDout = (uint8_t) map(RGBLED->bright(), 0, 255, 0, Blue(RGBLED->col24()));
                                    ledcWrite(PWM_RGB_BLUE, LEDout);
                                    LEDout = FALSE;
                                  // update brightness
                                    if (RGBLED->bright() != RGBLEDold->bright())
                                      {
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                valRGBBright = (RGBLED->bright());    // RGB-LED col24
                                                    //SVAL(topRGBBright, valRGBBright);
                                                errMQTT = (int8_t) mqtt.publish(topRGBBright.c_str(), (uint8_t*) valRGBBright.c_str(), valRGBBright.length());
                                                    soutMQTTerr(" MQTT publish RGBBright", errMQTT);
                                              }
                                          #endif

                                        #if (USE_WEBSERVER > OFF)
                                            outStr = "SVB1";
                                            outStr.concat(RGBLED->bright());    // RGB-LED col24
                                            pmdServ->updateAll(outStr);
                                              //STXT(outStr);
                                          #endif
                                      }
                                  // update color
                                    if (RGBLED->col24() != RGBLEDold->col24())
                                      {
                                        #if (USE_MQTT > OFF)
                                            colToHexStr(cMQTT, RGBLED->col24());
                                            valRGBCol = cMQTT;    // RGB-LED col24
                                                //SVAL(topRGBCol, valRGBCol);
                                            errMQTT = (int8_t) mqtt.publish(topRGBCol.c_str(), (uint8_t*) valRGBCol.c_str(), valRGBCol.length());
                                                soutMQTTerr(" MQTT publish RGBCol", errMQTT);
                                          #endif

                                        #if (USE_WEBSERVER > OFF)
                                            outStr = "SVC1";
                                            colToHexStr(ctmp8, RGBLED->col24());
                                            outStr.concat(ctmp8);    // RGB-LED col24
                                            pmdServ->updateAll(outStr);
                                                //STXT(outStr);
                                          #endif
                                      }
                                          //S2HEXVAL(" outcycle update RGBLEDold RGBLED ", RGBLEDold->toInt(), RGBLED->toInt());
                                    RGBLEDold->fromInt(RGBLED->toInt());
                                          //S2HEXVAL(" outcycle update RGBLEDold RGBLED ", RGBLEDold->toInt(), RGBLED->toInt());
                                }
                            }
                        #endif
                      break;
                    case 18: // FAN_PWM
                      #if (USE_FAN_PWM > OFF)
                          if (fanT.TOut())
                            {
                                  //STXT(" # Out FAN");
                              fanT.startT();
                              if (fanIdx++ > 1000)
                                {
                                  fanIdx = 0;
                                  for (uint8_t i=0 ; i < USE_FAN_PWM ; i++)
                                    {
                                      valFanPWM[i] += 1;
                                      if (valFanPWM[i] >= 255) { valFanPWM[i] = 0; } // -50%
                                    }
                                  #if (USE_POTICTRL_FAN > 0)
                                      valFan[INP_CNT_FAN_1] = map((long) -inpValADC[INP_POTI_CTRL], -4095, 0, 0, 255);
                                          //SVAL(" fan poti ", inpValADC[INP_POTI_CTRL]);
                                          //SVAL(" fan cnt ", valFan[INP_CNT_FAN_1]);
                                      valFanPWM[0] = valFan[INP_CNT_FAN_1];
                                      #if (USE_POTICTRL_FAN > 1)
                                          valFan[INP_CNT_FAN_2] = map((long) -inpValADC[INP_POTI_CTRL], -4095, 0, 0, 255);
                                          valFanPWM[1] = valFan[INP_CNT_FAN_2];
                                        #endif
                                    #endif
                                  ledcWrite(PWM_FAN_1, valFanPWM[0]);
                                  #if (USE_FAN_PWM > 1)
                                      ledcWrite(PWM_FAN_2, valFanPWM[1]);
                                    #endif
                                }
                              #if (USE_MQTT > OFF)
                                  if (errMQTT == MD_OK)
                                    {
                                    }
                                #endif
                              #if (USE_WEBSERVER > OFF)
                                #endif
                            }
                        #endif
                      break;
                    case 19: // digital output
                      #if (USE_GEN_DIG_OUT > OFF)
                          if (testLED != testLEDold)
                            {
                              valtestLED = testLED;
                              #if (DIG_OUT1_INV > OFF)
                                  digitalWrite(PIN_GEN_DIG_OUT1, !testLED);
                              #else
                                  digitalWrite(PIN_GEN_DIG_OUT1, testLED);
                                #endif
                              #if (USE_MQTT)
                                  if (errMQTT == MD_OK)
                                    {
                                      errMQTT = (int8_t) mqtt.publish(toptestLED.c_str(), (uint8_t*) valtestLED.c_str(),  valtestLED.length());
                                          soutMQTTerr(" MQTT subscribe testLED", errMQTT);
                                    }
                                #endif
                              #if (USE_WEBSERVER > OFF)
                                #endif
                              testLEDold = testLED;
                            }
                        #endif
                    case 20: // WEBSERVER
                      #if (USE_WEBSERVER > OFF)
                          if (newClient)
                            {
                              // EL_TSLIDER
                              #if (USE_WS2812_LINE_OUT > OFF)
                                  outStr = "SVB2";
                                  outStr.concat(line2812[0]->bright());    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);
                                #endif
                              #if (USE_WS2812_MATRIX_OUT > OFF)
                                  outStr = "SVB3";
                                  md_LEDPix24* ppix = outM2812[0].text->pix24;
                                  outStr.concat(ppix->bright());           // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);                              outStr = "SVB3";
                                #endif
                                  //tmpStr = "SVB4";
                                  //tmpStr.concat(line2812[0]->bright());    // RGB-LED col24
                                  //pmdServ->updateAll(tmpStr);

                              // EL_TCOLOR
                              #if (USE_WS2812_LINE_OUT > OFF)
                                  outStr = "SVC2";
                                  colToHexStr(ctmp, line2812[0]->col24());
                                  outStr.concat(ctmp);    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  //STXT(outStr);
                                #endif
                              #if (USE_WS2812_MATRIX_OUT > OFF)
                                  outStr = "SVC3";
                                  ppix = outM2812[0].text->pix24;
                                  colToHexStr(ctmp, ppix->col24());
                                  outStr.concat(ctmp);    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);
                                  outStr = "SVC4";
                                  ppix = outM2812[0].bmpB->pix24;
                                  colToHexStr(ctmp, ppix->col24());
                                  outStr.concat(ctmp);    // RGB-LED col24
                                  pmdServ->updateAll(outStr);
                                  STXT(outStr);
                                #endif

                              newClient = false;
                            }
                        #endif
                      break;
                    default:
                      outpIdx = 0;
                      break;
                  }
              }
          #endif
          #endif // defined(DISP)
      // system loop end
        if (firstrun == true)
          {
            String taskMessage = "loop task running on core ";
            taskMessage = taskMessage + xPortGetCoreID();
            STXT(taskMessage);
            usLast = micros();
            firstrun = false;
          }
        anzUsCycles++;
        sleep(1);
          //usleep(20);
    }
// ----------------------------------------------------------------
// --- subroutine and drivers ----------------
// ----------------------------------------------------------------
  // --- system --------------------------
    // --- heap output
      void heapFree(const char* text)
        {
          uint32_t tmp32 = ESP.getFreeHeap();
          //uint32_t tmp32 = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT);
          SVAL(text, tmp32);
        }
  // --- user output ---------------------
    // --- display
      #ifdef USE_DISP
          void clearDisp()
            {
              #if (USE_OLED_I2C > OFF)
                  oled.clear();
                #endif
              // #if (tft)
            }
          void dispStatus(String msg, bool direct)
            {
              #ifdef USE_STATUS
                  size_t statLen = msg.length();
                  bool   doIt    = false;
                  bool   info    = false;

                  if (statLen)
                    {
                      if ( statLen > DISP_MAXCOLS)
                        {
                          msg.remove(DISP_MAXCOLS);
                        }
                      statOn = true;
                      statT.startT();
                      doIt = true;    // output statOut
                      statT.startT();
                    }
                  else // empty input
                    {
                      if (statOn && statT.TOut())
                        statOn = false;
                    }
                  if (!statOn) // disp def val and actual time
                    {
                      if (statN.TOut())
                        {
                          statN.startT();
                          #if (USE_NTP_SERVER > OFF)
                              sprintf(statOut,"%02d.%02d. %02d:%02d:%02d ", day(), month(), hour(), minute(), second());
                              msg = statOut;
                              msg.concat(" ");
                              msg.concat((unsigned long) usPerCycle);
                              msg.concat("us");
                              info = true;
                              doIt = true;
                            #endif
                        }
                    }
                  if (doIt)
                    {
                      #if (USE_TOUCHSCREEN > OFF)
                          touch.wrStatus(msg);
                        #endif
                      #if defined (USE_OLED_I2C)
                          oled.wrStatus(msg);
                        #endif
                      #if (USE_TFT > 0)
                          #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                              mlcd.wrStatus((char*) statOut);
                            #endif
                                  //#if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
                                       //if (info)
                                       //  {
                                       //    #if ( USE_BME280_I2C > OFF )
                                       //        outStr[0] = 0;
                                       //        outStr.concat(bme280T.getVal());
                                       //        outStr.concat("° ");
                                       //        outStr.concat(bme280H.getVal());
                                       //        outStr.concat("% ");
                                       //        outStr.concat(bme280P.getVal());
                                       //        outStr.concat("mb  ");
                                       //      #endif
                                       //  }
                                       // outStr.concat((char*) statOut);
                                       // if (info)
                                       // {
                                       //   #if (USE_WEBSERVER > OFF)
                                       //       outStr.concat(" ");
                                       //       outStr.concat(WiFi.localIP().toString());
                                       //     #endif
                                       // }
                                      // #endif
                        #endif // USE_DISP
                      info = false;
                    }
                #endif // USE_STATUS
            }
          void dispStatus(const char* msg, bool direct)
            {
              dispStatus((String) msg);
            }
          void dispText(String msg, uint8_t col, uint8_t row, uint8_t len)
            {
              #if (USE_OLED_I2C > OFF)
                  oled.wrText(msg, col, row, len);
                #endif
              #if (USE_TFT > 0)
                  #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                      mlcd.wrText(msg, row, col);
                    #endif
                  #if (USE_TOUCHSCREEN > OFF)
                      touch.wrText(msg, col, row, len);
                    #endif
                #endif
            }
          void dispText(char* msg, uint8_t col, uint8_t row, uint8_t len)
            {
              dispText((String) msg, col, row, len);
                /*
                    #if (USE_DISP > 0)
                        #if (USE_OLED1_I2C > OFF)
                            oled1.wrText(msg, col, row, len);
                          #endif
                        #if defined(OLED2)
                            oled2.wrText(msg, col, row, len);
                          #endif
                        #if (USE_TFT > 0)
                            #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                                mlcd.wrText(msg, row, col);
                              #endif
                            #if !(DISP_TFT ^ MC_UO_TOUCHXPT2046_AZ)
                                touch.wrText(msg, col, row, len);
                              #endif
                          #endif
                        #if (USE_TOUCHSCREEN > OFF)
                          #endif
                      #endif
                  */
            }
          void startDisp()
            {
              String _str = "";
              #if defined (USE_OLED_I2C)
                  SOUT(" ... init OLED ...");
                  tmp_i8 = oled.init();
                  SVAL(" init returns ",tmp_i8);
                  #if (OLED_FLIP_VERTICAL > OFF)
                      oled.flipScreenVertically();
                    #endif
                  oled.setContrast(255);
                  oled.clear();
                  S2VAL(" ... OLED ... wide high ", oled.getWidth(), oled.getHeight());
                  oled.setFont(ArialMT_Plain_10);
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                  oled.drawString(0, oled.getHeight() - 10, "Hallo OLED"); oled.display();
                  STXT(" ... OLED is init");
                  #ifdef TEST_OLED
                      test_oled();
                    #endif
                #endif // USE_OLED_I2C
              #ifdef USE_STATUS
                statOut[DISP_MAXCOLS] = 0;  // limit strlen
                #endif
              #ifdef USE_TFT
                  #if !(DISP_TFT ^ MC_UO_TFT1602_GPIO_RO)
                      mlcd.start(plcd);
                    #endif
                  #if (USE_TOUCHSCREEN_SPI > OFF)
                      touch.start(DISP_ORIENT, DISP_BCOL);
                          #if (DEBUG_MODE >= CFG_DEBUG_DETAILS)
                            STXT(" startTouch ");
                          #endif
                    #endif
                  #ifdef USE_TFT_GC9A01
                      tft.begin();
                      tft.setRotation(1);
                      tft.fillScreen(TFT_BLACK);
                      tft.setFreeFont(&FreeMono9pt7b);
                    #endif
                #endif
            }
          #if (PROJECT == PRJ_TEST_LIB_OLED)
              void drawLines()
                {
                  oled.clear();
                  oled.setColor(WHITE);
                  dispStatus("drawlines"); // 21 letters
                  for (int16_t i = 0; i < oled.getWidth(); i += 4)
                    {
                      oled.drawLine(0, 0, i, oled.getHeight() - 11);
                      oled.display();
                      usleep(10000);
                    }
                  for (int16_t i = 0; i < oled.getHeight()-10; i += 4)
                    {
                      oled.drawLine(0, 0, oled.getWidth() - 1, i);
                      oled.display();
                      usleep(10000);
                    }
                  usleep(150000); //sleep(2);
                  oled.clearUser();
                  //oled.fillRect(0, 0, oled.getWidth(), oled.getHeight()-10);
                  for (int16_t i = 0; i < oled.getWidth(); i += 4)
                    {
                      oled.drawLine(0, oled.getHeight() - 11, i, 0);
                      oled.display();
                      usleep(10000);
                    }
                  for (int16_t i = oled.getHeight() - 11; i >= 0; i -= 4)
                    {
                      oled.drawLine(0, oled.getHeight() - 11, oled.getWidth() - 1, i);
                      oled.display();
                      usleep(10000);
                    }
                  usleep(150000); //sleep(2);
                  oled.clearUser();
                  //oled.fillRect(0, 0, oled.getWidth(), oled.getHeight()-10);
                  for (int16_t i = oled.getWidth() - 1; i >= 0; i -= 4)
                    {
                      oled.drawLine(oled.getWidth() - 1, oled.getHeight() - 11, i, 0);
                      oled.display();
                      usleep(10000);
                    }
                  for (int16_t i = oled.getHeight() - 11; i >= 0; i -= 4)
                    {
                      oled.drawLine(oled.getWidth() - 1, oled.getHeight() - 11, 0, i);
                      oled.display();
                      usleep(10000);
                    }
                  usleep(150000); //sleep(2);
                  oled.clearUser();
                  for (int16_t i = 0; i < oled.getHeight()-10; i += 4)
                    {
                      oled.drawLine(oled.getWidth() - 1, 0, 0, i);
                      oled.display();
                      delay(10);
                    }
                  for (int16_t i = 0; i < oled.getWidth(); i += 4)
                    {
                      oled.drawLine(oled.getWidth() - 1, 0, i, oled.getHeight() - 11);
                      oled.display();
                      delay(10);
                    }
                  usleep(150000); //sleep(2);
                }
              // Adapted from Adafruit_SSD1306
              void drawRect(void)
                {
                  oled.clear();
                  dispStatus("drawRect"); // 21 letters
                  for (int16_t i = 0; i < (oled.getHeight() -10) / 2; i += 2)
                    {
                      oled.drawRect(i, i, oled.getWidth() - 2 * i, (oled.getHeight() -10) - 2 * i);
                      oled.display();
                      usleep(50000);
                    }
                  usleep(100000); //sleep(2);
                }
              // Adapted from Adafruit_SSD1306
              void fillRect(void)
                {
                  oled.clear();
                  dispStatus("fillRect"); // 21 letters
                  uint8_t color = 1;
                  for (int16_t i = 0; i < (oled.getHeight() - 10) / 2; i += 3)
                    {
                      oled.setColor((color % 2 == 0) ? BLACK : WHITE); // alternate colors
                      oled.fillRect(i, i, oled.getWidth() - i * 2, (oled.getHeight() - 10) - i * 2);
                      oled.display();
                      usleep(100000);
                      color++;
                    }
                  // Reset back to WHITE
                  oled.setColor(WHITE);
                  usleep(100000);
                }
              // Adapted from Adafruit_SSD1306
              void drawCircle(void)
                {
                  oled.clear();
                  dispStatus("drawCircle"); // 21 letters
                  for (int16_t i = 0; i < (oled.getHeight() - 10); i += 3)
                    {
                      oled.drawCircle(oled.getWidth() / 2, (oled.getHeight() - 10) / 2, i);
                      oled.display();
                      usleep(70000);
                   }
                  usleep(50000);
                  oled.clear();
                  dispStatus("drawCircleQads"); // 21 letters
                  //  This will draw the part of the circel in quadrant 1
                   // Quadrants are numberd like this:
                   //   0010 | 0001
                   //  ------|-----
                   //   0100 | 1000
                   //
                  oled.drawCircleQuads(oled.getWidth() / 2, (oled.getHeight() - 10) / 2, (oled.getHeight() - 10) / 4, 0b00000001);
                  oled.display();
                  usleep(50000);
                  oled.drawCircleQuads(oled.getWidth() / 2, (oled.getHeight() - 10) / 2, (oled.getHeight() - 10) / 3.3, 0b00000011);
                  oled.display();
                  usleep(50000);
                  oled.drawCircleQuads(oled.getWidth() / 2, (oled.getHeight() - 10) / 2, (oled.getHeight() - 10) / 2.6, 0b00000110);
                  oled.display();
                  usleep(50000);
                  oled.drawCircleQuads(oled.getWidth() / 2, (oled.getHeight() - 10) / 2, (oled.getHeight() - 10) / 2, 0b00001100);
                  oled.display();
                  usleep(50000);
                }
              void printBuffer(void)
                {
                  oled.clear();
                  dispStatus("printBuffer"); // 21 letters
                  // Initialize the log buffer
                  // allocate memory to store 8 lines of text and 30 chars per line.
                  oled.setLogBuffer(5, 30);
                  // Some test data
                  const char* test[] =
                    {
                      "Hello",
                      "World" ,
                      "----",
                      "Show off",
                      "how",
                      "the log buffer",
                      "is",
                      "working.",
                      "Even",
                      "scrolling is",
                      "working"
                    };
                  for (uint8_t i = 0; i < 11; i++)
                    {
                      oled.clearUser();
                      // Print to the screen
                      oled.println(test[i]);
                      // Draw it to the internal screen buffer
                      oled.drawLogBuffer(0, 0);
                      // Display it on the screen
                      oled.display();
                      usleep(200000);
                    }
                }
              void drawFontFaceDemo()
                {
                  // Font Demo1
                  // create more fonts at http://oleddisplay.squix.ch/
                  oled.clear();
                  dispStatus("drawFontFace"); // 21 letters
                  //STXT(" drawFontFaceDemo ");
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                  oled.setFont(ArialMT_Plain_10);
                  oled.drawString(0, 0, "Hello world");  oled.display();
                  usleep(50000);
                  oled.setFont(ArialMT_Plain_16);
                  oled.drawString(0, 10, "Hello world"); oled.display();
                  usleep(50000);
                  oled.setFont(ArialMT_Plain_24);
                  oled.drawString(0, 26, "Hello world"); oled.display();
                  usleep(250000);
                }
              void drawTextFlowDemo()
                {
                  oled.clear();
                  dispStatus("drawTextFlow"); // 21 letters
                  oled.setFont(ArialMT_Plain_10);
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                  oled.drawStringMaxWidth(0, 0, oled.getHeight() - 10,
                                             "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
                  oled.display();
                  usleep(400000);
                }
              void drawTextAlignmentDemo()
                {
                  oled.clear();
                  dispStatus("drawTextAlignment"); // 21 letters
                  // Text alignment demo
                  oled.setFont(ArialMT_Plain_10);
                  // The coordinates define the left starting point of the text
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                  oled.drawString(0, 0, "Left aligned (0,00)");      oled.display();
                  usleep(20000); sleep(1);
                  // The coordinates define the center of the text
                  oled.setTextAlignment(TEXT_ALIGN_CENTER);
                  oled.drawString(64, 11, "Center aligned (64,11)");  oled.display();
                  usleep(20000); sleep(1);
                 // The coordinates define the right end of the text
                  oled.setTextAlignment(TEXT_ALIGN_RIGHT);
                  oled.drawString(128, 22, "Right aligned (128,22)"); oled.display();
                  usleep(20000); sleep(1);
                  // The coordinates define the center of the text
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                  oled.drawString(0, 33, "1  Center (64,33)  21");  oled.display();
                  usleep(20000); sleep(1);
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                  oled.drawString(0, 44, "L (0,0)");
                  oled.setTextAlignment(TEXT_ALIGN_RIGHT);
                  oled.drawString(128, 44, "R (128,22)"); oled.display();
                  usleep(50000); sleep(1);
                }
              void drawRectDemo()
                {
                  oled.clear();
                  dispStatus("drawRect"); // 21 letters
                  // Draw a pixel at given position
                  for (int i = 0; i < 10; i++)
                    {
                      oled.setPixel(i, i);
                      oled.setPixel(10 - i, i);  oled.display();
                    }
                  usleep(200000);
                  oled.drawRect(12, 12, 20, 20);  oled.display();
                  usleep(200000);
                  // Fill the rectangle
                  oled.fillRect(14, 14, 17, 17);  oled.display();
                  usleep(200000);
                  // Draw a line horizontally
                  oled.drawHorizontalLine(0, 40, 20);  oled.display();
                  usleep(200000);
                  // Draw a line horizontally
                  oled.drawVerticalLine(40, 0, 20);  oled.display();
                  usleep(200000);
                }
              void drawCircleDemo()
                {
                  oled.clear();
                  dispStatus("drawCircle"); // 21 letters
                  for (int i = 1; i < 8; i++)
                    {
                      oled.setColor(WHITE);
                      oled.drawCircle(32, 32, i * 3);      oled.display();
                      if (i % 2 == 0)
                        {
                          oled.setColor(BLACK);
                        }
                      oled.fillCircle(96, 32, 32 - i * 3); oled.display();
                    }
                }
              void drawProgressBarDemo()
                {
                  oled.clear();
                  dispStatus("drawProgressBar"); // 21 letters
                  uint8_t progress = 0;
                  uint8_t lin0     = oled.getHeight() - 20;
                  uint8_t widthbar = oled.getWidth()  - 32;
                  oled.setTextAlignment(TEXT_ALIGN_LEFT);
                    //usleep(500000);
                  while (progress <= 100)
                    {
                      oled.clearLine(1, oled.getRows()-1, 3);
                        //usleep(500000);
                      oled.drawProgressBar(30, oled.getRowY(oled.getRows()-1) + 3, widthbar, 6 , progress); //oled.display();
                      outStr = String(progress) + "%";
                      oled.wrText(outStr, 1, oled.getRows()-1, 3); oled.display();
                      outStr = "";
                      progress+=20;
                      usleep(50000);
                    }
                  usleep(50000);
                }
              void writeTextDemo()
                {
                  dispStatus("writeText");
                  for (uint8_t i = 1; i < oled.getRows(); i++)
                    {
                      sprintf(tmp_c32, "Zeile%i",i);
                      oled.wrText(tmp_c32, i, i);
                    }
                  usleep(50000);
                }
              //void drawImageDemo()
                //{
                //  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
                //  // on how to create xbm files
                //  oled.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
                //}
            #endif // PRJ_TEST_LIB_OLED
          #if (PROJECT == PRJ_TEST_LIB_TFT)
              void draw_Julia(float c_r, float c_i, float zoom)
                {

                  //tft.setcursor(0,0);
                  float new_r = 0.0, new_i = 0.0, old_r = 0.0, old_i = 0.0;

                  /* Pour chaque pixel en X */

                  for(int16_t x = SCREEN_WIDTH/2 - 1; x >= 0; x--) { // Rely on inverted symmetry
                    /* Pour chaque pixel en Y */
                    for(uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
                      old_r = 1.5 * (x - SCREEN_WIDTH / 2) / (0.5 * zoom * SCREEN_WIDTH);
                      old_i = (y - SCREEN_HEIGHT / 2) / (0.5 * zoom * SCREEN_HEIGHT);
                      uint16_t i = 0;
                      while ((old_r * old_r + old_i * old_i) < 4.0 && i < MAX_ITERATION) {
                        new_r = old_r * old_r - old_i * old_i ;
                        new_i = 2.0 * old_r * old_i;

                        old_r = new_r+c_r;
                        old_i = new_i+c_i;

                        i++;
                      }
                      /* Affiche le pixel */
                      if (i < 100){
                        tft.drawPixel(x,y,tft.color565(255,255,map(i,0,100,255,0)));
                        tft.drawPixel(SCREEN_WIDTH - x - 1,SCREEN_HEIGHT - y - 1,tft.color565(255,255,map(i,0,100,255,0)));
                      }if(i<200){
                        tft.drawPixel(x,y,tft.color565(255,map(i,100,200,255,0),0));
                        tft.drawPixel(SCREEN_WIDTH - x - 1,SCREEN_HEIGHT - y - 1,tft.color565(255,map(i,100,200,255,0),0));
                      }else{
                        tft.drawPixel(x,y,tft.color565(map(i,200,300,255,0),0,0));
                        tft.drawPixel(SCREEN_WIDTH - x - 1,SCREEN_HEIGHT - y - 1,tft.color565(map(i,200,300,255,0),0,0));
                      }
                    }
                  }
                }
            #endif
        #endif // USE_DISP
  // --- sensors -------------------------
    // --- BME280
      #if (USE_BME280_I2C > OFF)
          static void initBME280()
            {
              dispStatus("init BME280");
              STXT(" init BME280 ...");
              bme280da = bme280_1.begin(I2C_BME280_76, pbme280i2c);
              if (bme280da)
                {
                  bme280_1.setSampling(bme280_1.MODE_FORCED);
                  STXT(" BME280(1) gefunden");
                  #if (BME280T_FILT > OFF)
                      bme280TVal.begin(BME280T_FILT, BME280T_Drop);
                    #endif
                  #if (BME280P_FILT > OFF)
                      bme280PVal.begin(BME280P_FILT, BME280P_Drop);
                    #endif
                  #if (BME280H_FILT > OFF)
                      bme280HVal.begin(BME280H_FILT, BME280H_Drop);
                    #endif
                }
                else
                {
                  STXT(" BME280(1) nicht gefunden");
                }
            }
        #endif
      #if (USE_INA3221_I2C > OFF)
          void initINA3221()
            {
              dispStatus("init INA3221");
              STX(" init INA3221 ID "); SOUTHEXLN((ina3221.getManufID()));
              ina3221.begin();
              for (uint8_t i = 0; i < 3 ; i++ )
                {
                  pubINA3221i[0][i] = OFF;
                  pubINA3221u[0][i] = OFF;
                }
              // channel 1
                #if (INA3221U1_FILT > OFF)
                    inaUVal[0][0].begin(INA3221U1_FILT, INA3221U1_DROP);
                  #endif
                #if (INA3221I1_FILT > OFF)
                    inaIVal[0][0].begin(INA3221I1_FILT, INA3221I1_DROP);
                  #endif
              // channel 2
                #if (INA3221U2_FILT > OFF)
                    inaUVal[0][1].begin(INA3221U2_FILT, INA3221U2_DROP);
                  #endif
                #if (INA3221I2_FILT > OFF)
                    inaIVal[0][1].begin(INA3221I2_FILT, INA3221I2_DROP);
                  #endif
              // channel 3
                #if (INA3221U3_FILT > OFF)
                   inaUVal[0][2].begin(INA3221U3_FILT, INA3221U3_DROP);
                  #endif
                #if (INA3221I3_FILT > OFF)
                    inaIVal[0][2].begin(INA3221I3_FILT, INA3221I3_DROP);
                  #endif
            }
        #endif

#endif // ESP32_TEST_MD_LIB

