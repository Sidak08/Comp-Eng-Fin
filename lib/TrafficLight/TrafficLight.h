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
  // Constructor with pin assignments for red, yellow, green
  TrafficLight(int redPin, int yellowPin, int greenPin);
  
  // Update method to be called in the loop
  void update();
  
  // Manually set a specific light
  void setLight(LightState state);
  
  // Set timing for each light state
  void setTiming(unsigned long redTime, unsigned long yellowTime, unsigned long greenTime);
  
  // Get current state
  LightState getCurrentState();
  
  // Get how long the current state has been active
  unsigned long getTimeInCurrentState();
  
private:
  int _pins[3]; // Array for the three pins (red, yellow, green)
  LightState _currentState;
  unsigned long _lastChangeTime;
  unsigned long _redTime;
  unsigned long _yellowTime;
  unsigned long _greenTime;
};

#endif // TRAFFIC_LIGHT_H