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
    static char     cmsg[MSG_MAXLEN+1] = "";
    static char     ctmp30[33];
    static String   tmpStr;
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
      #ifdef USE_FRAM_I2C
          md_FRAM fram = md_FRAM();
        #endif // USE_FRAM_I2C
    // network
      #ifdef USE_WIFI
          md_wifi wifi  = md_wifi();
          msTimer wifiT = msTimer(WIFI_CONN_CYCLE);
          #if (USE_LOCAL_IP > OFF)
            #endif // USE_LOCAL_IP
          #if (USE_NTP_SERVER > OFF)
              msTimer ntpT    = msTimer(NTPSERVER_CYCLE);
              time_t  ntpTime = 0;
              //bool    ntpGet  = true;
              uint8_t ntpOk   = FALSE;
            #endif // USE_WEBSERVER
        #endif
      #if defined(USE_WEBSERVER)
          #if (TEST_SOCKET_SERVER > OFF)
            /*
              const char index_html[] PROGMEM = R"rawliteral(
              <!DOCTYPE html>
              <html>
              <head>
                <meta name="viewport" content="width=device-width, initial-scale=1">  <title>ESP32 Websocket</title>
                <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js">
                <script src="https://cdn.jsdelivr.net/npm/spectrum-colorpicker2/dist/spectrum.min.js"></script>

                <link rel="stylesheet" type="text/css" href="https://cdn.jsdelivr.net/npm/spectrum-colorpicker2/dist/spectrum.min.css">
                <script language="javascript">

                  window.alert(location.host);
                  var gwUrl = "ws://" + location.host + "/ws";
                  var webSocket = new WebSocket(gwUrl);
                  webSocket.onopen = function(e) {
                      console.log("open");
                  }
                  webSocket.onclose = function(e) {
                      console.log("close");
                  }

                 webSocket.onmessage = function(e) {
                      console.log("message");
                  }
                  function handleColor() {
                    var val = document.getElementById('type-color-on-page').value;
                    webSocket.send(val.substring(1));
                  }
                </script>

                <style>
                  h2 {background: #3285DC;
                      color: #FFFFFF;
                      align:center;
                  }

                  .content {
                      border: 1px solid #164372;
                      padding: 5px;
                  }

                  .button {
                     background-color: #00b300;
                     border: none;
                     color: white;
                     padding: 8px 10px;
                     text-align: center;
                     text-decoration: none;
                     display: inline-block;
                     font-size: 14px;
                }
                </style>
              </head>
              <body>
                <h2>ESP32 Websocket</h2>
                <div class="content">
                <p>Pick a color</p>
                <div id="qunit"></div>

                <input type="color" id="type-color-on-page"  />
                 <p>
                   <input type="button" class="button" value="Send to ESP32" id="btn" onclick="handleColor()" />
                 </p>

                </div>
              </body>
              </html>
              )rawliteral";

              // Web server running on port 80
              AsyncWebServer serv(80);
              // Web socket
              AsyncWebSocket socket("/ws");
            */
          #else
              md_server*   pmdServ   = new md_server();
              static bool  newClient = false;
            #endif
          static msTimer   servT  = msTimer(WEBSERVER_CYCLE);
          static uint8_t   pubWeb = TRUE;
          static uint8_t   webOn  = OFF;
        #endif // USE_WEBSERVER
      #ifdef USE_MQTT
          const char cerrMQTT[10][20]  =
            {
              "success",          "UnknownError",
              "TimedOut",         "AlreadyConnected",
              "BadParameter",     "BadProperties",
              "NetworkError",     "NotConnected",
              "TranscientPacket", "WaitingForResult"
            };
          const  String     mqttID       = MQTT_DEVICE;
          const  String     topDevice    = MQTT_TOPDEV;
          static char       cMQTT[20]    = "";
          static String     tmpMQTT      = "";
          static MQTTmsg_t  MQTTmsgs[MQTT_MSG_MAXANZ];
          static MQTTmsg_t* pMQTTWr      = &MQTTmsgs[0];
          static MQTTmsg_t* pMQTTRd      = &MQTTmsgs[0];
          static uint8_t    anzMQTTmsg   = 0;
          static uint8_t    pubMQTT      = TRUE;
          static int8_t     errMQTT      = 0;
          struct MessageReceiver : public Network::Client::MessageReceived
            {
              void messageReceived(const Network::Client::MQTTv5::DynamicStringView & topic,
                                   const Network::Client::MQTTv5::DynamicBinDataView & payload,
                                   const uint16 packetIdentifier,
                                   const Network::Client::MQTTv5::PropertiesView & properties)
                {
                  fprintf(stdout, "  Topic: %.*s ", topic.length, topic.data);
                  fprintf(stdout, "  Payload: %.*s\n", payload.length, payload.data);
                  if (anzMQTTmsg < (MQTT_MSG_MAXANZ - 1))
                    {
                      sprintf(pMQTTWr->topic,   "%.*s", topic.length,   topic.data);
                      sprintf(pMQTTWr->payload, "%.*s", payload.length, payload.data);
                      pMQTTWr->topic[topic.length] = 0;
                      pMQTTWr->payload[payload.length] = 0;
                      anzMQTTmsg++;
                          S3VAL(" topic payload count", pMQTTWr->topic, pMQTTWr->payload, anzMQTTmsg);
                      pMQTTWr = (MQTTmsg_t*) pMQTTWr->pNext;
                    }
                  //fprintf(stdout, "Msg received: (%04X)\n", packetIdentifier);
                  //readMQTTmsg(topic, payload);
                }
            };
          MessageReceiver msgHdl;
          Network::Client::MQTTv5 mqtt(mqttID.c_str(), &msgHdl);
        #endif
    // sensors
      #ifdef USE_CCS811_I2C
          #define CCS811_I2C_ADDR     I2C_CSS811_
          #define CCS811_I2C          I2C1
        #endif // USE_CCS811_I2C
      #ifdef USE_BME680_I2C
          #define BME680_I2C         I2C1
        #endif // USE_BME680_I2C
      #if defined(USE_BME280_I2C)
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
      #if defined(USE_INA3221_I2C)
          SDL_Arduino_INA3221 ina3221(INA3221_ADDR); // def 0.1 ohm
          #if (INA3221_I2C == DEV_I2C1)
              TwoWire* ina1i2c = &i2c1;
            #else
              TwoWire* ina1i2c = &i2c2;
            #endif // INA3221_I2C
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
              static String topina3221i[3] = { MQTT_ina3221I1, MQTT_ina3221I2, MQTT_ina3221I3 };
              static String topina3221u[3] = { MQTT_ina3221U1, MQTT_ina3221U2, MQTT_ina3221U3 };
              static String topina3221p[3] = { MQTT_ina3221P1, MQTT_ina3221P2, MQTT_ina3221P3 };
            #endif
        #endif // USE_INA3221_I2C
      #if defined(USE_PZEM017_RS485)
          #if (PZEM_SER == DEV_SER2)
              HardwareSerial& portser = Serial2;
            #else
              HardwareSerial& portser = Serial1;
            #endif
          uint8_t           pzemIdx  = 0;
          RS485_SlaveData_t pzemData[NUM_PZEMS];
          //md_PZEM017        pzems  = md_PZEM017(portser, &pzemData[0], NUM_PZEMS, SERIAL_8N1,
          //                                      PIN_PZEM1_RTS, PIN_COMM2_RX, PIN_COMM2_TX) ; //, portser };
          md_PZEM017        pzems  = md_PZEM017(portser, SERIAL_8N2,
                                                PIN_PZEM1_RTS, PIN_COMM2_RX, PIN_COMM2_TX) ; //, portser };
          //uint8_t           pzemAddr[NUM_PZEMS];
          //float             pzemU[NUM_PZEMS];
          //float             pzemI[NUM_PZEMS];
          //float             pzemP[NUM_PZEMS];
          //float             pzemE[NUM_PZEMS];
          static String     valpzemU[NUM_PZEMS];
          static String     valpzemI[NUM_PZEMS];
          static String     valpzemP[NUM_PZEMS];
          static int8_t     pubpzemE[NUM_PZEMS];
        #endif // USE_PZEM017_RS485
// -------------------------------- --------------------------------
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
      // start WIFI
        #if defined(USE_WIFI)
            uint8_t rep = WIFI_ANZ_LOGIN;
            while(rep > 0)
              {
                    //STXT(" setup   Start WiFi ");
                tmp_i8 = startWIFI(true);
                if (tmp_i8 == MD_OK)
                    {
                      dispStatus("WIFI connected",true);
                      break;
                    }
                  else
                    {
                      #if (WIFI_IS_DUTY > OFF)
                          dispStatus("WIFI error -> halted", true);
                      #else
                          rep--;
                          if (rep > 0)
                            { dispStatus("WIFI error ..."); }
                          else
                            { dispStatus("WIFI not connected"); }
                        #endif
                    }
                //usleep(50000);
              }
          #endif // USE_WIFI
      // start Webserer
        #if defined(USE_WEBSERVER)
            {
              servT.startT();
              #if (TEST_SOCKET_SERVER > OFF)
                  //socket.onEvent(onEvent);
                  //serv.addHandler(&socket);

                  //serv.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  //  {
                  //    request->send_P(200, "text/html", index_html, NULL);
                  //  });

                  // serv.begin();
              #else
                  startWebServer();
                #endif
            }
          #endif
      // start MQTT
        #if (USE_MQTT > OFF)
            startMQTT();
          #endif
      // BME280 temperature, pessure, humidity
        #if defined(USE_BME280_I2C)
            initBME280();
          #endif
      // temp. current sensor INA3221
        #if (USE_INA3221_I2C > OFF)
            initINA3221();
          #endif
      // DC energy measurement with PZEM003/PZEM017
        #if defined(USE_PZEM017_RS485)
            #if defined(PZEM_LIST_ADDR)
                listPZEMaddr();
              #endif
            initPZEM017();
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
      // --- network ---
        #if defined(USE_NTP_SERVER)   // get time from NTP server
            if (ntpT.TOut() == true)
              {
                setTime(++ntpTime);
                  //SVAL("loop NTP time ", ntpTime);
                if (WiFi.status() == WL_CONNECTED)
                  { // WiFi online
                      //SVAL("WIFI online ntpOk ", ntpOk);
                    if (!ntpOk)
                      {
                        // STXT("InitNTPTime ");
                        initNTPTime();
                        tmp_i8 = TRUE;
                        if (tmp_i8)
                          {
                            tmp_i8 = wifi.getNTPTime(&ntpTime);
                            if (tmp_i8 == WIFI_OK)
                              {
                                setTime(ntpTime);
                                STXT(" NTP time syncronized");
                                ntpOk = TRUE;
                              }
                          }
                      }
                  }
                ntpT.startT();
              }
          #endif // USE_NTP_SERVER
      // --- trigger measurement ---
        #if (USE_ADC1115_I2C > OFF)
            md_ADS1115_run();
          #endif
      // --- direct input ---
        #if (USE_CNT_INP > OFF)
            uint64_t        lim  = 0ul;
            pcnt_evt_type_t ev;
            uint8_t         doIt = false;
            pcnt_unit_t     unit;
            sprintf(cmsg,"  loop/cnt_inp");
            for ( uint8_t i = 0; i < USE_CNT_INP ; i++ )
              {
                switch (i)
                  {
                    case 0:
                      lim  = PCNT0_UFLOW;
                      ev   = PCNT0_EVT_0;
                      unit = PCNT0_UNIDX;
                      pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT0_UNIDX], &cntErg[i], 0);
                      break;
                    #if (USE_CNT_INP > 1)
                        case 1:
                          lim  = PCNT1_UFLOW;
                          ev   = PCNT1_EVT_0;
                          unit = PCNT1_UNIDX;
                          pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT1_UNIDX], &cntErg[i], 0);
                          break;
                      #endif
                    #if (USE_CNT_INP > 2)
                        case 2:
                          lim  = PCNT2_UFLOW;
                          ev   = PCNT2_EVT_0;
                          unit = PCNT2_UNIDX;
                          pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT2_UNIDX], &tmpErg, 0);
                          break;
                      #endif
                    #if (USE_CNT_INP > 3)
                        case 3:
                          lim  = PCNT3_UFLOW;
                          ev   = PCNT3_EVT_0;
                          unit = PCNT3_UNIDX;
                          pcnt_res = xQueueReceive(pcnt_evt_queue[PCNT3_UNIDX], &tmpErg, 0);
                          break;
                      #endif
                    default:
                      break;
                  }
                if (pcnt_res == pdTRUE)
                  {
                            //if (i == 0) { SOUT(cmsg); }
                            //SOUT("  "); SOUT(millis()); SOUT("  "); SOUT(i);
                            //SOUT(" "); SOUT("usCnt"); SOUT(" "); SOUT(cntErg[i].usCnt);
                            //SOUT(" T "); SOUT(cntThresh[i]); Serial.flush();

                    if ( (cntErg[i].usCnt > 0) )
                      {
                        cntErg[i].freq = (uint16_t) (1000000ul * cntThresh[i] * cntFakt[i] / cntErg[i].usCnt);
                          //cntErg[i].freq = (uint16_t) (cntErg[i].pulsCnt * 1000000ul / cntErg[i].usCnt);
                        //SOUT(" "); SOUT(cntErg[i].freq); Serial.flush();
                      }
                    else
                      {
                        cntErg[i].freq = 0;
                      }
                            //SOUT(" "); SOUT(i); SOUT(" "); SOUT((uint32_t) cntErg[i].freq);
                  }
                // autorange
                #if (USE_CNT_AUTORANGE > OFF)
                    // check for auto range switching

                    if (cntErg[i].usCnt > PNCT_AUTO_SWDN)
                      { // low freq
                        if (cntFilt[i] > -5)
                          {
                            SVAL("SWDN filt ", cntFilt[i]);
                            cntFilt[i]--;
                            usleep(500000);
                          }
                        if ((cntThresh[i] > 1) && (cntFilt[i] > -5))
                          {
                            cntThresh[i] /= 2;
                            doIt = true;
                            STXT("SWDN new ", cntThresh[i]);
                            usleep(500000);
                          }
                      }
                    else if ( (cntErg[i].usCnt < PNCT_AUTO_SWUP) && (cntErg[i].pulsCnt > 0) )
                      { // high freq
                        if (cntFilt[i] < 5)
                          {
                            STXT("SWUP filt ", cntFilt[i]);
                            cntFilt[i]++;
                            usleep(500000);
                          }
                        if ((cntThresh[i] < 16) && (cntFilt[i] > 5))
                          {
                            cntThresh[i] *= 2;
                            doIt = true;
                            STXT("SWUP new ", cntThresh[i]);
                            usleep(500000);
                          }
                      }
                    else
                      {
                        cntFilt[i] = 0;
                      }

                    if (doIt)
                      {
                        pcnt_counter_pause(unit);
                        logESP(pcnt_event_disable  (unit, ev),            cmsg, i);
                        logESP(pcnt_set_event_value(unit, ev, cntThresh[i]), cmsg, i);
                        pcnt_counter_clear(unit);
                        pcnt_counter_resume(unit);
                        logESP(pcnt_event_enable   (unit, ev),            cmsg, i);
                        doIt = false;
                        cntThresh[i] = 0;
                      }
                  #endif // USE_CNT_AUTORANGE
              }
                      //Serial.flush();
          #endif
        #if (USE_PWM_INP > OFF)
            mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 1);
            pwmInVal->lowVal = mcpwm_capture_signal_get_value(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0);

            mcpwm_capture_enable(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 1);
            pwmInVal->highVal = mcpwm_capture_signal_get_value(MCPWM_UNIDX_0, MCPWM_SELECT_CAP0);
          #endif
      // --- standard input cycle ---
        #ifdef USE_INPUT_CYCLE
            if (inputT.TOut())
              {
                inpIdx++;
                    //SOUT("b1a ");
                    //heapFree("+meascyc");
                    //STXT(" # MEASCYCLE ");
                inputT.startT();
                  //SOUT("b1b ");
                switch(inpIdx)
                  {
                    case 1: // BME280_I2C / BME680_I2C
                        //SOUT("c1 ");
                        #if (USE_BME280_I2C > OFF)
                            // BME280 temperature
                              bme280T = round(bme280_1.readTemperature() * 10) / 10;
                              #if (BME280T_FILT > 0)
                                  bme280T = bme280TVal.doVal(bme280T);
                                #endif
                            // BME280 humidity
                              bme280H = round(bme280_1.readHumidity() * 10) / 10;
                              #if (BME280H_FILT > 0)
                                  bme280H = bme280HVal[0].doVal( bme280H);
                                #endif
                            // BME280 envirement air pressure
                              bme280P = round((bme280_1.readPressure() / 100) + 0.5);
                              #if (BME280P_FILT > 0)
                                  bme280P = bme280PVal[0].doVal(bme280P);
                                #endif
                          #endif
                        #if (USE_BME680_I2C > OFF)
                            // BME680 temperature
                              //bme680T = round((bme680.readTemperature() * 10 + 5)/10);
                              bme680T = round((bme680.temperature * 10 + 5) / 10);
                              #if (BME680T_FILT > 0)
                                  bme680T = bme280TVal.doVal(bme680T);
                                #endif
                            // BME680 humidity
                              //bme680H = round((bme680.readHumidity() * 10 + 5 ) / 10);
                              bme680H = round((bme680.humidity * 10 + 5 ) / 10);
                              #if (BME680H_FILT > 0)
                                  bme680H = bme680HVal[0].doVal( bme680H);
                                #endif
                            // BME680 envirement air pressure
                              //bme680P = round((bme680.readPressure() / 100) + 0.5);
                              bme680P = round((bme680.pressure / 100) + 0.5);
                              #if (BME680P_FILT > 0)
                                  bme680P = bme680PVal[0].doVal(bme680P);
                                #endif
                            // BME680 gas sensor resistance
                              //bme680G = (float) (bme680.readGas() / 100);
                              bme680G = (float) (bme680.gas_resistance / 100);
                                  //S4VAL(" BME680 values T H P G", bme680T, bme680H, bme680P, bme680G );
                              #if (BME680G_FILT > 0)
                                  bme680G = bme680GVal[0].doVal(bme680G);
                                #endif
                          #endif
                      break;
                    case 2: // CCS811_I2C
                        #if (USE_CCS811_I2C > OFF)
                            if (ccs811.available())
                              {
                                if (ccs811.readData());  // CSS811 internal read data
                                // CO2 value
                                  ccsC = ccs811.geteCO2();
                                  #if (CCS811C_FILT > OFF)
                                      ccsC = ccsCVal.doVal(ccsC);
                                    #endif
                                // TVOC value
                                  ccsT = ccs811.getTVOC();
                                  #if (CCS811T_FILT > 0)
                                      ccsT = ccsTVal.doVal(ccsT);
                                    #endif
                              }
                          #endif
                      break;
                    case 3: // INA3221_I2C 3x U+I measures
                        //SOUT(" c3");
                        #if (USE_INA3221_I2C > OFF)
                            #if (INA3221U1_ACT > OFF)
                                inaU[0][0] = ina3221.getBusVoltage_V(1);
                                #if (INA3221I1_FILT > OFF)
                                    inaU[0][0] = ccsCVal.doVal(inaU[0][0]);
                                  #endif
                                //S2VAL(" incycle 3221 ina[0][0] inaUold[0][0] ", inaUold[0][0], inaU[0][0]);
                              #endif // INA3221U1_ACT
                            #if (INA3221I1_ACT > OFF)
                                inaI[0][0] = ina3221.getCurrent_mA(1);
                                #if (INA3221U1_ACT > OFF)
                                    inaP[0][0] = (inaU[0][0] * inaI[0][0]) / 1000;
                                  #endif
                              #endif // INA3221I1_ACT
                            #if (INA3221U2_ACT > OFF)
                                inaU[0][1] = ina3221.getBusVoltage_V(2);
                                #if (INA3221U2_FILT > OFF)
                                    inaU[0][1] = ccsCVal.doVal(inaU[0][1]);
                                  #endif
                              #endif // INA3221U2_ACT
                            #if (INA3221I2_ACT > OFF)
                                inaI[0][1] = -ina3221.getCurrent_mA(2);
                                #if (INA3221U2_ACT > OFF)
                                    inaP[0][1] = (inaU[0][1] * inaI[0][1]) / 1000;
                                  #endif // INA3221U2_ACT
                              #endif // INA3221I2_ACT
                            #if (INA3221U3_ACT > OFF)
                                inaU[0][2] = ina3221.getBusVoltage_V(3);
                                #if (INA3221U3_FILT > OFF)
                                    inaU[0][2] = ccsCVal.doVal(inaU[0][2]);
                                  #endif
                              #endif // INA3221U3_ACT
                            #if (INA3221I3_ACT > OFF)
                                inaI[0][2] = -ina3221.getCurrent_mA(3);
                                #if (INA3221I3_FILT > OFF)
                                    inaI[0][2] = ccsCVal.doVal(inaI[0][2]);
                                  #endif
                                #if (INA3221U3_ACT > OFF)
                                    inaP[0][2] = (inaU[0][2] * inaI[0][2]) / 1000;
                                  #endif // INA3221U3_ACT
                              #endif // INA3221I3_ACT
                          #endif
                      break;
                    case 4: // DS18B20_1W
                        //SOUT(" c4");
                        #if (USE_DS18B20_1W_IO > OFF)
                            outStr = "";
                            //outStr = getDS18D20Str();
                            dispText(outStr ,  0, 1, outStr.length());
                          #endif
                      break;
                    case 5: // MQ135_GAS_ANA
                        //SOUT(" c5");
                        #if (USE_MQ135_GAS_ANA > OFF)
                            #if (MQ135_GAS_ADC > OFF)
                                gasVal.doVal(analogRead(PIN_MQ135));
                                      //STXT(" gas measurment val = ", gasValue);
                                gasValue = (int16_t) valGas.value((double) gasValue);
                                      //STXT("    gasValue = ", gasValue);
                              #endif
                            #if (MQ135_GAS_1115 > OFF)
                              #endif
                          #endif
                      break;
                    case 6: // MQ3_ALK_ANA
                        //SOUT(" c6");
                        #if (USE_MQ3_ALK_ANA > OFF)
                            #if (MQ3_ALK_ADC > OFF)
                              #endif
                            #if (MQ3_ALK_1115 > OFF)
                                //ads[0].setGain(MQ3_1115_ATT);
                                //ads[0].setDataRate(RATE_ADS1115_860SPS);
                                //ads[0].startADCReading(MUX_BY_CHANNEL[MQ3_1115_CHIDX], /*continuous=*/false);
                                usleep(1200); // Wait for the conversion to complete
                                //while (!ads[0].conversionComplete());
                                //alk = ads[0].getLastConversionResults();   // Read the conversion results
                                alkVal.doVal(alk);   // Read the conversion results
                                //alk[0] = (uint16_t) (1000 * ads[0].computeVolts(alkVal[0].doVal(ads->readADC_SingleEnded(MQ3_1115_CHIDX))));
                              #endif
                          #endif
                      break;
                    case 7: // PHOTO_SENS_ANA
                        //SOUT(" c7");
                        #if (USE_PHOTO_SENS_ANA > OFF)
                            #if (PHOTO1_ADC > OFF)
                                //photoVal[0].doVal(analogRead(PIN_PHOTO1_SENS));
                                photof[0] = (float) analogRead(PIN_PHOTO1_SENS);
                                        //SVAL(" photo1  new ", photof[0]);
                                //photof[0] = photof[0];
                                        //SVAL(" photo1  new ", photof[0]);
                                photof[0] = photoScal[0].scale(photof[0]);
                              #endif
                            #if (PHOTO1_1115 > OFF)
                              #endif
                          #endif
                      break;
                    case 8: // POTI_ANA
                        //SOUT(" c8");
                        #if (USE_POTI_ANA > OFF)
                            #if (POTI1_ADC > OFF)
                              #endif
                            #if (POTI1_1115 > OFF)
                                poti[0]  = ads[POTI1_1115_UNIDX].getResult(POTI1_1115_CHIDX);
                                potif[0] = ads[POTI1_1115_UNIDX].getVolts(POTI1_1115_CHIDX);
                                    //S3VAL(" main vcc50f unit chan Volts ", VCC_1115_UNIDX, VCC50_1115_CHIDX, vcc50f );
                                //potif[0] = potifScal[0].scale(potif[0]);
                              #endif
                          #endif
                      break;
                    case 9: // USE_VCC50_ANA
                        //SOUT(" c9");
                        #if (USE_VCC50_ANA > OFF)
                            #if (VCC50_ADC > OFF)
                              #endif
                            #if (VCC50_1115 > OFF)
                                vcc50  = ads[VCC_1115_UNIDX].getResult(VCC50_1115_CHIDX);
                                vcc50f = ads[VCC_1115_UNIDX].getVolts(VCC50_1115_CHIDX);
                                    //S3VAL(" main vcc50f unit chan Volts ", VCC_1115_UNIDX, VCC50_1115_CHIDX, vcc50f );
                                vcc50f = vcc50fScal.scale(vcc50f);
                              #endif
                          #endif
                      break;
                    case 10: // USE_VCC33_ANA
                        //SOUT(" c10");
                        #if (USE_VCC33_ANA > OFF)
                            #if (VCC33_ADC > OFF)
                              #endif
                            #if (VCC33_1115 > OFF)
                                vcc33  = ads[VCC_1115_UNIDX].getResult(VCC33_1115_CHIDX);
                                vcc33f = ads[VCC_1115_UNIDX].getVolts(VCC33_1115_CHIDX);
                                    //S3VAL(" main vcc33f unit chan Volts ", VCC_1115_UNIDX, VCC33_1115_CHIDX, vcc33f );
                                //vcc33f = vcc33fScal.scale(vcc33f);
                              #endif
                          #endif
                    case 11: // ACS712_ANA
                        //SOUT(" c11");
                        #if (USE_ACS712_ANA > OFF)
                            #if (I712_1_ADC > OFF)
                              #endif
                            #if (I712_1_1115 > OFF)
                                i712[0]  = ads[I712_1_1115_UNIDX].getResult(VCC33_1115_CHIDX);
                                i712f[0] = ads[I712_1_1115_UNIDX].getVolts(VCC33_1115_CHIDX);
                                i712f[0] -= vcc50f/2;
                                i712f[0] *= 185;
                                        //S2VAL(" 712 Isup     ", i712[0], i712f[0]);
                              #endif
                          #endif
                      break;
                    case 12: // TYPE_K_SPI
                        //SOUT(" c12");
                        #if (USE_TYPE_K_SPI > OFF)
                            int8_t  tkerr = (int8_t) ISOK;
                            int16_t ival = TypeK1.actT();
                            tkerr = TypeK1.readErr();
                                  //SVAL(" typeK1 err ", tkerr);
                            if (!tkerr)
                              {
                                tk1Val    = valTK1.value((double) ival);
                                    //SVAL(" k1val ", tk1Val);
                                ival      = TypeK1.refT();
                                    //SVAL(" k1ref raw = ", (int) ival);
                                tk1ValRef = valTK1ref.value((double) ival);
                                    //SVAL(" k1ref = ", (int) tk1ValRef);
                              }
                            #if (USE_TYPE_K_SPI > 1)
                                ival      = TypeK2.actT();
                                tkerr     = TypeK2.readErr() % 8;
                                    //SVAL(" typeK1 err ", tkerr);
                                if (!tkerr)
                                  {
                                    tk2Val    = valTK2.value((double) ival);
                                        //SVAL(" k2val ", tk2Val);
                                    ival      = TypeK2.refT();
                                        //SVAL(" k2ref raw = ", (int) ival);
                                    tk2ValRef = valTK2ref.value((double) ival);
                                        //SVAL(" k2ref = ", (int) tk2ValRef);
                                  }
                              #endif
                          #endif
                      break;
                    case 13: // CNT_INP
                        //SOUT(" c13");
                        #if (USE_CNT_INP > OFF)
                            #ifdef USE_PW
                                getCNTIn();
                              #endif
                          #endif
                      break;
                    case 14: // DIG_INP
                        //SOUT(" c14");
                        #if (USE_DIG_INP > OFF)
                            getDIGIn();
                          #endif
                      break;
                    case 15: // ESPHALL
                        //SOUT(" c15");
                        #if (USE_ESPHALL > OFF)
                            valHall = hallRead();
                          #endif
                      break;
                    case 16: // MCPWM
                        //SOUT(" c16");
                        #if (USE_MCPWM > OFF)
                            getCNTIn();
                          #endif
                      break;
                    case 17: // MQTT
                        //SOUT(" c17");
                        #if (USE_MQTT > OFF)
                            mqtt.eventLoop();
                          #endif
                        //SOUT(" c17a");
                      break;
                    case 18:
                        #if defined(USE_PZEM017_RS485)
                            //if (pzemT.TOut())
                              {
                                //pzems.updateValues(pzemIdx);
                                pzems.updateValues(&pzemData[pzemIdx]);
                                pzemIdx++;
                                if (pzemIdx >= NUM_PZEMS)
                                  {
                                    pzemIdx = 0;
                                  }
                                else
                                  {
                                    inpIdx--;
                                  }
                                //pzemT.startT();
                              }
                          #endif // USE_PZEM017_RS485
                      break;
                    default:
                        inpIdx = 0;
                      break;
                  }
                    //heapFree("-meascyc");
              }
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
        #if (PROJECT == PRJ_TEST_LIB_PZEM017)
            //pzemU = pzems.voltage();
            //pzemI = pzems.current();
            //pzemP = pzems.power();
            //pzemE = pzems.energy();
            //S2VAL("U ", pzemU[0], "V");
            //S2VAL("I ", pzemI[0], "A");
            //S2VAL("P ", pzemP[0], "W");
            //S2VAL("E ", pzemE[0], "Wh");
            //SVAL("Update ", pzems.updateValues());
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
                            #if defined(USE_BME280_I2C)
                                if (bme280T != bme280Told)
                                  {
                                        //SVAL(" 280readT  new ", bme280T);
                                    #if (USE_MQTT > OFF)
                                        if (errMQTT == MD_OK)
                                          {
                                            valBME280t = bme280T;
                                            errMQTT = (int8_t) mqtt.publish(topBME280t.c_str(), (uint8_t*) valBME280t.c_str(), valBME280t.length());
                                            soutMQTTerr(topBME280t.c_str(), errMQTT);
                                                //SVAL(topBME280t, valBME280t);
                                          }
                                      #endif
                                    #if defined(USE_WEBSERVER)
                                        tmpStr = "SVA0";
                                        tmpval16 = (int16_t) (bme280T+ 0.5);
                                        tmpStr.concat(tmpval16);
                                        pmdServ->updateAll(tmpStr);
                                      #endif
                                    bme280Told = bme280T;
                                  }
                                if (bme280P != bme280Pold)
                                  {
                                    #if (USE_MQTT > OFF)
                                        if (errMQTT == MD_OK)
                                          {
                                            valBME280p = bme280P;
                                            errMQTT = (int8_t) mqtt.publish(topBME280p.c_str(), (uint8_t*) valBME280p.c_str(), valBME280p.length());
                                            soutMQTTerr(topBME280p.c_str(), errMQTT);
                                                //SVAL(topBME280p, valBME280p);
                                          }
                                      #endif
                                    #if defined(USE_WEBSERVER)
                                        tmpStr = "SVA1";
                                        tmpval16 = (uint16_t) bme280P;
                                        tmpStr.concat(tmpval16);
                                        pmdServ->updateAll(tmpStr);
                                      #endif
                                    bme280Pold = bme280P;
                                  }
                                if (bme280H != bme280Hold)
                                  {
                                    #if (USE_MQTT > OFF)
                                        if (errMQTT == MD_OK)
                                          {
                                            valBME280h = bme280H;
                                            errMQTT = (int8_t) mqtt.publish(topBME280h.c_str(), (uint8_t*) valBME280h.c_str(), valBME280h.length());
                                            soutMQTTerr(topBME280h.c_str(), errMQTT);
                                                //SVAL(topBME280h, valBME280h);
                                          }
                                      #endif
                                    #if defined(USE_WEBSERVER)
                                        tmpStr = "SVA2";
                                        tmpval16 = (int16_t) bme280H;
                                        tmpStr.concat(tmpval16);
                                        pmdServ->updateAll(tmpStr);
                                      #endif
                                    bme280Hold = bme280H;
                                  }
                              #endif
                            #if (USE_BME680_I2C > OFF)
                                if (bme680T != bme680Told)
                                  {
                                        //SVAL(" 680readT  new ", bme280T);
                                    #if (USE_MQTT > OFF)
                                        if (errMQTT == MD_OK)
                                          {
                                            valBME680t = bme680T;
                                            errMQTT = (int8_t) mqtt.publish(topBME680t.c_str(), (uint8_t*) valBME680t.c_str(), valBME680t.length());
                                            soutMQTTerr(topBME680t.c_str(), errMQTT);
                                                //SVAL(topBME280t, valBME280t);
                                          }
                                      #endif
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
                                      #endif
                                    ccsTold    = ccsT;
                                        //SVAL(" 811readT  new ", ccsT);
                                  }
                              #endif
                            outpIdx++;
                          //break;
                        case 3:  // INA3221_I2C 3x U+I measures
                            #if defined(USE_INA3221_I2C)
                                #if (INA3221U1_ACT > OFF)
                                    if (inaU[0][0] != inaUold[0][0])
                                      {
                                        valINA3221u[0][0] = inaU[0][0];
                                            //S2VAL(" incycle 3221 ina[0][0] inaUold[0][0] ", inaUold[0][0], inaU[0][0]);
                                            //SVAL(" U 3.3    new ", inaU[0][0]);
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                errMQTT = (int8_t) mqtt.publish(topina3221u[0].c_str(),
                                                                                 (uint8_t*) valINA3221u[0][0].c_str(),
                                                                                valINA3221u[0][0].length());
                                                soutMQTTerr(topina3221u[0].c_str(), errMQTT);
                                                    //SVAL(topina3221u[0].c_str(), valINA3221u[0][0]);
                                              }
                                          #endif
                                        #if defined(USE_WEBSERVER)
                                          #endif
                                        inaUold[0][0]     = inaU[0][0];
                                      }
                                  #endif // INA3221U1_ACT
                                #if (INA3221I1_ACT > OFF)
                                    if (inaI[0][0] != inaIold[0][0])
                                      {
                                        valINA3221i[0][0] = inaI[0][0];
                                            SVAL(" I 3.3 new ", inaI[0][0]);
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                errMQTT = (int8_t) mqtt.publish(topina3221i[0].c_str(),
                                                                                (uint8_t*) valINA3221i[0][0].c_str(),
                                                                                valINA3221i[0][0].length());
                                                soutMQTTerr(topina3221i[0].c_str(), errMQTT);
                                                    SVAL(topina3221i[0].c_str(), valINA3221i[0][0]);
                                              }
                                          #endif
                                      }
                                    #if defined(USE_WEBSERVER)
                                      #endif
                                    inaIold[0][0]     = inaI[0][0];
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                              }
                                          #endif
                                        #if defined(USE_WEBSERVER)
                                          #endif
                                  #endif // INA3221I1_ACT
                                #if (INA3221U2_ACT > OFF)
                                    if (inaU[0][1] != inaUold[0][1])
                                      {
                                        valINA3221u[0][1] = inaU[0][1];
                                        //pubINA3221u[0][1] = TRUE;
                                            //SVAL(" U 5.0    new ", inaU[0][1]);
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                errMQTT = (int8_t) mqtt.publish(topina3221u[1].c_str(),
                                                                                (uint8_t*) valINA3221u[0][1].c_str(),
                                                                                valINA3221u[0][1].length());
                                                soutMQTTerr(topina3221u[1].c_str(), errMQTT);
                                                    //SVAL(topina3221u[1].c_str(), valINA3221u[0][1]);
                                              }
                                          #endif
                                        #if defined(USE_WEBSERVER)
                                          #endif
                                        inaUold[0][1]     = inaU[0][1];
                                      }
                                  #endif // INA3221U2_ACT
                                #if (INA3221I2_ACT > OFF)
                                    if (inaI[0][1] != inaIold[0][1])
                                      {
                                        valINA3221i[0][1] = inaI[0][1];
                                        //pubINA3221i[0][1] = TRUE;
                                            //SVAL(" I 5.0    new ", inaI[0][1]);
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                errMQTT = (int8_t) mqtt.publish(topina3221i[1].c_str(),
                                                                                (uint8_t*) valINA3221i[0][1].c_str(),
                                                                                valINA3221i[0][1].length());
                                                soutMQTTerr(topina3221i[1].c_str(), errMQTT);
                                                    //SVAL(topina3221i[1].c_str(), valINA3221i[0][1]);
                                              }
                                          #endif
                                        #if defined(USE_WEBSERVER)
                                          #endif
                                        inaIold[0][1]     = inaI[0][1];
                                      }
                                    #if (INA3221U2_ACT > OFF)
                                        if (inaP[0][1] != inaPold[0][1])
                                          {
                                            valINA3221p[0][1] = inaP[0][1];
                                              //pubINA3221p[0][1] = TRUE;
                                                //SVAL(" P 5.0    new ", inaP[0][1]);
                                            #if (USE_MQTT > OFF)
                                                if (errMQTT == MD_OK)
                                                  {
                                                    errMQTT = (int8_t) mqtt.publish(topina3221p[1].c_str(),
                                                                                    (uint8_t*) valINA3221p[0][1].c_str(),
                                                                                    valINA3221p[0][1].length());
                                                    soutMQTTerr(topina3221p[1].c_str(), errMQTT);
                                                        //SVAL(topina3221p[1].c_str(), valINA3221p[0][1]);
                                                  }
                                              #endif
                                            #if defined(USE_WEBSERVER)
                                              #endif
                                          }
                                        inaPold[0][1]     = inaP[0][1];
                                      #endif // INA3221U2_ACT
                                  #endif // INA3221I2_ACT
                                #if (INA3221U3_ACT > OFF)
                                    //if (inaU[0][2] != inaUold[0][2])
                                      {
                                        valINA3221u[0][2] = inaU[0][2];
                                        //pubINA3221u[0][2] = TRUE;
                                            //sprintf(tmp_c32, "U3 %f V", inaU[0,2]);
                                            dispText("U3 " + valINA3221u[0][2], 0, 1);
                                            S2VAL(" U supply new ", inaU[0][2], "V");
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                errMQTT = (int8_t) mqtt.publish(topina3221u[2].c_str(),
                                                                                (uint8_t*) valINA3221u[0][2].c_str(),
                                                                                valINA3221u[0][2].length());
                                                soutMQTTerr(topina3221u[2].c_str(), errMQTT);
                                                    //SVAL(topina3221u[2].c_str(), valINA3221u[0][2]);
                                              }
                                          #endif
                                        #if defined(USE_WEBSERVER)
                                          #endif
                                        inaUold[0][2]     = inaU[0][2];
                                      }
                                  #endif // INA3221U3_ACT
                                #if (INA3221I3_ACT > OFF)
                                    //if (inaI[0][2] != inaIold[0][2])
                                      {
                                        valINA3221i[0][2] = inaI[0][2];
                                        //pubINA3221i[0][2] = TRUE;
                                            //sprintf(tmp_c32, " %fmA ", inaI[0,2]);
                                            dispText(valINA3221i[0][2], 6, 1);
                                            S2VAL(" I supply new ", inaI[0][2], "mA");
                                        #if (USE_MQTT > OFF)
                                            if (errMQTT == MD_OK)
                                              {
                                                errMQTT = (int8_t) mqtt.publish(topina3221i[2].c_str(),
                                                                                (uint8_t*) valINA3221i[0][2].c_str(),
                                                                                valINA3221i[0][2].length());
                                                soutMQTTerr(topina3221i[2].c_str(), errMQTT);
                                                    //SVAL(topina3221i[2].c_str(), valINA3221i[0][2]);
                                              }
                                          #endif
                                        #if defined(USE_WEBSERVER)
                                          #endif
                                        inaIold[0][2]     = inaI[0][2];
                                      }
                                    #if (INA3221U3_ACT > OFF)
                                        //if (inaP[0][2] != inaPold[0][2])
                                          {
                                            valINA3221p[0][2] = inaP[0][2];
                                            //pubINA3221p[0][2] = TRUE;
                                            inaPold[0][2]     = inaP[0][2];
                                            //sprintf(tmp_c32, " %.2fmW ", inaP[0,2]);
                                            dispText(valINA3221p[0][2], 9, 1);
                                                S2VAL(" P supply new ", inaP[0][2], "W");
                                            #if (USE_MQTT > OFF)
                                                if (errMQTT == MD_OK)
                                                  {
                                                    errMQTT = (int8_t) mqtt.publish(topina3221p[2].c_str(),
                                                                                    (uint8_t*) valINA3221p[0][2].c_str(),
                                                                                    valINA3221p[0][2].length());
                                                    soutMQTTerr(topina3221p[2].c_str(), errMQTT);
                                                        //SVAL(topina3221p[2].c_str(), valINA3221p[0][2]);
                                                  }
                                              #endif
                                            inaPold[0][2]     = inaP[0][2];
                                          }
                                      #endif // INA3221U3_ACT
                                  #endif // INA3221I3_ACT
                              #endif // USE_INA3221_I2C
                          //outpIdx++;
                          break;
                        case 4:  // DS18B20_1W temperature
                            #if (USE_DS18B20_1W_IO > OFF)
                                #if (USE_MQTT > OFF)
                                    if (errMQTT == MD_OK)
                                      {
                                      }
                                  #endif
                                #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                                //SVAL(topina3221u[1].c_str(), valINA3221u[0][1]);
                                          }
                                      #endif
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                    #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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
                                #if defined(USE_WEBSERVER)
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

                                            #if defined(USE_WEBSERVER)
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

                                            #if defined(USE_WEBSERVER)
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
                                  #if defined(USE_WEBSERVER)
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
                                  #if defined(USE_WEBSERVER)
                                    #endif
                                  testLEDold = testLED;
                                }
                            #endif
                        case 20: // WEBSERVER
                          #if defined(USE_WEBSERVER)
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
                        case 21: // PZEM DC energy module
                          #if defined(USE_PZEM017_RS485)
                                  //if (pzemU[0] != pzemUold[0])
                                    {
                                      valpzemU[0] = pzemData[0].voltage;
                                      //pubpzemU[0] = TRUE;
                                          //sprintf(tmp_c32, "U3 %f V", pzemU[0]);
                                          dispText("Us1 " + valpzemU[0], 0, 2);
                                          S2VAL(" Us1 PZEM new ", pzemData[0].voltage, "V");
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topina3221u[2].c_str(),
                                                                              (uint8_t*) valINA3221u[0][2].c_str(),
                                                                              valINA3221u[0][2].length());
                                              soutMQTTerr(topina3221u[2].c_str(), errMQTT);
                                                  //SVAL(topina3221u[2].c_str(), valINA3221u[0][2]);
                                            }
                                        #endif
                                      #if defined(USE_WEBSERVER)
                                        #endif
                                      //pzemUold[0]     = pzemU[0];
                                    }
                                  //if (pzemI[0] != pzemIold[0])
                                    {
                                      valpzemI[0] = pzemData[0].current;
                                      //pubpzemI[0] = TRUE;
                                          //sprintf(tmp_c32, " %fmA ", inaI[0,2]);
                                          dispText(valpzemI[0], 6, 2);
                                          S2VAL(" Is1 PZEM new ", pzemData[0].current, "A");
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topina3221i[2].c_str(),
                                                                              (uint8_t*) valINA3221i[0][2].c_str(),
                                                                              valINA3221i[0][2].length());
                                              soutMQTTerr(topina3221i[2].c_str(), errMQTT);
                                                  //SVAL(topina3221i[2].c_str(), valINA3221i[0][2]);
                                            }
                                        #endif
                                      #if defined(USE_WEBSERVER)
                                        #endif
                                      //pzemIold[0]     = pzemI[0];
                                    }
                                  //if (pzemP[0] != pzemPold[0])
                                    {
                                      valpzemP[0] = pzemData[0].power;
                                      //pubpzemP[0] = TRUE;
                                          //sprintf(tmp_c32, " %.2fmW ", inaP[0,2]);
                                          dispText(valpzemP[0], 9, 2);
                                          S2VAL(" Ps1 PZEM new ", pzemData[0].power, "W");
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topina3221p[2].c_str(),
                                                                              (uint8_t*) valINA3221p[0][2].c_str(),
                                                                              valINA3221p[0][2].length());
                                              soutMQTTerr(topina3221p[2].c_str(), errMQTT);
                                                  //SVAL(topina3221p[2].c_str(), valINA3221p[0][2]);
                                            }
                                        #endif
                                      //pzemPold[0][2]     = pzemP[0];
                                    }
                              #if (NUM_PZEMS > 1)
                                  //if (pzemU[1] != pzemUold[1])
                                    {
                                      valpzemU[1] = pzemData[1].voltage;
                                      //pubpzemU[0] = TRUE;
                                          //sprintf(tmp_c32, "U3 %f V", pzemU[0]);
                                          dispText("Us2 " + valpzemU[1], 0, 3);
                                          S2VAL(" Us2 PZEM new ", pzemData[1].voltage, "V");
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topina3221u[2].c_str(),
                                                                              (uint8_t*) valINA3221u[0][2].c_str(),
                                                                              valINA3221u[0][2].length());
                                              soutMQTTerr(topina3221u[2].c_str(), errMQTT);
                                                  //SVAL(topina3221u[2].c_str(), valINA3221u[0][2]);
                                            }
                                        #endif
                                      #if defined(USE_WEBSERVER)
                                        #endif
                                      //pzemUold[0]     = pzemU[0];
                                    }
                                  //if (pzemI[1] != pzemIold[1])
                                    {
                                      valpzemI[1] = pzemData[1].current;
                                      //pubpzemI[0] = TRUE;
                                          //sprintf(tmp_c32, " %fmA ", inaI[0,2]);
                                          dispText(valpzemI[1], 6, 3);
                                          S2VAL(" Is2 PZEM new ", pzemData[1].current, "A");
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topina3221i[2].c_str(),
                                                                              (uint8_t*) valINA3221i[0][2].c_str(),
                                                                              valINA3221i[0][2].length());
                                              soutMQTTerr(topina3221i[2].c_str(), errMQTT);
                                                  //SVAL(topina3221i[2].c_str(), valINA3221i[0][2]);
                                            }
                                        #endif
                                      #if defined(USE_WEBSERVER)
                                        #endif
                                      //pzemIold[0]     = pzemI[0];
                                    }
                                  //if (pzemP[1] != pzemPold[1])
                                    {
                                      valpzemP[1] = pzemData[1].power;
                                      //pubpzemP[0] = TRUE;
                                          //sprintf(tmp_c32, " %.2fmW ", inaP[0,2]);
                                          dispText(valpzemP[1], 9, 3);
                                          S2VAL(" Ps2 PZEM new ", pzemData[1].power, "W");
                                      #if (USE_MQTT > OFF)
                                          if (errMQTT == MD_OK)
                                            {
                                              errMQTT = (int8_t) mqtt.publish(topina3221p[2].c_str(),
                                                                              (uint8_t*) valINA3221p[0][2].c_str(),
                                                                              valINA3221p[0][2].length());
                                              soutMQTTerr(topina3221p[2].c_str(), errMQTT);
                                                  //SVAL(topina3221p[2].c_str(), valINA3221p[0][2]);
                                            }
                                        #endif
                                      //pzemPold[1]     = pzemP[1];
                                    }
                                #endif
                          #endif // USE_PZEM017_RS485
                              break;
                            default:
                              outpIdx = 0;
                              break;
                      }
                    #ifdef USE_STATUS
                        dispStatus("");
                      #endif
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
                                       //   #if defined(USE_WEBSERVER)
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
  // --- network -------------------------
    // --- WIFI
      #if defined(USE_WIFI) || defined(USE_WEBSERVER)
          uint8_t startWIFI(bool startup)
            {
              bool ret = MD_ERR;
              char _cssid[LOGINTXT_MAX_LEN + 1];
              char _cpw[LOGINTXT_MAX_LEN + 1];
                  //SVAL(" startWIFI   Start WiFi ", startup);
              #if defined(USE_WIFI)
                  dispStatus("  start WIFI");
                      //heapFree(" before generating ipList ");
                  if (startup)
                    {
                      md_ip_list ipList = md_ip_list(); // temporary object
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    SHEXVAL(" setup startWIFI created ipList ", (int) &ipList);
                                    STXT(" setup startWIFI add WIFI 0");
                                  #endif
                      sprintf(_cssid, "%s\0", WIFI_SSID0);
                      sprintf(_cpw, "%s\0", WIFI_SSID0_PW);
                      ipList.append(WIFI_FIXIP0, WIFI_GATEWAY0, WIFI_SUBNET, _cssid, _cpw);
                      #if (WIFI_ANZ_LOGIN > 1)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 1");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID1);
                          sprintf(_cpw, "%s\0", WIFI_SSID1_PW);
                          ipList.append(WIFI_FIXIP1, WIFI_GATEWAY1, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 2)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 2");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID2);
                          sprintf(_cpw, "%s\0", WIFI_SSID2_PW);
                          ipList.append(WIFI_FIXIP2, WIFI_GATEWAY2, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 3)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 3");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID3);
                          sprintf(_cpw, "%s\0", WIFI_SSID3_PW);
                          ipList.append(WIFI_FIXIP3, WIFI_GATEWAY3, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 4)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 4");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID4);
                          sprintf(_cpw, "%s\0", WIFI_SSID4_PW);
                          ipList.append(WIFI_FIXIP4, WIFI_GATEWAY4, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 5)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 5");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID5);
                          sprintf(_cpw, "%s\0", WIFI_SSID5_PW);
                          ipList.append(WIFI_FIXIP5, WIFI_GATEWAY5, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 6)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 6");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID6);
                          sprintf(_cpw, "%s\0", WIFI_SSID6_PW);
                          ipList.append(WIFI_FIXIP6, WIFI_GATEWAY6, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 7)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup startWIFI add WIFI 7");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID7);
                          sprintf(_cpw, "%s\0", WIFI_SSID7_PW);
                          ipList.append(WIFI_FIXIP7, WIFI_GATEWAY7, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                      #if (WIFI_ANZ_LOGIN > 8)
                                #if (DEBUG_MODE > CFG_DEBUG_STARTUP)
                                    STXT(" setup add WIFI 8");
                                  #endif
                          sprintf(_cssid, "%s\0", WIFI_SSID8);
                          sprintf(_cpw, "%s\0", WIFI_SSID8_PW);
                          ipList.append(WIFI_FIXIP8, WIFI_GATEWAY8, WIFI_SUBNET, _cssid, _cpw);
                        #endif
                                //STXT(UTLN(" setup startWIFI locWIFI fertig");

                                //md_ip_cell* pip = (md_ip_cell*) ipList.pFirst();
                                //char stmp[NET_MAX_SSID_LEN] = "";
                                        /*
                                          SOUT(" setup md_ip_list addr "); SOUT((u_long) &ipList);
                                          SOUT(" count "); SOUTLN(ipList.count());
                                          SOUT(" ip1: addr "); SOUTHEX((u_long) pip);
                                          SOUT(" locIP "); SOUTHEX(pip->locIP());
                                          SOUT(" gwIP ");  SOUTHEX(pip->gwIP());
                                          SOUT(" snIP ");  SOUTHEX(pip->snIP());
                                          pip->getSSID(stmp); SOUT(" ssid "); SOUT(stmp);
                                          pip->getPW(stmp); SOUT(" pw "); SOUTLN(stmp);
                                          pip = (md_ip_cell*) pip->pNext();
                                          SOUT(" ip2: addr "); SOUTHEX((u_long) pip);
                                          SOUT(" locIP "); SOUTHEX(pip->locIP());
                                          SOUT(" gwIP ");  SOUTHEX(pip->gwIP());
                                          SOUT(" snIP ");  SOUTHEX(pip->snIP());
                                          pip->getSSID(stmp); SOUT(" ssid "); SOUT(stmp);
                                          pip->getPW(stmp); SOUT(" pw "); SOUTLN(stmp);
                                          pip = (md_ip_cell*) pip->pNext();
                                          SOUT(" ip3: addr "); SOUTHEX((u_long) pip);
                                          SOUT(" locIP "); SOUTHEX(pip->locIP());
                                          SOUT(" gwIP ");  SOUTHEX(pip->gwIP());
                                          SOUT(" snIP ");  SOUTHEX(pip->snIP());
                                          pip->getSSID(stmp); SOUT(" ssid "); SOUT(stmp);
                                          pip->getPW(stmp); SOUT(" pw "); SOUTLN(stmp);
                                        */

                          //heapFree(" ipList generated ");
                      ret = wifi.scanWIFI(&ipList);
                              //SVAL(" scanWIFI ret=", ret);
                          //heapFree(" before deleting ipList ");
                      ipList.~md_ip_list();
                          //heapFree(" after deleting ipList ");
                    }
                  ret = wifi.startWIFI();
                      //SVAL(" startWIFI ret ", ret);
                  if (ret == MD_OK)
                    {
                      #if (USE_MD_ATSMARTHOME > OFF)
                          ctmp30[0]=0;
                          iret               = wifi.getSSID(ctmp30);
                            //S2VAL(" SSID ", iret, ctmp30);
                          atdbSetup.SSID     = ctmp30;
                          ctmp30[0]=0;
                          iret               = wifi.getPW(ctmp30);
                            //S2VAL(" PW ", iret, ctmp30);
                          atdbSetup.password = ctmp30;
                          atdbSetup.useWlan  = true;
                            //database.setupWiFi(&atdbSetup);
                        #endif
                      dispStatus("WIFI connected");
                    }
                    else
                    {
                      dispStatus("WIFI error");
                      #if (USE_MD_ATSMARTHOME > OFF)
                          atdbSetup.useWlan  = false;
                        #endif
                    }
                #endif // USE_WIFI
              return ret;
            }
        // --- NTP server
          void initNTPTime()
            {
              #if (USE_NTP_SERVER > OFF)
                  bool ret = wifi.initNTP();
                        #if (DEBUG_MODE >= CFG_DEBUG_DETAIL)
                          //Serial.print("initNTPTime ret="); Serial.print(ret);
                        #endif
                  if (ret = MD_OK)
                    {
                      dispStatus("NTPTime ok");
                    }
                    else
                    {
                      dispStatus("NTPTime error");
                    }
                #endif // USE_NTP_SERVER
            }
        #endif // USE_WIFI
    // --- webserver
      #if defined(USE_WEBSERVER)
          void startWebServer()
            {
              tmp_i8 = MD_ERR;
              if (!webOn)
                {
                  dispStatus("start webserver");
                  STXT(" startServer ... ");
                  if (WiFi.status() == WL_CONNECTED)
                    {
                      tmp_i8 = pmdServ->md_startServer();
                          #if (DEBUG_MODE >= CFG_DEBUG_DETAIL)
                              SVAL(" webserver ret ", tmp_i8);
                            #endif
                      if (tmp_i8 == MD_OK)
                        {
                          webOn = TRUE;
                        }
                    }
                  if (webOn)
                    {
                      dispStatus("Webserver online");
                      STXT("Webserver online");
                    }
                    else
                    {
                      dispStatus("Webserver ERROR");
                      STXT("Webserver ERROR");
                    }
                }
            }
          void readWebMessage()
            {
              md_message *pM   = NULL;
              int         itmp = 0;
              char*       ctmp = NULL;
              #if (USE_WS2812_MATRIX_OUT > OFF)
                  md_LEDPix24* ppix = NULL;
                #endif
              uint8_t     idx;
              char        tval;
              char        tdata;

              if (inMsgs->count() > 0)
                {
                  pM   = (md_message*) inMsgs->pFirst();
                  switch (pM->msgType())
                    {
                      case ME_TSOCKET:
                        SOUT(" Socket ");
                        break;

                      case ME_TREQ:
                        pmdServ->isRequest = true;
                        break;

                      case ME_TCONN:
                        newClient = true;
                        break;

                      default:
                        ctmp  = pM->payload();
                        tdata = pM->dataType();
                        tval  = ctmp[0];
                        idx   = ctmp[1] - '0';
                        ctmp += 2;
                            SVAL(" msg client ", pM->client());
                            SVAL(" tMsg ",       pM->msgType());
                            SVAL(" tData ",      (char) pM->dataType());
                            SVAL(" tval ",       tval);
                            SVAL(" payload ",    pM->payload());
                            SVAL(" idx ",        idx);
                            SVAL(" ctmp '",      ctmp);
                        if (tdata == MD_SINGLE)
                          {
                            //STXT("---- switch(tval) ----");
                            switch (tval)
                              {
                                case EL_TANALOG:
                                    SOUT(" -- Analog nu ");
                                  break;
                                case EL_TSLIDER:
                                    STXT(" -- Slider");
                                    itmp  = (int)strtol(ctmp, NULL, 10);
                                    switch (idx) // index of serverelement
                                      {
                                        case 1: // RGB-LED col24
                                          #if (USE_RGBLED_PWM > OFF)
                                              RGBLED->bright(itmp);
                                              //S2VAL("  -- rgbLED bright old new ", RGBLEDold->bright(), RGBLED->bright());
                                            #endif
                                          break;
                                        case 2: // 2821 line col24
                                          #if (USE_WS2812_LINE_OUT >OFF)
                                              //SVAL("  -- line bright old", line2812[1]->bright());
                                              line2812[1]->bright((uint8_t) itmp);
                                              //SVAL("  -- line bright new", line2812[1]->bright());
                                            #endif
                                          break;
                                        case 3:
                                          #if (USE_WS2812_MATRIX_OUT > OFF)
                                              ppix = outM2812[1].text->pix24;
                                              //SVAL("  -- matrix bright old", ppix->bright());
                                              ppix->bright((uint8_t) itmp);
                                              //SVAL("  -- matrix bright new", ppix->bright());
                                            #endif
                                          break;
                                        case 4:
                                          #if (USE_WS2812_MATRIX_OUT > OFF)
                                              ppix = outM2812[1].bmpB->pix24;
                                              //SVAL("  -- smilyB bright old"); SOUT(ppix->bright());
                                              ppix->bright((uint8_t) itmp);
                                              //SVAL("  -- smilyB bright new"); SOUT(ppix->bright());
                                              ppix = outM2812[1].bmpE->pix24;
                                              //SVAL("  -- smilyE bright old"); SOUT(ppix->bright());
                                              ppix->bright((uint8_t) itmp);
                                              //SVAL("  -- smilyE bright new"); SOUT(ppix->bright());
                                            #endif
                                          break;
                                      }
                                  break;
                                case EL_TCOLOR:
                                    STXT(" ---- Color ----");
                                    itmp  = (int)strtol(ctmp, NULL, 16);
                                    switch (idx) // index of serverelement
                                      {
                                        case 1: // RGB-LED col24
                                          #if (USE_RGBLED_PWM > OFF)
                                              //SVAL("  -- RGBLED color old", RGBLED[1]->col24());
                                              RGBLED->col24(itmp);
                                              //SVAL("  -- RGBLED color new", RGBLED[1]->col24());
                                            #endif
                                          break;
                                        case 2: // 2821 line col24
                                          #if (USE_WS2812_LINE_OUT >OFF)
                                              SHEXVAL(" -- line2812 color24 old ", line2812[1]->col24());
                                              line2812[1]->col24(itmp);
                                              SHEXVAL(" -- line2812 color24 new ", line2812[1]->col24());
                                              SHEXVAL(" -- line2812 color16 new ", Col16(line2812[1]->col24()));
                                            #endif
                                          break;
                                        case 3: // 2821 matrix col16
                                          #if (USE_WS2812_MATRIX_OUT > OFF)
                                              ppix = outM2812[1].text->pix24;
                                              SHEXVAL(" -- matrix color24 old", ppix->col24());
                                              ppix->col24(itmp);
                                              SHEXVAL(" -- matrix color24 new", ppix->col24());
                                              SHEXVAL(" -- matrix color16 new", Col16(ppix->col24()));
                                            #endif
                                          break;
                                        case 4: // 2821 matrix col16 bmp
                                          #if (USE_WS2812_MATRIX_OUT > OFF)
                                              ppix = outM2812[1].bmpB->pix24;
                                              SHEXVAL(" -- matrixB color24 old", ppix->col24());
                                              ppix->col24(itmp);
                                              SHEXVAL(" -- matrixB color24 new", ppix->col24());
                                              SHEXVAL(" -- matrixB color16 new", Col16(ppix->col24()));
                                              SHEXVAL(" -- matrixB color24 old", ppix->col24());
                                              ppix = outM2812[1].bmpE->pix24;
                                              SHEXVAL(" -- matrixE color24 old", ppix->col24());
                                              ppix->col24(itmp);
                                              SHEXVAL(" -- matrixE color24 new", ppix->col24());
                                              SHEXVAL(" -- matrixE color16 new", Col16(ppix->col24()));
                                            #endif
                                          break;
                                      }
                                  break;
                                case EL_TSWITCH:
                                    SOUT(" Switch ");
                                  break;
                                case EL_TTEXT:
                                    SOUT(" Text ");
                                  break;
                                case EL_TOFFSET:
                                    SOUT(" Offset ");
                                  break;
                                case EL_TGRAPH:
                                    SOUT(" Graph ");
                                  break;
                                case EL_TINDEX:
                                    SOUT(" Index ");
                                  break;
                                default:
                                    SOUT(" ERROR ");
                                  break;
                              }
                          }
                        break;
                    }
                  SOUT("'"); SOUT(pM->payload()); SOUT("'");
                  inMsgs->rem();
                  SVAL(" inMsgs.count ", inMsgs->count());
                }
            }
            /*
              void    handleClient(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
                {
                  AwsFrameInfo *info = (AwsFrameInfo*)arg;
                  char* txt = (char*) data;
                  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
                    { //  SOUT(" handleWebSocketMessage info->index "); SOUT(info->index); SOUT(" info->final "); SOUT(info->final); SOUT(" info->len "); SOUTLN(info->len);
                      data[len] = 0;
                      uint8_t type  = txt[0];  // extract obj type
                      uint8_t index = txt[1] - WS_IDX_OFFSET;  // extract index
                      int16_t value = atoi(&txt[2]);
                                //SOUT(" Payload type "); SOUT(type);
                                //SOUT(" index "); SOUT(index); SOUT(" len "); SOUT(len);
                                //SOUT(" data '"); SOUT(&txt[2]); SOUT(" = "); SOUT(value);
                                //SOUT(" ledList cnt "); SOUTLN(psliderList->count());

                      if (type == EL_TSLIDER)
                        {
                          md_slider* psl = (md_slider*) psliderList->pIndex(index);
                                //SOUT(" psl "); SOUTHEX((uint32_t) psl);
                          if (psl != NULL)
                            {
                              psl->destVal = value;
                              SOUT(" slider "); SOUT((index+1)); SOUT("="); SOUTLN(value);
                            }
                        }

                      else if (type == EL_TSWITCH)
                        {
                          md_switch* psw = (md_switch*) pswitchList->pIndex(index);
                          while (psw != NULL)
                            {
                              psw->destVal = value; SOUT(" switch "); SOUTLN(value);
                            }
                        }

                      else if (type == EL_TANALOG)
                        {
                          md_analog* pana = (md_analog*) panalogList->pIndex(index);
                          while (pana != NULL)
                            {
                              pana->destVal = value; SOUT(" analog "); SOUTLN(value);
                            }
                        }

                      else { }
                    }
                }
              */
            /*
              void handlingIncomingData()
                {


                  AwsFrameInfo *info = (AwsFrameInfo*)arg;

                  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
                    {
                      String hexColor = "";
                      for (int i=0; i < len; i++)
                        hexColor += ((char) data[i]);

                      Serial.println("Hex Color: " + hexColor);

                      long n = strtol(&hexColor[0], NULL, 16);
                      Serial.println(n);
                      strip.fill(n);
                      strip.show();
                    }
                }
              */
              // Callback for incoming event
            /*
              void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
                             void * arg, uint8_t *data, size_t len)
                {
                  switch(type)
                    {
                      case WS_EVT_CONNECT:
                        Serial.printf("Client connected: \n\tClient id:%u\n\tClient IP:%s\n",
                             client->id(), client->remoteIP().toString().c_str());
                        break;
                      case WS_EVT_DISCONNECT:
                        Serial.printf("Client disconnected:\n\tClient id:%u\n", client->id());
                        break;
                      case WS_EVT_DATA:
                        handlingIncomingData(client, arg, data, len);
                        break;
                      case WS_EVT_PONG:
                        Serial.printf("Pong:\n\tClient id:%u\n", client->id());
                        break;
                      case WS_EVT_ERROR:
                        Serial.printf("Error:\n\tClient id:%u\n", client->id());
                        break;
                    }
                }
              void configWebsite()
                {
                  webMD.createElement(EL_TSLIDER, "LED red", "%");
                  webMD.createElement(EL_TSLIDER, "LED green", "%");
                  webMD.createElement(EL_TSLIDER, "LED blue", "%");

                  webMD.createElement(EL_TANALOG, "DS18B20 Temp", "°C");
                  webMD.createElement(EL_TANALOG, "Type-K Temp", "°C");
                  webMD.createElement(EL_TANALOG, "BME_Temp", "°C");
                  webMD.createElement(EL_TANALOG, "BME_Humidity", "%");
                  webMD.createElement(EL_TANALOG, "BME_Pressure", "mb");
                  webMD.createElement(EL_TANALOG, "Gaswert", "");
                }
              */
        #endif // USE_WEBSERVER
    // --- MQTT
      #if (USE_MQTT > OFF)
          void startMQTT()
            {
              STXT("Connecting to MQTT...");
                  S2VAL(" startMQTT msgs-len &msgs[0]", sizeof(MQTTmsg_t), (uint32_t) &MQTTmsgs[0]);
              for ( uint8_t i=0 ; i < MQTT_MSG_MAXANZ - 1; i++)
                {
                  MQTTmsgs[i].pNext = (void*) &MQTTmsgs[i+1];
                      //S3VAL(" startMQTT i msgs[i] pNext", i, (uint32_t) &MQTTmsgs[i], (uint32_t) MQTTmsgs[i].pNext);
                }
              MQTTmsgs[MQTT_MSG_MAXANZ-1].pNext = (void*) &MQTTmsgs[0];
                  //S3VAL(" startMQTT i msgs[i] pNext", MQTT_MSG_MAXANZ-1, (uint32_t) &MQTTmsgs[MQTT_MSG_MAXANZ-1], (uint32_t) MQTTmsgs[MQTT_MSG_MAXANZ-1].pNext);
              connectMQTT();
            } // tested -> ok

          void connectMQTT() // TODO: move all subcribes to here -> reconnect
            {
              errMQTT = (int8_t) mqtt.connectTo(MQTT_HOST, MQTT_PORT);
              soutMQTTerr(" MQTT connect", errMQTT);
              if (errMQTT == MD_OK)
                {
                  #if (USE_BME280_I2C > OFF) // 1
                      topBME280t = topDevice + topBME280t;
                      errMQTT = (int8_t) mqtt.subscribe(topBME280t.c_str());
                          soutMQTTerr(" MQTT subscribe BME280t", errMQTT);
                      topBME280p = topDevice + topBME280p;
                      errMQTT = (int8_t) mqtt.subscribe(topBME280p.c_str());
                          soutMQTTerr(" MQTT subscribe BME280p", errMQTT);
                      topBME280h = topDevice + topBME280h;
                      errMQTT = (int8_t) mqtt.subscribe(topBME280h.c_str());
                          soutMQTTerr(" MQTT subscribe BME280h", errMQTT);
                    #endif
                  #if (USE_BME680_I2C > OFF) // 1
                      topBME680t = topDevice + topBME680t;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680t.c_str());
                          soutMQTTerr(" MQTT subscribe BME680t", errMQTT);
                      topBME680p = topDevice + topBME680p;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680p.c_str());
                          soutMQTTerr(" MQTT subscribe BME680p", errMQTT);
                      topBME680h = topDevice + topBME680h;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680h.c_str());
                          soutMQTTerr(" MQTT subscribe BME680h", errMQTT);
                      topBME680g = topDevice + topBME680g;
                      errMQTT = (int8_t) mqtt.subscribe(topBME680g.c_str());
                          soutMQTTerr(" MQTT subscribe BME680G", errMQTT);
                    #endif
                  #if (USE_CCS811_I2C > OFF)
                          topCCS811t = topDevice + topCCS811t;
                          errMQTT = (int8_t) mqtt.subscribe(topCCS811t.c_str());
                              soutMQTTerr(" MQTT subscribe CCS811t", errMQTT);
                          topCCS811c = topDevice + topCCS811c;
                          errMQTT = (int8_t) mqtt.subscribe(topCCS811c.c_str());
                              soutMQTTerr(" MQTT subscribe CCS811c", errMQTT);
                    #endif
                  #if (USE_INA3221_I2C > OFF) // unit 1
                      // channel 1
                        topina3221i[0] = topDevice + topina3221i[0];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221i[0].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221i[0]", errMQTT);

                        topina3221u[0] = topDevice + topina3221u[0];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221u[0].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221u[0]", errMQTT);

                        topina3221p[0] = topDevice + topina3221p[0];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221p[0].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221p[0]", errMQTT);
                      // channel 2
                        topina3221i[1] = topDevice + topina3221i[1];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221i[1].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221i[1]", errMQTT);

                        topina3221u[1] = topDevice + topina3221u[1];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221u[1].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221u[1]", errMQTT);

                        topina3221p[1] = topDevice + topina3221p[1];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221p[1].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221p[1]", errMQTT);
                      // channel 3
                        topina3221i[2] = topDevice + topina3221i[2];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221i[2].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221i[2]", errMQTT);
                        topina3221u[2] = topDevice + topina3221u[2];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221u[2].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221u[2]", errMQTT);
                        topina3221p[2] = topDevice + topina3221p[2];
                        errMQTT = (int8_t) mqtt.subscribe(topina3221p[2].c_str());
                            soutMQTTerr(" MQTT subscribe topina3221p[2]", errMQTT);
                      #if (USE_INA3221_I2C > 1)  // unit 2
                          // channel 1
                            topINA32212i[0] = topDevice + topINA32212i[0];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212i[0].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212i[0]", errMQTT);
                            topINA32212u[0] = topDevice + topINA32212u[0];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212u[0].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212u[0]", errMQTT);
                          // channel 2
                            topINA32212i[1] = topDevice + topINA32212i[1];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212i[1].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212i[1]", errMQTT);
                            topINA32212u[1] = topDevice + topINA32212u[1];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212u[1].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212u[1]", errMQTT);
                          // channel 3
                            topINA32212i[2] = topDevice + topINA32212i[2];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212i[2].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212i[2]", errMQTT);
                            topINA32212u[2] = topDevice + topINA32212u[2];
                            errMQTT = (int8_t) mqtt.subscribe(topINA32212u[2].c_str());
                                soutMQTTerr(" MQTT subscribe topINA32212u[2]", errMQTT);
                          #if (USE_INA3221_I2C > 2)  // unit 3
                              // channel 1
                                topINA32213i[0] = topDevice + topINA32213i[0];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213i[0].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213i[0]", errMQTT);
                                topINA32213u[0] = topDevice + topINA32212u[0];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213u[0].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213u[0]", errMQTT);
                              // channel 2
                                topINA32213i[1] = topDevice + topINA32213i[1];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213i[1].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213i[1]", errMQTT);
                                topINA32213u[1] = topDevice + topINA32213u[1];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213u[1].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213u[1]", errMQTT);
                              // channel 3
                                topINA32213i[2] = topDevice + topINA32213i[2];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213i[2].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213i[2]", errMQTT);
                                topINA32213u[2] = topDevice + topINA32213u[2];
                                errMQTT = (int8_t) mqtt.subscribe(topINA32213u[2].c_str());
                                    soutMQTTerr(" MQTT subscribe topINA32213u[2]", errMQTT);
                            #endif
                        #endif
                    #endif
                  #if (USE_DS18B20_1W_IO > OFF)
                      topDS18B201 = topDevice + topDS18B201;
                      errMQTT = (int8_t) mqtt.subscribe(topDS18B201.c_str());
                          soutMQTTerr(" MQTT subscribe DS18B201 ", errMQTT);
                      #if (USE_DS18B20_1W_IO > 1)
                          topDS18B202 = topDevice + topDS18B202;
                          errMQTT = (int8_t) mqtt.subscribe(topDS18B202.c_str());
                              soutMQTTerr(" MQTT subscribe DS18B202 ", errMQTT);
                        #endif
                    #endif
                  #if (USE_MQ135_GAS_ANA > OFF)
                    #endif
                  #if (USE_MQ3_ALK_ANA > OFF)
                      topMQ3alk = topDevice + topMQ3alk;
                      errMQTT = (int8_t) mqtt.subscribe(topMQ3alk.c_str());
                          soutMQTTerr(" MQTT subscribe MQ3alk", errMQTT);
                    #endif
                  #if (USE_PHOTO_SENS_ANA > OFF)
                      topPhoto1 = topDevice + topPhoto1;
                      errMQTT = (int8_t) mqtt.subscribe(topPhoto1.c_str());
                          soutMQTTerr(" MQTT subscribe Photo1 ", errMQTT);
                    #endif
                  #if (USE_POTI_ANA > OFF)
                      topPoti1 = topDevice + topPoti1;
                      errMQTT = (int8_t) mqtt.subscribe(topPoti1.c_str());
                          soutMQTTerr(" MQTT subscribe poti1", errMQTT);
                    #endif
                  #if (USE_VCC50_ANA > OFF)
                      topVCC50 = topDevice + topVCC50;
                      errMQTT = (int8_t) mqtt.subscribe(topVCC50.c_str());
                          soutMQTTerr(" MQTT subscribe VCC50", errMQTT);
                    #endif
                  #if (USE_VCC33_ANA > OFF)
                      topVCC33 = topDevice + topVCC33;
                      errMQTT = (int8_t) mqtt.subscribe(topVCC33.c_str());
                          soutMQTTerr(" MQTT subscribe VCC33", errMQTT);
                    #endif
                  #if (USE_ACS712_ANA > OFF)
                      topi7121 = topDevice + topi7121;
                      errMQTT = (int8_t) mqtt.subscribe(topi7121.c_str());
                          soutMQTTerr(" MQTT subscribe i7121", errMQTT);
                      #if (USE_ACS712_ANA > 1)
                          topi7122 = topDevice + topi7122;
                          errMQTT = (int8_t) mqtt.subscribe(topi7122.c_str());
                              soutMQTTerr(" MQTT subscribe i7122", errMQTT);
                          #if (USE_ACS712_ANA > 2)
                              topi7123 = topDevice + topi7123;
                              errMQTT = (int8_t) mqtt.subscribe(topi7123.c_str());
                                  soutMQTTerr(" MQTT subscribe i7123", errMQTT);
                              #if (USE_ACS712_ANA > 3)
                                  topi7124 = topDevice + topi7124;
                                  errMQTT = (int8_t) mqtt.subscribe(topi7124.c_str());
                                      soutMQTTerr(" MQTT subscribe i7124", errMQTT);
                                #endif
                            #endif
                        #endif
                    #endif
                  #if (USE_TYPE_K_SPI > 0)
                    #endif
                  #if (USE_CNT_INP > OFF)
                    #endif
                  #if (USE_DIG_INP > OFF)
                    #endif
                  #if (USE_ESPHALL > OFF)
                    #endif
                  #if (USE_MCPWM > OFF)
                    #endif
                  #if (USE_RGBLED_PWM > OFF)
                      topRGBBright = topDevice + topRGBBright;
                      errMQTT = (int8_t) mqtt.subscribe(topRGBBright.c_str());
                          soutMQTTerr(" MQTT subscribe LEDBright ", errMQTT);

                      topRGBCol = topDevice + topRGBCol;
                      errMQTT = (int8_t) mqtt.subscribe(topRGBCol.c_str());
                          soutMQTTerr(" MQTT subscribe LEDCol ", errMQTT);
                    #endif
                  #if (USE_WS2812_MATRIX_OUT > OFF)
                    #endif
                  #if (USE_WS2812_LINE_OUT > OFF)
                    #endif
                  #if (USE_BUZZER_PWM > OFF)
                    #endif
                  #if (USE_GEN_DIG_OUT > OFF)
                      toptestLED = topDevice + toptestLED;
                      errMQTT = (int8_t) mqtt.subscribe(toptestLED.c_str());
                          soutMQTTerr(" MQTT subscribe testLED", errMQTT);
                    #endif
                }
            }

          void soutMQTTerr(String text, int8_t errMQTT)
            {
              if (errMQTT == MD_OK)
                {
                  errMQTTold = errMQTT;
                }
              if (errMQTT < MD_OK)
                {
                  if ((errMQTT != errMQTTold) && (errMQTT != -3))
                    {
                      SVAL(text, cerrMQTT[(-1) * errMQTT]);
                    }
                  if (errMQTT != -7) // not connected stays
                    { errMQTT = MD_OK; }
                }
            }

          void readMQTTmsg()
            {
              if (errMQTT != MD_OK) // not connected
                {
                  connectMQTT();
                }

              if (errMQTT == MD_OK) // connected
                {
                  char* ptopic = NULL;
                  while (anzMQTTmsg > 0)
                    {
                      ptopic = pMQTTRd->topic + strlen(MQTT_TOPDEV); // remove device ID
                          //S3VAL(" readMQTT pMQTTRd ptopic payload ", (uint32_t) pMQTTRd->topic, ptopic, pMQTTRd->payload);
                          //S3VAL(" readMQTT Bright  result ", topRGBBright, pMQTTRd->topic, topRGBBright.equals(pMQTTRd->topic));
                          //S3VAL(" readMQTT Color   result ", topRGBCol,    pMQTTRd->topic, topRGBCol.equals(pMQTTRd->topic));
                          //S3VAL(" readMQTT testLED result ", toptestLED,   pMQTTRd->topic, toptestLED.equals(pMQTTRd->topic));
                      #if (USE_RGBLED_PWM > OFF)
                          //if (strcmp(ptopic, topRGBBright.c_str())) // RGB LED bright
                          if (topRGBBright.equals(pMQTTRd->topic)) // RGB LED bright
                            {
                              RGBLED->bright(atoi(pMQTTRd->payload));
                              //S2VAL(" readMQTT RGBLED new bright payload ", RGBLED->bright(), pMQTTRd->payload);
                            }
                          else if (topRGBCol.equals(pMQTTRd->topic)) // RGB LED bright
                            {
                              tmpMQTT = pMQTTRd->payload;
                              sscanf(tmpMQTT.c_str(), "%x", &tmpval32);
                              RGBLED->col24(tmpval32);
                              //SHEXVAL(" readMQTT RGBLED new color  payload ", RGBLED->col24());
                            }
                          else {}
                        #endif
                      #if (USE_GEN_DIG_OUT > OFF)
                          if (toptestLED.equals(pMQTTRd->topic)) // test-led
                            {
                              if (strcmp(pMQTTRd->payload, "false") == 0)
                                { testLED = OFF;}
                              else
                                { testLED = ON; }
                              //SVAL(" readMQTT testLED new val ", testLED);
                            }
                        #endif
                      pMQTTRd = (MQTTmsg_t*) pMQTTRd->pNext;
                      anzMQTTmsg--;
                    }
                }
            }
        #endif
    // --- BME280
      #if defined(USE_BME280_I2C)
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
      #if defined(USE_INA3221_I2C)
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
      #if defined(USE_PZEM017_RS485)
          void initPZEM017()
            {
              uint8_t     i = 0;
              uint8_t _type = 0;
              while (i < NUM_PZEMS)
                {
                  S2VAL(" setup conf PZEM", i, "...");
                  sleep(2);
                  switch (i)
                    {
                      #if (NUM_PZEMS > 0)
                          case 0:
                              pzemData[i].devaddr = PZEM_1_ADDR;
                              _type               = PZEM_1_TYPE;
                            break;
                        #endif
                      #if (NUM_PZEMS > 1)
                          case 1:
                              pzemData[i].devaddr = PZEM_2_ADDR;
                              _type               = PZEM_2_TYPE;
                            break;
                        #endif
                      #if (NUM_PZEMS > 2)
                          case 2:
                              pzemData[i].devaddr = PZEM_3_ADDR;
                            break;
                        #endif
                      #if (NUM_PZEMS > 3)
                          case 3: pzemData[i].devaddr = PZEM_4_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 4)
                          case 4: pzemData[i].devaddr = PZEM_5_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 5)
                          case 5: pzemData[i].devaddr = PZEM_6_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 6)
                          case 6: pzemData[i].devaddr = PZEM_7_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 7)
                          case 7: pzemData[i].devaddr = PZEM_8_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 8)
                          case 8: pzemData[i].devaddr = PZEM_9_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 9)
                          case 9: pzemData[i].devaddr = PZEM_10_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 10)
                          case 10: pzemData[i].devaddr = PZEM_11_ADDR; break;
                        #endif
                      #if (NUM_PZEMS > 11)
                          case 11: pzemData[i].devaddr = PZEM_12_ADDR; break;
                        #endif
                      default:
                        break;
                    }
                  //pzems.config(pzemData[i].devaddr, i);
                  pzems.config(&pzemData[i], pzemData[i].devaddr );
                  if (pzemData[i].devtype != _type)
                    {
                      pzems.setShuntType(_type, &pzemData[i]);
                    }
                  pzems.config(&pzemData[i], pzemData[i].devaddr );
                  i++;
                  //S2VAL(" setup conf PZEM ready", i, "...");
                  //pzemT.startT();
                } // while i
              #if defined(PZEM_CHANGE_IDX)
                  {
                    if (pzems[PZEM_CHANGE_IDX-1].setAddress(pzemAddr[PZEM_CHANGE_IDX-1], PZEM_DEF_ADDR))
                      {
                        S3VAL("changed slave address PZEM[", PZEM_CHANGE_IDX-1, "] to ", pzemAddr[PZEM_CHANGE_IDX-1]);
                      }
                    else
                      {
                        S3VAL("could not change adress PZEM[", PZEM_CHANGE_IDX-1, "] to ", pzemAddr[PZEM_CHANGE_IDX-1]);
                      }
                  }
                #endif
            }
          void listPZEMaddr()
            {
              uint8_t _cnt  = pzems.search(PZEM_DEF_ADDR, PZEM_DEF_ADDR, true); // new device
                      _cnt += pzems.search(MIN_PZEM017,   MAX_PZEM017, true); //
                      _cnt += pzems.search(MIN_PZEM003,   MAX_PZEM003, true);
                      _cnt += pzems.search(MIN_PZEM004,   MIN_PZEM004, true);
              S2VAL("  found", _cnt, "PZEMs");
            }
        #endif
#endif // ESP32_TEST_MD_LIB

