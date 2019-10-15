# SimpleTouchScreenDSO

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Hit Counter](https://hitcounter.pythonanywhere.com/count/tag.svg?url=https%3A%2F%2Fgithub.com%2FArminJo%2FSimpleTouchScreenDSO)](https://github.com/brentvollebregt/hit-counter)

Simple DSO Software for Arduino Uno/MEGA with a Touchscreen Shield.

## No dedicated hardware, just off the shelf components + c software

OK I personally added a piezo buzzer for audio feedback of touches and 3 resistors, a capacitator and a switch to have an AC input. 
And few more resistors and a switch for an attenuator.

## Features:

 - 300 kSamples per second
 - Timebase 1-2-5 from 100 microseconds to 500 milliseconds per div (31 pixel)
 - Automatic trigger,range and offset value selection
 - Single shot mode
 - 580 Byte sample buffer
 - Min, max, average and peak-to-peak voltage display
 - Fast draw mode. Up to 45 screens per second in pixel mode
 - Switching between fast pixel and slower line display mode
 - All settings can be changed during measurement
 - All AVR ADC input channels selectable
 - Touch trigger level select
 - Touch voltage picker
 - 1.1 or 5 Volt reference selectable
 
## Hardware:

 - [Arduino Uno Rev ](http://www.watterott.com/de/Arduino-Uno)
 - [mSD-Shield](http://www.watterott.com/de/Arduino-mSD-Shield)
 - [MI0283QT-2 Adapter](http://www.watterott.com/de/MI0283QT-2-Adapter)
 
# Safety circuit and AC/DC switch
 3 resistors   2 diodes   1 capacitor   1 switch
```
                ADC INPUT PIN
                      /\
                      |
   VSS-------|<|------+-----|<|-----+-GND
                      |             |
   VREF----|R 2.2M|---+---|R 2.2M|--+
                      |
                      |
            \   o-----+
             \        |
    AC/DC     \       =  C 0.1uF
    Switch     \      |
                o-----+
                      |
                      |
                      +---|R 1K  |-----<  INPUT DIRECT
                      |
                      |
                      +---|R 2.2M|-+---<  INPUT 10 VOLT
                      +---|R 2.2M|-+
                      +------||----+
                      |  app. 80 pF (adjustable trimmer)
                      |
  ATTENUATOR          +---|R 3.3M|-+---<  INPUT 20 VOLT
                      +------||----+
                      |  app. 25 pF
                      |
                      +---|R 4.7M|-|R 4.7M|--+---<  INPUT 50 VOLT
                      |                      |
                      +-----------||---------+
                               app. 10 pF
```
![Hardware](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/Hardware.jpg)
![StartScreen](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/StartScreen.jpg)
![Settings](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/Settings.jpg)
![5kHzSine](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/5kHzSine.jpg)
![Linearity-100us / div](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/Linearity-100.JPG)
![Linearity-200us / div](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/Linearity-200.JPG)
![slowTimebase](https://github.com/ArminJo/SimpleTouchScreenDSO/blob/master/img/slowTimebase.jpg)
