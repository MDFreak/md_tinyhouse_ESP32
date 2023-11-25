# Project ESP32_test_md_stdlib 
A new base project for collecting my solutions concerning ESP32 implementation.

The project done using PlatformIO with Visual Studio Code.
Tasks, that represent testing a certain library, are implemented as an 'env' definition in fie 'platformio.ini'.   
platformio.ini containes some documentation for understanding.  

Using -D ... staments in platformio.ini it is possible to pass C/C++ #defines via commantline to the compiler.
This makes it possible to select distinquish projects and devices without editing source code.
Additional I can avoid compiling *.c/.cpp files, that are not needed.

The same system I will use for projects coming up, that have more than 1 processing units communicating within the same environment. 
This is like smart home with an common host (i.e. RaspberryPi for INET connection)

TODO:
-  Install base system
