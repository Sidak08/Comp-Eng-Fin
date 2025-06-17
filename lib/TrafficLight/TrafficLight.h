#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <Arduino.h>

// Light states
enum LightState {
  RED,
  YELLOW,
  GREEN
};

class TrafficLight {
public:
  // Setup with RGB pins
  TrafficLight(int redPin, int yellowPin, int greenPin);
  
  // Call this in loop
  void update();
  
  // Change light manually
  void setLight(LightState state);
  
  // Set duration for each color
  void setTiming(unsigned long redTime, unsigned long yellowTime, unsigned long greenTime);
  
  // Get active light
  LightState getCurrentState();
  
  // Time in current state
  unsigned long getTimeInCurrentState();
  
private:
  int _pins[3]; // RGB pin array
  LightState _currentState;
  unsigned long _lastChangeTime;
  unsigned long _redTime;
  unsigned long _yellowTime;
  unsigned long _greenTime;
};

#endif // TRAFFIC_LIGHT_H