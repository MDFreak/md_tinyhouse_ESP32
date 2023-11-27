#include <Arduino.h>
#include <main.h>

#if (ESP32_TEST_MD_LIB > 0) // compiling only enabled inside own workspace
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
    int8_t          tmp_i8  = 0;
    int16_t         tmp_i16 = 0;
    int32_t         tmp_i32 = 0;
  // --- system devices
    // i2C devices
    #if defined(USE_I2C)
      // OLED
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
  // --- project devices
    // oled
      #if defined (USE_OLED_I2C)
          #define TEST_OLED
          #if (OLED_DRV == OLED_DRV_1306)
              SSD1306Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED_GEO);
            #else
              //SH1106Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED_GEO);
              SH1106Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL, (OLEDDISPLAY_GEOMETRY) OLED_GEO);
            #endif // OLED_DRV
        #endif // USE_OLED_I2C
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
      #if (USE_BME280_I2C > OFF )
          #define BME280_I2C         I2C1
        #endif // USE_BME280_I2C
      #if (USE_BME680_I2C > OFF )
          #define BME680_I2C         I2C1
        #endif // USE_BME680_I2C






  // put function declarations here:
  //int myFunction(int, int);

  #ifdef TEST_OLED
      void drawLines()
        {
          for (int16_t i = 0; i < oled.getWidth(); i += 4)
            {
              oled.drawLine(0, 0, i, oled.getHeight() - 1);
              oled.display();
              delay(10);
            }
          for (int16_t i = 0; i < oled.getHeight(); i += 4)
            {
              oled.drawLine(0, 0, oled.getWidth() - 1, i);
              oled.display();
              delay(10);
            }
          delay(250);
          oled.clear();
          for (int16_t i = 0; i < oled.getWidth(); i += 4)
            {
              oled.drawLine(0, oled.getHeight() - 1, i, 0);
              oled.display();
              delay(10);
            }
          for (int16_t i = oled.getHeight() - 1; i >= 0; i -= 4)
            {
              oled.drawLine(0, oled.getHeight() - 1, oled.getWidth() - 1, i);
              oled.display();
              delay(10);
            }
          delay(250);
          oled.clear();
          for (int16_t i = oled.getWidth() - 1; i >= 0; i -= 4)
            {
              oled.drawLine(oled.getWidth() - 1, oled.getHeight() - 1, i, 0);
              oled.display();
              delay(10);
            }
          for (int16_t i = oled.getHeight() - 1; i >= 0; i -= 4)
            {
              oled.drawLine(oled.getWidth() - 1, oled.getHeight() - 1, 0, i);
              oled.display();
              delay(10);
            }
          delay(250);
          oled.clear();
          for (int16_t i = 0; i < oled.getHeight(); i += 4)
            {
              oled.drawLine(oled.getWidth() - 1, 0, 0, i);
              oled.display();
              delay(10);
            }
          for (int16_t i = 0; i < oled.getWidth(); i += 4)
            {
              oled.drawLine(oled.getWidth() - 1, 0, i, oled.getHeight() - 1);
              oled.display();
              delay(10);
            }
          delay(250);
        }
      // Adapted from Adafruit_SSD1306
      void drawRect(void) {
        for (int16_t i = 0; i < oled.getHeight() / 2; i += 2)
          {
            oled.drawRect(i, i, oled.getWidth() - 2 * i, oled.getHeight() - 2 * i);
            oled.display();
            delay(10);
          }
      }
      // Adapted from Adafruit_SSD1306
      void fillRect(void) {
        uint8_t color = 1;
        for (int16_t i = 0; i < oled.getHeight() / 2; i += 3)
          {
            oled.setColor((color % 2 == 0) ? BLACK : WHITE); // alternate colors
            oled.fillRect(i, i, oled.getWidth() - i * 2, oled.getHeight() - i * 2);
            oled.display();
            delay(10);
            color++;
          }
        // Reset back to WHITE
        oled.setColor(WHITE);
      }
      // Adapted from Adafruit_SSD1306
      void drawCircle(void)
        {
          for (int16_t i = 0; i < oled.getHeight(); i += 2)
            {
              oled.drawCircle(oled.getWidth() / 2, oled.getHeight() / 2, i);
              oled.display();
              delay(10);
            }
          delay(1000);
          oled.clear();
          // This will draw the part of the circel in quadrant 1
          // Quadrants are numberd like this:
          //   0010 | 0001
          //  ------|-----
          //   0100 | 1000
          //
          oled.drawCircleQuads(oled.getWidth() / 2, oled.getHeight() / 2, oled.getHeight() / 4, 0b00000001);
          oled.display();
          delay(200);
          oled.drawCircleQuads(oled.getWidth() / 2, oled.getHeight() / 2, oled.getHeight() / 4, 0b00000011);
          oled.display();
          delay(200);
          oled.drawCircleQuads(oled.getWidth() / 2, oled.getHeight() / 2, oled.getHeight() / 4, 0b00000111);
          oled.display();
          delay(200);
          oled.drawCircleQuads(oled.getWidth() / 2, oled.getHeight() / 2, oled.getHeight() / 4, 0b00001111);
          oled.display();
        }
      void printBuffer(void)
        {
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
              oled.clear();
              // Print to the screen
              oled.println(test[i]);
              // Draw it to the internal screen buffer
              oled.drawLogBuffer(0, 0);
              // Display it on the screen
              oled.display();
              delay(500);
            }
        }
    #endif



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
        #if (SCAN_I2C > OFF)
            scanI2C(&i2c1, PIN_I2C1_SDA, PIN_I2C1_SCL);
          #endif
      // start oled
        #if defined (USE_OLED_I2C)
            STXT(" ... init OLED ...");
            tmp_i32 = oled.init();
            SVAL(" init returns ",tmp_i32);
            oled.flipScreenVertically();
            oled.setContrast(255);
            #ifdef TEST_OLED
                drawLines();
                delay(1000);
                oled.clear();

                drawRect();
                delay(1000);
                oled.clear();

                fillRect();
                delay(1000);
                oled.clear();

                drawCircle();
                delay(1000);
                oled.clear();

                printBuffer();
                delay(1000);
                oled.clear();
              #endif
          #endif // USE_OLED_I2C
        STXT(" ... setup finished");
    }

  void loop()
    {
      // put your main code here, to run repeatedly:
    }

#endif // ESP32_TEST_MD_LIB