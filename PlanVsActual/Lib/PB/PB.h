

#ifndef Debounce_h
#define Debounce_h

#include "Arduino.h"

class PushButton
{

public:
  PushButton(unsigned long interval_millis, unsigned char pin);
  void interval(unsigned long interval_millis);
  int update();
  bool read();
  void write(int new_state);
  
private:
  unsigned long  previous_millis, debounceCount;
  bool inputState;
  bool newState;
  bool previousState;
  bool keyPressed;
  unsigned char pin;
};

#endif


