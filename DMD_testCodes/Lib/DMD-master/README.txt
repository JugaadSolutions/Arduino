DOT MATRIX library
--------------


A library for driving the  512 pixel dot matrix LED display "DMD", a 32 x 16 layout.

Includes:
- High speed display connection straight to SPI port and pins.
- A full 5 x 7 pixel font set and character routines for display.
- A numerical and symbol 6 x 16 font set with a colon especially for clocks and other fun large displays.
- Clear screen with all pixels off or on.



USAGE NOTES
-----------

- Install/ Download the Arduino Software ARDUINO 1.6.4
- Place the DMD library folder into the "arduino/libraries/" folder of your Arduino installation.
- Place the TimerOne library folder into the "arduino/libraries/" folder of your Arduino installation.
- Restart the IDE.
- In the Arduino IDE, you can open File > Examples > DMD > dmd_demo and get it
  running straight away!

- USE following definition for increasing  the number panels
-following definition can connect 3 panels 
-#define DISPLAYS_ACROSS 3   //Number of panels attached in series
-#define DISPLAYS_DOWN 1     // Number of rows
