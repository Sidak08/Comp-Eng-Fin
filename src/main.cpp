#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>

// Display functions
#include "displayFunctions.h"

// Global data
WeatherData currentWeather = {19.5, 35.0, "Sunny"};
long long currentPopulation = 10000; // Population count
float currentSpeed = 75.0;           // Speed in km/h

// Display modes
enum DisplayMode
{
  WEATHER_DISPLAY,
  TIME_DISPLAY,
  POPULATION_DISPLAY,
  SPEED_DISPLAY
};

DisplayMode currentMode = WEATHER_DISPLAY;
unsigned long lastModeChange = 0;
const unsigned long modeChangeInterval = 10000; // 10s rotation

// Traffic light setup
int ledPins[] = {19, 14, 13, 5, 26, 12}; // R Y G R2 Y2 G2
int waitTime[] = {
    2000,
    5000,
    1000,
    2000,
    5000,
    1000,
};

const int numPins = sizeof(ledPins) / sizeof(ledPins[0]);

// Non-blocking variables
unsigned long prevMillis = 0;    // Last light update time
unsigned long currInterval = 0;  // Current wait interval
int currStep = 0;                // Current traffic sequence
bool lightIsOn = false;          // Light status

// Light sensors

// Pin setup
const int darknessSensorPin = 33; // Darkness detection
const int outputPin = 27;         // Dark signal output
const int speedSensor1Pin = 34;   // Speed detection 1
const int speedSensor2Pin = 35;   // Speed detection 2

// Sensor config
const float distance = 0.5;       // Meters between sensors

// Thresholds
const int darknessThreshold = 4050;    // Darkness trigger level
const int speedSensorThreshold = 2000; // Speed trigger level

// Speed detection states
enum SpeedDetectorState
{
  WAITING_FOR_FIRST_SENSOR,
  WAITING_FOR_SECOND_SENSOR,
  COOLDOWN
};

// Speed detector config
struct SpeedDetector
{
  SpeedDetectorState state;
  unsigned long startTime;
  unsigned long cooldownEndTime;
  unsigned long cooldownDuration;
};

SpeedDetector speedDetector;

// Display value setters
void setWeather(float temp, float humidity, String condition)
{
  currentWeather.temperature = temp;
  currentWeather.humidity = humidity;
  currentWeather.condition = condition;
}

void setPopulation(unsigned long population)
{
  currentPopulation = population;
}

void setSpeed(float speed)
{
  currentSpeed = speed;
}

void updateDisplayValues()
{
  // Update based on current screen
  switch (currentMode)
  {
  case WEATHER_DISPLAY:
    // Rotate weather conditions
    static int weatherState = 0;
    weatherState = (weatherState + 1) % 3;

    switch (weatherState)
    {
    case 0:
      setWeather(23.5, 67.0, "Sunny");
      break;
    case 1:
      setWeather(18.2, 82.5, "Cloudy");
      break;
    case 2:
      setWeather(15.8, 91.0, "Rain");
      break;
    }
    break;

  case SPEED_DISPLAY:
    // Rotate speed values
    static int speedState = 0;
    speedState = (speedState + 1) % 3;

    switch (speedState)
    {
    case 0:
      setSpeed(60.0);
      break; // Safe
    case 1:
      setSpeed(85.0);
      break; // Caution
    case 2:
      setSpeed(120.0);
      break; // Danger
    }
    break;

  case POPULATION_DISPLAY:
    // Increment population
    if (currentPopulation < 3900000000UL)
    {
      currentPopulation += 10000000;
    }
    else
    {
      currentPopulation = 2860000000UL;
    }
    break;
  }
}

void updateTrafficLights()
{
  unsigned long currentMillis = millis();

  // Time to change state?
  if (currentMillis - prevMillis >= currInterval)
  {
    prevMillis = currentMillis;

    int i = currStep % numPins;

    if (!lightIsOn)
    {
      // Activate light
      if (i >= 3)
      {
        digitalWrite(ledPins[0], HIGH); // red1
        digitalWrite(ledPins[3], LOW);  // red2
      }
      else
      {
        digitalWrite(ledPins[3], HIGH); // red2
        digitalWrite(ledPins[0], LOW);  // red1
      }

      digitalWrite(ledPins[i], HIGH);
      lightIsOn = true;
      currInterval = waitTime[i];

      Serial.print("Turning ON light at pin ");
      Serial.println(ledPins[i]);
    }
    else
    {
      // Deactivate light
      digitalWrite(ledPins[i], LOW);
      lightIsOn = false;
      currStep++;
      currInterval = 10; // Brief pause between changes

      Serial.print("Turning OFF light at pin ");
      Serial.println(ledPins[i]);
    }
  }
}

unsigned long lastReadTime = 0;
const int readInterval = 100; // 100ms sampling rate

void checkDarkness()
{
  // Throttle readings
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= readInterval)
  {
    // Get sensor reading
    int darknessValue = analogRead(darknessSensorPin);
    Serial.println(darknessValue);

    // Control light based on reading
    if (darknessValue < darknessThreshold)
    {
      digitalWrite(outputPin, HIGH); // Dark - light on
      Serial.print("Turning on the light on peak brightness");
    }
    else
    {
      digitalWrite(outputPin, LOW); // Bright - light off
    }

    lastReadTime = currentTime;
  }
}

void updateSpeedDetection()
{
  // Get sensor readings
  int sensor1Value = analogRead(speedSensor1Pin);
  int sensor2Value = analogRead(speedSensor2Pin);

  Serial.println("sensor 2 go brr");
  Serial.println(sensor2Value);
  Serial.println("sensor 1 go brr");
  Serial.println(sensor1Value);

  // Speed detection state machine
  switch (speedDetector.state)
  {
  case WAITING_FOR_FIRST_SENSOR:
    // First sensor check
    if (sensor1Value > speedSensorThreshold)
    {
      speedDetector.startTime = millis();
      speedDetector.state = WAITING_FOR_SECOND_SENSOR;
      Serial.println("Object detected at first sensor");
    }
    break;

  case WAITING_FOR_SECOND_SENSOR:
    // Timeout check
    if (millis() - speedDetector.startTime > 5000)
    { // 5s timeout
      Serial.println("Object missed second sensor or timeout occurred");
      speedDetector.state = WAITING_FOR_FIRST_SENSOR;
    }
    // Second sensor check
    else if (sensor2Value > speedSensorThreshold)
    {
      unsigned long endTime = millis();
      float timeDiff = (endTime - speedDetector.startTime) / 1000.0;

      if (timeDiff > 0)
      {
        float speed = distance / timeDiff;

        Serial.print("Time: ");
        Serial.print(timeDiff, 4);
        Serial.println(" seconds");

        Serial.print("Speed: ");
        Serial.print(speed, 2);
        Serial.print(" m/s (");
        Serial.print(speed * 3.6, 2);
        Serial.println(" km/h)");
      }

      // Start cooldown
      speedDetector.cooldownEndTime = millis() + speedDetector.cooldownDuration;
      speedDetector.state = COOLDOWN;
    }
    break;

  case COOLDOWN:
    // Wait for cooldown
    if (millis() >= speedDetector.cooldownEndTime)
    {
      speedDetector.state = WAITING_FOR_FIRST_SENSOR;
    }
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Display Main Program Starting");

  // Init display
  tft.begin();
  tft.setRotation(3); // Landscape
  tft.fillScreen(BACKGROUND_COLOR);
 
  // Set default time
  setTime(8, 50, 0, 16, 6, 2025);

  // Welcome message
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(20, 100);
  tft.println("Display System Ready");

  // Setup traffic lights
  for (int i = 0; i < numPins; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Start with red lights
  digitalWrite(ledPins[0], HIGH); // red1
  digitalWrite(ledPins[3], HIGH); // red2

  // Init timers
  prevMillis = millis();

  // Setup sensors
  pinMode(speedSensor1Pin, INPUT);
  pinMode(speedSensor2Pin, INPUT);
  pinMode(outputPin, OUTPUT);

  // Config speed detector
  speedDetector.state = WAITING_FOR_FIRST_SENSOR;
  speedDetector.cooldownDuration = 1000; // 1s cooldown

  Serial.println("ESP32 Speed detection system is running.");
}

void loop()
{
  unsigned long currentMillis = millis();
  updateTrafficLights();
  checkDarkness();
  updateSpeedDetection();

  // Rotate display modes
  if (currentMillis - lastModeChange > modeChangeInterval)
  {
    // Next display mode
    currentMode = (DisplayMode)((currentMode + 1) % 4);
    lastModeChange = currentMillis;

    // Update values
    updateDisplayValues();

    // Update screen
    switch (currentMode)
    {
    case WEATHER_DISPLAY:
      displayWeather(currentWeather);
      break;

    case TIME_DISPLAY:
    {
      // Increment time
      int h = hour();
      int m = minute() + 1;
      if (m >= 60)
      {
        m = 0;
        h = (h + 1) % 24;
      }
      setTime(h, m, 0, day(), month(), year());
      displayTime(now());
    }
    break;

    case POPULATION_DISPLAY:
      displayPopulation(currentPopulation);
      break;

    case SPEED_DISPLAY:
      displaySpeed(currentSpeed);
      break;
    }

    Serial.print("Changed display mode to: ");
    Serial.println(currentMode);
  }
}

// Additional helper functions can be added here
