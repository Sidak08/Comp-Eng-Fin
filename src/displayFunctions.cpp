#include "displayFunctions.h"

// Software-defined SPI
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// Animation tracking
float lastHourAngle = 0;
float lastMinuteAngle = 0;
float lastSecondAngle = 0;
float lastSpeedNeedleAngle = -60.0 * PI / 180.0; // Min speed position
float lastSpeedValue = 0;

// Animation config
bool enableSmoothAnimations = true;

// Legacy function - kept for compatibility
void resetDrawFlags() {
  // Retained for API compatibility
}

// Shows current weather with temp, humidity and condition
void displayWeather(WeatherData weather) {
  // Clear screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Draw title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(20, 10);
  tft.println("Weather Info");
  
  // Add demo tag if needed
  if (weather.condition.indexOf("DEMO") >= 0) {
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setCursor(tft.width() - 45, 15);
    tft.print("[DEMO]");
    weather.condition.replace("DEMO ", "");
  }
  
  // Title underline
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Temp reading
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(30, 70);
  tft.print("Temperature: ");
  tft.print(weather.temperature, 1);
  tft.print(" C");
  
  // Humidity reading
  tft.setCursor(30, 100);
  tft.print("Humidity: ");
  tft.print(weather.humidity, 1);
  tft.print(" %");
  
  // Weather condition
  tft.setCursor(30, 130);
  tft.print("Condition: ");
  tft.print(weather.condition);
  
  // Weather icon
  int iconX = tft.width() / 2;
  int iconY = tft.height() - 50;
  
  if (weather.condition.indexOf("Sunny") >= 0) {
    drawSunIcon(iconX, iconY);
  } else if (weather.condition.indexOf("Cloudy") >= 0) {
    drawCloudIcon(iconX, iconY);
  } else if (weather.condition.indexOf("Rain") >= 0) {
    drawRainIcon(iconX, iconY);
  }
}

// Shows digital time and analog clock
void displayTime(time_t t) {
  // Clear screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Draw title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(65, 10);
  tft.println("Current Time");
  
  // Add demo tag if needed
  if (year(t) == 2023) {
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setCursor(tft.width() - 45, 15);
    tft.print("[DEMO]");
  }
  
  // Title underline
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Clear time area
  tft.fillRect(tft.width() - 170, 60, 170, 140, BACKGROUND_COLOR);
  
  // Digital clock
  tft.setTextSize(4);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(tft.width() - 140, 70);
  
  // 12-hour format time
  int hourValue = hourFormat12(t);
  if (hourValue == 0) hourValue = 12;
  char timeString[12];
  sprintf(timeString, "%2d:%02d", hourValue, minute(t));
  tft.print(timeString);
  
  // AM/PM indicator
  tft.setTextSize(2);
  tft.setCursor(tft.width() - 140, 105);
  tft.print(isPM(t) ? "PM" : "AM");
  
  // Calendar date
  tft.setTextSize(2);
  tft.setCursor(tft.width() - 140, 130);
  char dateString[12];
  sprintf(dateString, "%02d/%02d/%04d", month(t), day(t), year(t));
  tft.print(dateString);
  
  // Analog clock
  int clockCenterX = 100;
  int clockCenterY = 160;
  int clockRadius = 55;
  
  tft.drawCircle(clockCenterX, clockCenterY, clockRadius, TITLE_COLOR);
  
  // Hand angles
  float hourAngle = (hour(t) % 12) * 30 + minute(t) * 0.5;
  float minuteAngle = minute(t) * 6;
  float secondAngle = second(t) * 6;
  
  // Clock hands
  drawClockHand(clockCenterX, clockCenterY, clockRadius * 0.6, hourAngle, 3, ILI9341_WHITE);
  drawClockHand(clockCenterX, clockCenterY, clockRadius * 0.8, minuteAngle, 2, ILI9341_WHITE);
  drawClockHand(clockCenterX, clockCenterY, clockRadius * 0.9, secondAngle, 1, ILI9341_RED);
}

// Shows population stats with growth indicators
void displayPopulation(unsigned long population) {
  // Clear screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Draw title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(40, 10);
  tft.println("Population");
  
  // Add demo tag
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(tft.width() - 45, 15);
  tft.print("[DEMO]");
  
  // Title underline
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Format with commas
  String popStr = formatLargeNumber(population);
  
  // Population label
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(20, 70);
  tft.println("World Population:");
  
  // Population count
  tft.setTextSize(3);
  tft.setCursor((tft.width() - popStr.length() * 18) / 2, 100);
  tft.print(popStr);
  
  // Growth stats
  tft.setTextSize(2);
  tft.setCursor(20, 150);
  tft.print("Growth Rate: +1.1% per year");
  
  // Progress bar
  int graphX = 50;
  int graphY = 190;
  int graphWidth = tft.width() - 100;
  int graphHeight = 30;
  
  tft.drawRect(graphX, graphY, graphWidth, graphHeight, TEXT_COLOR);
  tft.fillRect(graphX, graphY, graphWidth * 0.8, graphHeight, ILI9341_BLUE);
}

// Shows speed with color-coded warnings
void displaySpeed(float speed) {
  // Clear screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Set warning level
  uint16_t speedColor;
  String warningText;
  
  if (speed > 100) {
    speedColor = WARNING_COLOR;
    warningText = "DANGER: EXCESSIVE SPEED";
  } else if (speed > 80) {
    speedColor = CAUTION_COLOR;
    warningText = "CAUTION: High Speed";
  } else {
    speedColor = SAFE_COLOR;
    warningText = "Speed OK";
  }
  
  // Draw title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(40, 10);
  tft.println("Speed Monitor");
  
  // Add demo tag
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(tft.width() - 45, 15);
  tft.print("[DEMO]");
  
  // Title underline
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Speed value
  tft.setTextSize(5);
  tft.setTextColor(speedColor);
  tft.setCursor(60, 80);
  tft.print(speed, 1);
  
  // Speed unit
  tft.setTextSize(3);
  tft.setCursor(220, 95);
  tft.print("km/h");
  
  // Warning message
  tft.setTextSize(2);
  tft.setTextColor(speedColor);
  
  // Center text
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(warningText, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, 140);
  tft.print(warningText);
  
  // Speedometer
  drawSpeedometer(speed);
}

// Graphics helpers

void drawSunIcon(int x, int y) {
  int radius = 30;
  tft.fillCircle(x, y, radius, ILI9341_YELLOW);
  
  // Rays
  for (int i = 0; i < 12; i++) {
    float angle = i * 30 * PI / 180;
    int startX = x + (radius + 5) * cos(angle);
    int startY = y + (radius + 5) * sin(angle);
    int endX = x + (radius + 20) * cos(angle);
    int endY = y + (radius + 20) * sin(angle);
    tft.drawLine(startX, startY, endX, endY, ILI9341_YELLOW);
  }
}

void drawCloudIcon(int x, int y) {
  // Cloud body
  tft.fillRoundRect(x - 30, y, 90, 45, 20, ILI9341_WHITE);
  
  // Puffs
  tft.fillCircle(x - 15, y, 30, ILI9341_WHITE);
  tft.fillCircle(x + 20, y - 10, 35, ILI9341_WHITE);
  tft.fillCircle(x + 50, y + 5, 25, ILI9341_WHITE);
}

void drawRainIcon(int x, int y) {
  // Cloud
  drawCloudIcon(x, y - 15);
  
  // Raindrops
  for (int i = 0; i < 7; i++) {
    int dropX = x - 25 + i * 15;
    int dropY = y + 35;
    tft.fillRoundRect(dropX, dropY, 4, 15, 2, ILI9341_BLUE);
    dropY += 20;
    tft.fillRoundRect(dropX + 8, dropY, 4, 15, 2, ILI9341_BLUE);
  }
}

void drawClockHand(int centerX, int centerY, float length, float angle, int width, uint16_t color) {
  // Convert to radians
  float radians = (angle - 90) * PI / 180.0;
  
  int endX = centerX + length * cos(radians);
  int endY = centerY + length * sin(radians);
  
  // Triangle coords
  int xOffset = sin(radians) * (width / 2);
  int yOffset = -cos(radians) * (width / 2);
  
  // Draw as triangle
  int x1 = centerX + xOffset;
  int y1 = centerY + yOffset;
  int x2 = centerX - xOffset;
  int y2 = centerY - yOffset;
  
  tft.fillTriangle(x1, y1, x2, y2, endX, endY, color);
  
  // Extra line for thin hands
  if (width <= 1) {
    tft.drawLine(centerX, centerY, endX, endY, color);
  }
  
  // Center hub
  tft.fillCircle(centerX, centerY, width + 1, color);
}

String formatLargeNumber(unsigned long number) {
  String result = "";
  String numStr = String(number);
  
  int len = numStr.length();
  int commaPos = len % 3;
  if (commaPos == 0) commaPos = 3;
  
  for (int i = 0; i < len; i++) {
    result += numStr[i];
    if ((i + 1) == commaPos && (i + 1) < len) {
      result += ",";
      commaPos += 3;
    }
  }
  
  return result;
}

void drawSpeedometer(float speed) {
  int centerX = tft.width() / 2;
  int centerY = 210;
  int radius = 45;
  
  // Draw arc
  tft.drawCircle(centerX, centerY, radius, TITLE_COLOR);
  
  // Speed range ticks
  for (int i = -60; i <= 60; i += 5) {
    float angle = i * PI / 180.0;
    int x1 = centerX + (radius - 2) * cos(angle);
    int y1 = centerY + (radius - 2) * sin(angle);
    int x2 = centerX + radius * cos(angle);
    int y2 = centerY + radius * sin(angle);
    
    // Color zones
    uint16_t tickColor;
    if (i < -20) tickColor = SAFE_COLOR;
    else if (i < 20) tickColor = CAUTION_COLOR;
    else tickColor = WARNING_COLOR;
    
    tft.drawLine(x1, y1, x2, y2, tickColor);
  }
  
  // Map speed to angle
  float targetAngle = map(speed, 0, 160, -60, 60) * PI / 180.0;
  float speedAngle = targetAngle;
  
  // Set position directly
  speedAngle = targetAngle;
  lastSpeedNeedleAngle = targetAngle;
  lastSpeedValue = speed;
  
  // Draw needle
  int needleLength = radius - 5;
  int needleWidth = 3;
  int endX = centerX + needleLength * cos(speedAngle);
  int endY = centerY + needleLength * sin(speedAngle);
  
  // Needle points
  float perpAngle = speedAngle + PI/2;
  int x1 = centerX + needleWidth * cos(perpAngle);
  int y1 = centerY + needleWidth * sin(perpAngle);
  int x2 = centerX - needleWidth * cos(perpAngle);
  int y2 = centerY - needleWidth * sin(perpAngle);
  
  tft.fillTriangle(x1, y1, x2, y2, endX, endY, ILI9341_WHITE);
  
  // Draw hub
  tft.fillCircle(centerX, centerY, 6, ILI9341_WHITE);
  tft.fillCircle(centerX, centerY, 4, ILI9341_RED);
}