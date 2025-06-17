#include "TrafficLight.h"

// TrafficLight implementation

TrafficLight::TrafficLight(int redPin, int yellowPin, int greenPin) {
  _pins[0] = redPin;
  _pins[1] = yellowPin;
  _pins[2] = greenPin;
  
  for (int i = 0; i < 3; i++) {
    pinMode(_pins[i], OUTPUT);
    digitalWrite(_pins[i], LOW);
  }
  
  _currentState = RED;
  digitalWrite(_pins[0], HIGH); // Start with red
  
  _lastChangeTime = millis();
  _redTime = 5000;
  _yellowTime = 2000;
  _greenTime = 5000;
}

void TrafficLight::update() {
  unsigned long currTime = millis();
  unsigned long elapsedTime = currTime - _lastChangeTime;
  
  switch (_currentState) {
    case RED:
      if (elapsedTime >= _redTime) {
        setLight(GREEN);
      }
      break;
      
    case GREEN:
      if (elapsedTime >= _greenTime) {
        setLight(YELLOW);
      }
      break;
      
    case YELLOW:
      if (elapsedTime >= _yellowTime) {
        setLight(RED);
      }
      break;
  }
}

void TrafficLight::setLight(LightState state) {
  // Reset lights
  for (int i = 0; i < 3; i++) {
    digitalWrite(_pins[i], LOW);
  }
  
  // Activate selected light
  switch (state) {
    case RED:
      digitalWrite(_pins[0], HIGH);
      break;
    case YELLOW:
      digitalWrite(_pins[1], HIGH);
      break;
    case GREEN:
      digitalWrite(_pins[2], HIGH);
      break;
  }
  
  _currentState = state;
  _lastChangeTime = millis();
}

void TrafficLight::setTiming(unsigned long redTime, unsigned long yellowTime, unsigned long greenTime) {
  _redTime = redTime;
  _yellowTime = yellowTime;
  _greenTime = greenTime;
}

LightState TrafficLight::getCurrentState() {
  return _currentState;
}

unsigned long TrafficLight::getTimeInCurrentState() {
  return millis() - _lastChangeTime;
}