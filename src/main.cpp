#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>

// Include the display functions header
#include "displayFunctions.h"

// Global variables for storing data
WeatherData currentWeather = {22.5, 65.0, "Sunny"};
long long currentPopulation = 10000; // World population display value (reduced to fit in 32-bit unsigned long)
float currentSpeed = 75.0;           // Default speed value

// Display mode enumeration
enum DisplayMode
{
  WEATHER_DISPLAY,
  TIME_DISPLAY,
  POPULATION_DISPLAY,
  SPEED_DISPLAY
};

DisplayMode currentMode = WEATHER_DISPLAY;
unsigned long lastModeChange = 0;
const unsigned long modeChangeInterval = 10000; // 10 seconds

// trafic light

// Pin definitions for traffic lights
int ledPins[] = {19, 13, 14, 5, 26, 25};
int waitTime[] = {
    2000,
    5000,
    1000,
    2000,
    5000,
    1000,
};
// red yellow green red2 yellow2 green2
const int numPins = sizeof(ledPins) / sizeof(ledPins[0]);

// Variables for non-blocking operation
unsigned long previousMillis = 0;  // will store last time lights were updated
unsigned long currentInterval = 0; // current interval to wait
int currentStep = 0;               // current step in the traffic sequence
bool lightIsOn = false;            // track if a light is currently on

// photo resitors

// Pin configurations
const int DARKNESS_SENSOR_PIN = 33; // First photoresistor for darkness detection
const int OUTPUT_PIN = 27;          // Output pin that sends a signal when dark
const int SPEED_SENSOR_1_PIN = 34;  // First photoresistor for speed detection
const int SPEED_SENSOR_2_PIN = 35;  // Second photoresistor for speed detection

// Distance between speed sensors (in meters)
const float DISTANCE = 0.5; // Adjust this to the actual distance between your sensors

// Thresholds - adjust based on your specific sensors and lighting conditions
const int DARKNESS_THRESHOLD = 4000;     // ADC value threshold for "darkness"
const int SPEED_SENSOR_THRESHOLD = 2000; // ADC value threshold for speed sensors

// State variables for non-blocking operation
enum SpeedDetectorState
{
  WAITING_FOR_FIRST_SENSOR,
  WAITING_FOR_SECOND_SENSOR,
  COOLDOWN
};

// Structure for speed detector state management
struct SpeedDetector
{
  SpeedDetectorState state;
  unsigned long startTime;
  unsigned long cooldownEndTime;
  unsigned long cooldownDuration;
};

SpeedDetector speedDetector;

// Simple functions to update display values
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
  // Change values based on which screen is shown
  switch (currentMode)
  {
  case WEATHER_DISPLAY:
    // Cycle through different weather conditions
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
    // Cycle through different speed values
    static int speedState = 0;
    speedState = (speedState + 1) % 3;

    switch (speedState)
    {
    case 0:
      setSpeed(60.0);
      break; // Safe speed
    case 1:
      setSpeed(85.0);
      break; // Caution speed
    case 2:
      setSpeed(120.0);
      break; // Danger speed
    }
    break;

  case POPULATION_DISPLAY:
    // Increase population slightly
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

  // Check if it's time to change state
  if (currentMillis - previousMillis >= currentInterval)
  {
    previousMillis = currentMillis; // Save the time

    int i = currentStep % numPins;

    if (!lightIsOn)
    {
      // Turn on current light
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
      currentInterval = waitTime[i]; // Time to keep light on

      Serial.print("Turning ON light at pin ");
      Serial.println(ledPins[i]);
    }
    else
    {
      // Turn off current light and move to next step
      digitalWrite(ledPins[i], LOW);
      lightIsOn = false;
      currentStep++;
      currentInterval = 10; // Short delay before turning on next light

      Serial.print("Turning OFF light at pin ");
      Serial.println(ledPins[i]);
    }
  }
}

unsigned long lastReadTime = 0;
const int READ_INTERVAL = 100; // 100ms between readings

void checkDarkness()
{
  // Only take readings at specified intervals
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= READ_INTERVAL)
  {
    // Read darkness sensor
    int darknessValue = analogRead(DARKNESS_SENSOR_PIN);
    // Serial.print("Darkness: ");
    // Serial.println(darknessValue);

    // Control output pin based on darkness value
    if (darknessValue < DARKNESS_THRESHOLD)
    {
      digitalWrite(OUTPUT_PIN, HIGH); // Turn on output when dark
      Serial.print("Turning on the light on peak brightness");
    }
    else
    {
      digitalWrite(OUTPUT_PIN, LOW); // Turn off output when light
    }

    lastReadTime = currentTime; // Update the last read time
  }
}

void updateSpeedDetection()
{
  // Read speed sensors
  int speedSensor1Value = analogRead(SPEED_SENSOR_1_PIN);
  int speedSensor2Value = analogRead(SPEED_SENSOR_2_PIN);

  Serial.println("sensor 2 go brr");
  Serial.println(speedSensor2Value);
  Serial.println("sensor 1 go brr");
  Serial.println(speedSensor1Value);

  // State machine for speed detection
  switch (speedDetector.state)
  {
  case WAITING_FOR_FIRST_SENSOR:
    // Check if first sensor is triggered
    if (speedSensor1Value > SPEED_SENSOR_THRESHOLD)
    {
      speedDetector.startTime = millis();
      speedDetector.state = WAITING_FOR_SECOND_SENSOR;
      Serial.println("Object detected at first sensor");
    }
    break;

  case WAITING_FOR_SECOND_SENSOR:
    // Check if object missed the second sensor (timeout)
    if (millis() - speedDetector.startTime > 5000)
    { // 5 second timeout
      Serial.println("Object missed second sensor or timeout occurred");
      speedDetector.state = WAITING_FOR_FIRST_SENSOR;
    }
    // Check if second sensor is triggered
    else if (speedSensor2Value > SPEED_SENSOR_THRESHOLD)
    {
      unsigned long endTime = millis();
      float timeDiff = (endTime - speedDetector.startTime) / 1000.0; // Convert to seconds

      if (timeDiff > 0)
      {
        float speed = DISTANCE / timeDiff;

        Serial.print("Time: ");
        Serial.print(timeDiff, 4);
        Serial.println(" seconds");

        Serial.print("Speed: ");
        Serial.print(speed, 2);
        Serial.print(" m/s (");
        Serial.print(speed * 3.6, 2);
        Serial.println(" km/h)");
      }

      // Set cooldown period
      speedDetector.cooldownEndTime = millis() + speedDetector.cooldownDuration;
      speedDetector.state = COOLDOWN;
    }
    break;

  case COOLDOWN:
    // Wait for cooldown period to end
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

  // Initialize the display
  tft.begin();
  tft.setRotation(1); // Landscape mode
  tft.fillScreen(BACKGROUND_COLOR);

  // Set a default time
  setTime(12, 30, 0, 12, 6, 2023); // 12:30:00, June 12, 2023

  // Display welcome message
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(20, 100);
  tft.println("Display System Ready");

  // trafic light
  for (int i = 0; i < numPins; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Start with all lights off
  }

  // Initially set both red lights on
  digitalWrite(ledPins[0], HIGH); // red1
  digitalWrite(ledPins[3], HIGH); // red2

  // Initialize timing variables
  previousMillis = millis();

  // speed
  //    pinMode(DARKNESS_SENSOR_PIN, INPUT);
  pinMode(SPEED_SENSOR_1_PIN, INPUT);
  pinMode(SPEED_SENSOR_2_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);

  // Initialize speed detector state
  speedDetector.state = WAITING_FOR_FIRST_SENSOR;
  speedDetector.cooldownDuration = 1000; // 1 second cooldown

  Serial.println("ESP32 Speed detection system is running.");

  // delay(2000);
}

void loop()
{
  unsigned long currentMillis = millis();
  updateTrafficLights();
  checkDarkness();
  updateSpeedDetection();

  // Change display mode periodically
  if (currentMillis - lastModeChange > modeChangeInterval)
  {
    // Cycle through display modes
    currentMode = (DisplayMode)((currentMode + 1) % 4);
    lastModeChange = currentMillis;

    // Update display values for the new mode
    updateDisplayValues();

    // Display the current mode
    switch (currentMode)
    {
    case WEATHER_DISPLAY:
      displayWeather(currentWeather);
      break;

    case TIME_DISPLAY:
    {
      // Update the time
      int h = hour();
      int m = minute() + 1; // Advance time by 1 minute each cycle
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

  // delay(100);
}

// Additional helper functions can be added here
