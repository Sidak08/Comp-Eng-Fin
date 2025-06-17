#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>
#include <math.h>

// Weather info container
struct WeatherData
{
  float temperature; // celsius
  float humidity;    // percent
  String condition;  // weather state
};

// Main display functions
void displayWeather(WeatherData weather);
void displayTime(time_t t);
void displayPopulation(unsigned long population);
void displaySpeed(float speed);
void resetDrawFlags(); // Legacy function

// Drawing helpers
void drawSunIcon(int x, int y);
void drawCloudIcon(int x, int y);
void drawRainIcon(int x, int y);
void drawClockHand(int centerX, int centerY, float length, float angle, int width, uint16_t color);
String formatLargeNumber(unsigned long number);
void drawSpeedometer(float speed);

// Display pin config
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_MISO 19

// Display colors
#define BACKGROUND_COLOR ILI9341_BLACK
#define TITLE_COLOR ILI9341_WHITE
#define TEXT_COLOR ILI9341_CYAN
#define WARNING_COLOR ILI9341_RED
#define CAUTION_COLOR ILI9341_YELLOW
#define SAFE_COLOR ILI9341_GREEN

// External reference to the display object (defined in displayFunctions.cpp)
extern Adafruit_ILI9341 tft;

#endif // DISPLAY_FUNCTIONS_H
