#ifndef _MAIN_H_
  #define _MAIN_H_

  // standard system includes
    #include <Arduino.h>
    #include <unity.h>
    #include <string.h>
    #include <stdio.h>                                                        // Biblioteca STDIO
    #include <iostream>
  // project system includes
    #if (USE_I2C > OFF)
        #include <Wire.h>
      #endif // USE_I2C
  // standard project includes
    #include <prj_conf_test_lib_oled.h>
  // standard md_library
    #include <md_defines.h>
    #include <md_util.h>
    #include <md_filter.hpp>
    //#include <md_time.hpp>
  // project devices md_library
    // OLED
      #ifdef USE_I2C
          #if (OLED_DRV == OLED_DRV_1306)
              #include <SSD1306Wire.h>
            #endif // OLED_DRV
          #if (OLED_DRV == OLED_DRV_1106)
              #include <SH1106Wire.h>
            #endif // OLED_DRV
        #endif // USE_OLED_I2C

#endif // _MAIN_H_
