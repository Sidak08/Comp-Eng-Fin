#include "displayFunctions.h"

// Use software-defined SPI
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// Animation tracking variables
float lastHourAngle = 0;
float lastMinuteAngle = 0;
float lastSecondAngle = 0;
float lastSpeedNeedleAngle = -60.0 * PI / 180.0; // Start at minimum speed angle
float lastSpeedValue = 0;

// Animation options
bool enableSmoothAnimations = true;

// Setup and loop functions removed as they are in main.ino

// Function to reset all the "first draw" flags when changing display modes
void resetDrawFlags() {
  // This will force a full redraw next time each display function is called
  weatherFirstDraw = true;
  timeFirstDraw = true;
  populationFirstDraw = true;
  speedFirstDraw = true;
}

/**
 * Display weather information on the screen
 * Shows temperature, humidity, and weather condition
 */
void displayWeather(WeatherData weather) {
  // Clear the screen once at the beginning of drawing
  if (weatherFirstDraw) {
    tft.fillScreen(BACKGROUND_COLOR);
    weatherFirstDraw = false;
  }
  
  // For billboard mode, only clear specific areas as needed
  
  // Display title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(20, 10);
  tft.println("Weather Info");
  
  // Display demo mode indicator if needed
  if (weather.condition.indexOf("DEMO") >= 0) {
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setCursor(tft.width() - 45, 15);
    tft.print("[DEMO]");
    // Remove DEMO from condition string
    weather.condition.replace("DEMO ", "");
  }
  
  // Draw line under title
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Display temperature
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(30, 70);
  tft.print("Temperature: ");
  tft.print(weather.temperature, 1);
  tft.print(" C");  // Changed from println to print to avoid extra newline
  
  // Display humidity
  tft.setCursor(30, 100);
  tft.print("Humidity: ");
  tft.print(weather.humidity, 1);
  tft.print(" %");  // Changed from println to print to avoid extra newline
  
  // Display condition
  tft.setCursor(30, 130);
  tft.print("Condition: ");
  tft.print(weather.condition);  // Changed from println to print to avoid extra newline
  
  // Show weather icon/graphic at the bottom of the screen
  int iconX = tft.width() / 2;
  int iconY = tft.height() - 50;
  
  // Clear the icon area first
  tft.fillRect(iconX - 60, iconY - 60, 120, 120, BACKGROUND_COLOR);
  
  if (weather.condition.indexOf("Sunny") >= 0) {
    drawSunIcon(iconX, iconY);
  } else if (weather.condition.indexOf("Cloudy") >= 0) {
    drawCloudIcon(iconX, iconY);
  } else if (weather.condition.indexOf("Rain") >= 0) {
    drawRainIcon(iconX, iconY);
  }
}

/**
 * Display current time information
 * Shows current time in digital format and an analog clock
 */
void displayTime(time_t t) {
  // For Ontario time, we'll use the demo time set in main.cpp
  // The main program now correctly sets the time to Ontario time
  
  // No time zone adjustment needed here since it's handled in main.cpp
  // This allows for proper display of the time that's already set
  
  // Clear the screen only on first draw for billboard mode
  if (timeFirstDraw) {
    tft.fillScreen(BACKGROUND_COLOR);
    timeFirstDraw = false;
  }
  
  // Display title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(65, 10);
  tft.println("Current Time");
  
  // Check if we're in demo mode by looking at year (2023 is our demo year)
  if (year(t) == 2023) {
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setCursor(tft.width() - 45, 15);
    tft.print("[DEMO]");
  }
  
  // Draw line under title
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Digital time display - move to top right with a more compact layout
  tft.setTextSize(4); // Slightly smaller text size
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(tft.width() - 140, 70); // Adjusted position to handle wider PM times
  
  // Format time with leading zeros (display only hours and minutes in billboard mode)
  char timeString[12]; // HH:MM + null terminator + AM/PM
  int hourValue = hourFormat12(t);
  if (hourValue == 0) hourValue = 12; // 0 hour should display as 12 in 12-hour format
  sprintf(timeString, "%2d:%02d", hourValue, minute(t));
  tft.print(timeString);
  
  // Display AM/PM indicator directly below time
  tft.setTextSize(2);
  tft.setCursor(tft.width() - 90, 105); // Position AM/PM indicator below time
  tft.print(isPM(t) ? "PM" : "AM");
  
  // Display date directly underneath AM/PM
  tft.setTextSize(2);
  tft.setCursor(tft.width() - 130, 130);
  
  char dateString[12]; // MM/DD/YYYY + null terminator
  sprintf(dateString, "%02d/%02d/%04d", month(t), day(t), year(t));
  tft.print(dateString);
  
  // Draw a moderately sized analog clock outline in bottom left
  int clockCenterX = 100;
  int clockCenterY = 160;
  int clockRadius = 55;
  
  // Clear only the specific clock area to avoid flicker
  tft.fillCircle(clockCenterX, clockCenterY, clockRadius + 10, BACKGROUND_COLOR);
  
  tft.drawCircle(clockCenterX, clockCenterY, clockRadius, TITLE_COLOR);
  
  // Calculate target angles in degrees
  float hourAngle = (hour(t) % 12) * 30 + minute(t) * 0.5;
  float minuteAngle = minute(t) * 6;
  float secondAngle = second(t) * 6;
  
  // For billboard mode, no animation is needed as we want immediate positions
  // when the display updates
  float targetHourAngle = hourAngle * PI / 180.0;
  float targetMinAngle = minuteAngle * PI / 180.0;
  float targetSecAngle = secondAngle * PI / 180.0;
  
  // Store angles for next time
  lastHourAngle = targetHourAngle;
  lastMinuteAngle = targetMinAngle;
  lastSecondAngle = targetSecAngle;
  
  // Direct angles with no animation for billboard mode
  hourAngle = targetHourAngle * 180.0 / PI;
  minuteAngle = targetMinAngle * 180.0 / PI;
  secondAngle = targetSecAngle * 180.0 / PI;
  
  // Draw clock hands
  drawClockHand(clockCenterX, clockCenterY, clockRadius * 0.6, hourAngle, 3, ILI9341_WHITE); // Hour hand
  drawClockHand(clockCenterX, clockCenterY, clockRadius * 0.8, minuteAngle, 2, ILI9341_WHITE); // Minute hand
  drawClockHand(clockCenterX, clockCenterY, clockRadius * 0.9, secondAngle, 1, ILI9341_RED); // Second hand
}

/**
 * Display world or specified area population
 * Shows total population and growth indicators
 */
void displayPopulation(long population) {
  // Clear the screen only on first draw for billboard mode
  if (populationFirstDraw) {
    tft.fillScreen(BACKGROUND_COLOR);
    populationFirstDraw = false;
  }
  
  // Display title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(40, 10);
  tft.println("Population");
  
  // Show demo indicator since population is always demo data
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(tft.width() - 45, 15);
  tft.print("[DEMO]");
  
  // Draw line under title
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Format population with commas
  String popStr = formatLargeNumber(population);
  
  // Display total population
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(20, 70);
  tft.println("World Population:");
  
  // Clear previous population text area
  tft.fillRect(0, 100, tft.width(), 30, BACKGROUND_COLOR);
  
  // Clear previous population text area and display updated population
  tft.fillRect(0, 100, tft.width(), 30, BACKGROUND_COLOR);
  tft.setTextSize(3);
  tft.setCursor((tft.width() - popStr.length() * 18) / 2, 100);
  tft.print(popStr);
  
  // Display growth statistics
  tft.setTextSize(2);
  tft.setCursor(20, 150);
  tft.print("Growth Rate: +1.1% per year");
  
  // Draw a simple graph showing population growth
  int graphX = 50;
  int graphY = 190;
  int graphWidth = tft.width() - 100;
  int graphHeight = 30;
  
  // Clear the graph area first
  tft.fillRect(graphX, graphY, graphWidth, graphHeight, BACKGROUND_COLOR);
  
  tft.drawRect(graphX, graphY, graphWidth, graphHeight, TEXT_COLOR);
  tft.fillRect(graphX, graphY, graphWidth * 0.8, graphHeight, ILI9341_BLUE); // 80% filled
}

/**
 * Display current speed with warning indicators
 * Shows different colors based on the speed threshold
 */
void displaySpeed(float speed) {
  // Clear the screen only on first draw for billboard mode
  if (speedFirstDraw) {
    tft.fillScreen(BACKGROUND_COLOR);
    speedFirstDraw = false;
  } else {
    // Clear specific areas that will be updated with new content
    tft.fillRect(0, 80, tft.width(), 50, BACKGROUND_COLOR);  // Speed value area
    tft.fillRect(0, 130, tft.width(), 40, BACKGROUND_COLOR); // Warning text area
    
    // Also clear speedometer area to prevent ghosting
      int centerX = tft.width() / 2;
      int centerY = 210;  // Moved down to match drawing position
      int radius = 45;  // Reduced radius for speedometer
      tft.fillRect(centerX - radius - 20, centerY - radius - 20, 
                 (radius + 20) * 2, (radius + 20) * 2, BACKGROUND_COLOR);
  }
  
  uint16_t speedColor;
  String warningText;
  
  // Determine color and warning based on speed
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
  
  // Display title
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(40, 10);
  tft.println("Speed Monitor");
  
  // Always show demo indicator since speed is simulated
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(tft.width() - 45, 15);
  tft.print("[DEMO]");
  
  // Draw line under title
  tft.drawLine(20, 50, tft.width() - 20, 50, TITLE_COLOR);
  
  // Speed display area is already cleared above
  
  // Display the speed in large text with appropriate color
  tft.setTextSize(5);
  tft.setTextColor(speedColor);
  tft.setCursor(60, 80);
  tft.print(speed, 1);
  
  // Show speed unit
  tft.setTextSize(3);
  tft.setCursor(220, 95);
  tft.print("km/h");
  
  // Display warning message (ensure area is cleared first)
  tft.setTextSize(2);
  tft.setTextColor(speedColor);
  
  // Center the text properly
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(warningText, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, 140);
  tft.print(warningText);
  
  // Draw a speedometer graphic (moved down)
  drawSpeedometer(speed);
}

// Helper functions for drawing graphics

void drawSunIcon(int x, int y) {
  int radius = 30;
  tft.fillCircle(x, y, radius, ILI9341_YELLOW);
  
  // Sun rays
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
  // Make cloud larger for bottom display
  
  // Main cloud body
  tft.fillRoundRect(x - 30, y, 90, 45, 20, ILI9341_WHITE);
  
  // Cloud puffs
  tft.fillCircle(x - 15, y, 30, ILI9341_WHITE);
  tft.fillCircle(x + 20, y - 10, 35, ILI9341_WHITE);
  tft.fillCircle(x + 50, y + 5, 25, ILI9341_WHITE);
}

void drawRainIcon(int x, int y) {
  // Draw cloud first
  drawCloudIcon(x, y - 15);
  
  // Add more raindrops
  for (int i = 0; i < 7; i++) {
    int dropX = x - 25 + i * 15;
    int dropY = y + 35;
    tft.fillRoundRect(dropX, dropY, 4, 15, 2, ILI9341_BLUE);
    dropY += 20;
    tft.fillRoundRect(dropX + 8, dropY, 4, 15, 2, ILI9341_BLUE);
  }
}

void drawClockHand(int centerX, int centerY, float length, float angle, int width, uint16_t color) {
  // Convert angle from degrees to radians
  float radians = (angle - 90) * PI / 180.0;
  
  int endX = centerX + length * cos(radians);
  int endY = centerY + length * sin(radians);
  
  // Draw the hand as a filled triangle for better appearance and to avoid gaps
  int xOffset = sin(radians) * (width / 2);
  int yOffset = -cos(radians) * (width / 2);
  
  // Draw a filled triangle instead of lines
  int x1 = centerX + xOffset;
  int y1 = centerY + yOffset;
  int x2 = centerX - xOffset;
  int y2 = centerY - yOffset;
  
  tft.fillTriangle(x1, y1, x2, y2, endX, endY, color);
  
  // For very thin hands (like second hand), also draw a line
  if (width <= 1) {
    tft.drawLine(centerX, centerY, endX, endY, color);
  }
  
  // Draw a small circle at the center of the clock hand
  tft.fillCircle(centerX, centerY, width + 1, color);
}

String formatLargeNumber(long number) {
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
  int centerY = 210;  // Moved down by 10 pixels
  int radius = 45;  // Smaller speedometer radius
  
  // No need to clear here as it's already done in displaySpeed function
  
  // Draw speedometer arc
  tft.drawCircle(centerX, centerY, radius, TITLE_COLOR);
  
  // Draw speed ranges on the arc (green, yellow, red)
  for (int i = -60; i <= 60; i += 5) {
    float angle = i * PI / 180.0;
    int x1 = centerX + (radius - 2) * cos(angle);
    int y1 = centerY + (radius - 2) * sin(angle);
    int x2 = centerX + radius * cos(angle);
    int y2 = centerY + radius * sin(angle);
    
    // Color code the ticks
    uint16_t tickColor;
    if (i < -20) tickColor = SAFE_COLOR;
    else if (i < 20) tickColor = CAUTION_COLOR;
    else tickColor = WARNING_COLOR;
    
    tft.drawLine(x1, y1, x2, y2, tickColor);
  }
  
  // Convert speed to an angle for the needle
  // Map 0-160 km/h to -60 to +60 degrees
  float targetSpeedAngle = map(speed, 0, 160, -60, 60) * PI / 180.0;
  float speedAngle = targetSpeedAngle;
  
  // In billboard mode, we want immediate needle position rather than animation
  // since the display only updates when there's a significant change
  speedAngle = targetSpeedAngle;
  lastSpeedNeedleAngle = targetSpeedAngle;
  lastSpeedValue = speed;
  
  // Draw the needle as a triangle for better visibility
  int needleLength = radius - 5;
  int needleWidth = 3;
  int endX = centerX + needleLength * cos(speedAngle);
  int endY = centerY + needleLength * sin(speedAngle);
  
  // Calculate triangle points for the needle
  float perpAngle = speedAngle + PI/2;
  int x1 = centerX + needleWidth * cos(perpAngle);
  int y1 = centerY + needleWidth * sin(perpAngle);
  int x2 = centerX - needleWidth * cos(perpAngle);
  int y2 = centerY - needleWidth * sin(perpAngle);
  
  tft.fillTriangle(x1, y1, x2, y2, endX, endY, ILI9341_WHITE);
  
  // Draw the center hub of the speedometer
  tft.fillCircle(centerX, centerY, 6, ILI9341_WHITE);
  tft.fillCircle(centerX, centerY, 4, ILI9341_RED);
}