#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>
#include <math.h>

// Weather data structure
struct WeatherData {
  float temperature;  // in Celsius
  float humidity;     // in percentage
  String condition;   // e.g., "Sunny", "Cloudy", "Rainy"
};

// Display function declarations
// Main display functions
void displayWeather(WeatherData weather);
void displayTime(time_t t);
void displayPopulation(long population);
void displaySpeed(float speed);
void resetDrawFlags(); // Reset all first draw flags when changing display modes

// Animation tracking variables
extern float lastHourAngle;
extern float lastMinuteAngle;
extern float lastSecondAngle;
extern float lastSpeedNeedleAngle;
extern float lastSpeedValue;

// Animation options
extern bool enableSmoothAnimations;

// Helper function declarations
void drawSunIcon(int x, int y);
void drawCloudIcon(int x, int y);
void drawRainIcon(int x, int y);
void drawClockHand(int centerX, int centerY, float length, float angle, int width, uint16_t color);
String formatLargeNumber(long number);
void drawSpeedometer(float speed);

// Define your ESP32 GPIOs - these should match what's in your .ino files
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_MISO 19

// Define colors
#define BACKGROUND_COLOR ILI9341_BLACK
#define TITLE_COLOR ILI9341_WHITE
#define TEXT_COLOR ILI9341_CYAN
#define WARNING_COLOR ILI9341_RED
#define CAUTION_COLOR ILI9341_YELLOW
#define SAFE_COLOR ILI9341_GREEN

// Define animation constants
#define ANIMATION_STEPS 10
#define ANIMATION_DELAY 5
#define NEEDLE_SMOOTH_FACTOR 0.2

// External reference to the display object (defined in displayFunctions.cpp)
extern Adafruit_ILI9341 tft;

// External references to first draw flags
extern bool weatherFirstDraw;
extern bool timeFirstDraw;
extern bool populationFirstDraw;
extern bool speedFirstDraw;

#endif // DISPLAY_FUNCTIONS_H