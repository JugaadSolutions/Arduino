
#include "PB.h"


PushButton::PushButton(unsigned long debounceCount, unsigned char pin)
{
	interval(debounceCount);
	previous_millis = millis();
	newState = digitalRead(pin);
	previousState = newState;
	inputState = true;
    this->pin = pin;
}

void PushButton::write(int new_state)
{
	inputState = new_state;
	digitalWrite(pin,inputState);
}

void PushButton::interval(unsigned long debounceCount)
{
  this->debounceCount = debounceCount;
}

int PushButton::update()
{
	newState = digitalRead(pin);
	
	if(previousState == newState)
		return 0;
	if (previousState != newState && keyPressed == false ) {
		keyPressed = true;
		previous_millis = millis();
	}
	
	if( keyPressed == true )
	{
		if (millis() - previous_millis >= debounceCount) {
  			previous_millis = millis();
  			inputState = digitalRead(pin);
			previousState = newState;
			keyPressed = false;
  			return 1;
		}
  }
  
//  return 1;

}

bool PushButton::read()
{
	bool _state = inputState;
	
	if(this->inputState == false)
		this->inputState = true;
	return _state;
}


