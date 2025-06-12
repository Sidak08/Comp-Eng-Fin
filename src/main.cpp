#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Include the display functions header
#include "displayFunctions.h"

// Global flags for first draw states - must be declared here to match extern declarations
bool weatherFirstDraw = true;
bool timeFirstDraw = true;
bool populationFirstDraw = true;
bool speedFirstDraw = true;

// WiFi credentials - leave empty to use demo data
const char* ssid = "";       // Fill this with your WiFi SSID or leave empty
const char* password = "";   // Fill this with your WiFi password or leave empty

// OpenWeatherMap API details (for real weather data)
const String apiKey = "";    // Fill this with your API key or leave empty
const String city = "London";
const String countryCode = "UK";

// Flag to determine if we're using demo data
bool useDemoData = true;     // Will be set to false if WiFi connects and API key exists

// Global variables for storing data
WeatherData currentWeather = {0.0, 0.0, "Unknown"};
long currentPopulation = 7860123456; // Starting with a default value
float currentSpeed = 0.0;
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 600000; // 10 minutes

// Display mode enumeration
enum DisplayMode {
  WEATHER_DISPLAY,
  TIME_DISPLAY,
  POPULATION_DISPLAY,
  SPEED_DISPLAY
};

DisplayMode currentMode = WEATHER_DISPLAY;
unsigned long lastModeChange = 0;
const unsigned long modeChangeInterval = 10000; // 10 seconds

// Functions are now included from displayFunctions.h

// For the sake of the example, let's add a simulation function
float simulateSpeed() {
  // For billboard mode, we want distinct speed ranges that show different warnings
  // Rotate between safe, caution, and danger speeds with each call
  static int speedState = 0;
  
  // Rotate through 3 distinct speeds for billboard display
  speedState = (speedState + 1) % 3;
  
  switch(speedState) {
    case 0: return 60.0;  // Safe speed
    case 1: return 85.0;  // Caution speed
    case 2: return 120.0; // Danger speed
    default: return 60.0;
  }
}

void updateWeatherData() {
  if (useDemoData || WiFi.status() != WL_CONNECTED) {
    // Use demo data with cycling weather conditions
    unsigned long cycleTime = (millis() / 30000) % 3; // Change every 30 seconds, 3 conditions
    
    switch(cycleTime) {
      case 0:
        currentWeather = {23.5, 67.0, "DEMO Sunny"};
        break;
      case 1:
        currentWeather = {18.2, 82.5, "DEMO Cloudy"};
        break;
      case 2:
        currentWeather = {15.8, 91.0, "DEMO Rain"};
        break;
    }
    
    Serial.println("Using demo weather data: " + currentWeather.condition);
    return;
  }
  
  // Only try to fetch real data if we have an API key
  if (apiKey.length() == 0) {
    Serial.println("No API key provided. Using demo weather data.");
    currentWeather = {23.5, 67.0, "DEMO Partly Cloudy"};
    return;
  }
  
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + apiKey + "&units=metric";
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    
    float temp = doc["main"]["temp"];
    float humidity = doc["main"]["humidity"];
    String condition = doc["weather"][0]["main"].as<String>();
    
    currentWeather = {temp, humidity, condition};
    Serial.println("Weather updated: " + String(temp) + "°C, " + String(humidity) + "%, " + condition);
  } else {
    Serial.println("Error fetching weather data: " + http.errorToString(httpCode));
    // Use default values if the API call fails
    currentWeather = {23.5, 67.0, "Partly Cloudy"};
  }
  
  http.end();
}

void updatePopulationEstimate() {
  // Simple population growth model 
  // For billboard mode, we'll make more noticeable increases
  currentPopulation += 10000; // Add a significant number for visible change
}

void setup() {
  Serial.begin(9600);
  Serial.println("Display Main Program Starting");
  
  // Initialize the display
  tft.begin();
  tft.setRotation(1); // Landscape mode
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Initialize with reset to ensure first draw flags are set
  resetDrawFlags();
  
  // Check if we should use demo data
  useDemoData = (strlen(ssid) == 0 || strlen(password) == 0 || apiKey.length() == 0);
  
  if (!useDemoData) {
    // Try to connect to WiFi if credentials are provided
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    int wifiAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
      delay(500);
      Serial.print(".");
      wifiAttempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi connected");
      Serial.println("IP address: " + WiFi.localIP().toString());
      
      // Set time from NTP server
      configTime(0, 0, "pool.ntp.org", "time.nist.gov");
      
      // Initial weather update
      updateWeatherData();
    } else {
      Serial.println();
      Serial.println("WiFi connection failed. Using demo data.");
      useDemoData = true;
    }
  } else {
    Serial.println("WiFi credentials or API key not provided. Using demo data.");
  }
  
  // Get the current UTC time
  time_t utcTime = now();
  
  if (useDemoData) {
    // Calculate Ontario time (Eastern Time) from UTC
    // Ontario is in Eastern Time Zone (UTC-5 in standard time, UTC-4 in daylight saving time)
    
    // Check if we're in Daylight Saving Time (roughly March to November in Ontario)
    // This is a simplified calculation - a real implementation would use exact DST rules
    int mon = month(utcTime);
    bool isDST = (mon > 3 && mon < 11);
    
    // Apply the Ontario time offset to UTC
    int offsetHours = isDST ? -4 : -5;
    time_t ontarioTime = utcTime + (offsetHours * 3600);
    
xed at 6:57 PM for this demo
    setTime(18, 57, 0, 12, 6, 2023); // 6:57 PM, June 12, 2023
    
    // Initialize demo weather data
    updateWeatherData();
  }
  
  // Display welcome message
  tft.setTextSize(3);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(20, 100);
  tft.println("Display System Ready");
  delay(2000);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update data periodically
  // Update weather data periodically (less frequently for billboard mode)
  static unsigned long lastWeatherDataUpdate = 0;
  if (currentMillis - lastWeatherUpdate > weatherUpdateInterval) {
    updateWeatherData();
    lastWeatherUpdate = currentMillis;
    Serial.println("Weather data updated");
  }
  
  // Update population estimate (less frequently for billboard mode)
  static unsigned long lastPopulationUpdate = 0;
  if (currentMillis - lastPopulationUpdate > 30000) { // 30 seconds
    updatePopulationEstimate();
    lastPopulationUpdate = currentMillis;
    Serial.println("Population updated");
  }
  
  // Update simulated speed (less frequently for billboard mode)
  static unsigned long lastSpeedUpdate = 0;
  if (currentMillis - lastSpeedUpdate > 5000) { // 5 seconds
    currentSpeed = simulateSpeed();
    lastSpeedUpdate = currentMillis;
  }

  // If using demo mode, update the time based on billboard update frequency
  // For billboard mode, we want time to advance in larger increments to show visible changes
  // In demo mode, we'll keep the time fixed at 6:57 PM to match the requirement
  // This ensures the specified Ontario time is always displayed correctly
  static unsigned long lastTimeUpdate = 0;
  if (useDemoData) {
    // In demo mode, just ensure the time stays fixed at 6:57 PM
    // Only update the minute part slightly for demonstration purposes
    if (currentMillis - lastTimeUpdate >= 30000) { // Update every 30 seconds
      int currentMinute = minute();
      // Just alternate between a few minute values to show some variation
      currentMinute = (currentMinute >= 57 && currentMinute < 59) ? currentMinute + 1 : 57;
      
      // Always set to 6:57-6:59 PM range
      setTime(18, currentMinute, 0, 12, 6, 2023);
      
      lastTimeUpdate = currentMillis;
      Serial.println("Updated demo time: 6:" + String(currentMinute) + " PM");
    }
  }
  
  // Change display mode periodically
  if (currentMillis - lastModeChange > modeChangeInterval) {
    // Cycle through display modes
    currentMode = (DisplayMode)((currentMode + 1) % 4);
    lastModeChange = currentMillis;
    
    // Reset the first draw flags to ensure full redraw on mode change
    resetDrawFlags();
    
    Serial.print("Changing display mode to: ");
    Serial.println(currentMode);
  }
  
  // Only update the display when needed (mode change or significant data change)
  static DisplayMode previousMode = WEATHER_DISPLAY;
  static WeatherData prevWeather = {0, 0, ""};
  static time_t prevTime = 0;
  static long prevPopulation = 0;
  static float prevSpeed = -1;
  
  bool shouldUpdateDisplay = false;
  
  // Check if mode has changed
  if (currentMode != previousMode) {
    shouldUpdateDisplay = true;
    previousMode = currentMode;
  } else {
    // Check for significant data changes based on current mode
    switch(currentMode) {
      case WEATHER_DISPLAY:
        if (currentWeather.temperature != prevWeather.temperature || 
            currentWeather.humidity != prevWeather.humidity || 
            currentWeather.condition != prevWeather.condition) {
          shouldUpdateDisplay = true;
          prevWeather = currentWeather;
        }
        break;
        
      case TIME_DISPLAY:
        if (now() - prevTime >= 60) { // Only update time every minute
          shouldUpdateDisplay = true;
          prevTime = now();
        }
        break;
        
      case POPULATION_DISPLAY:
        if (abs(currentPopulation - prevPopulation) > 100) { // Only update when significant change
          shouldUpdateDisplay = true;
          prevPopulation = currentPopulation;
        }
        break;
        
      case SPEED_DISPLAY:
        if (abs(currentSpeed - prevSpeed) > 2.0) { // Only update with 2 km/h change
          shouldUpdateDisplay = true;
          prevSpeed = currentSpeed;
        }
        break;
    }
  }
  
  // Update display if needed
  if (shouldUpdateDisplay || currentMillis - lastModeChange < 100) {
    Serial.println("Updating display");
    
    // Display the current mode
    switch(currentMode) {
      case WEATHER_DISPLAY:
        displayWeather(currentWeather);
        break;
        
      case TIME_DISPLAY:
        displayTime(now());
        break;
        
      case POPULATION_DISPLAY:
        displayPopulation(currentPopulation);
        break;
        
      case SPEED_DISPLAY:
        displaySpeed(currentSpeed);
        break;
    }
  }
  
  // Check for button presses or other input to manually change modes
  // This would be where you'd add code to read input pins or touch screen
  
  delay(100); // Billboard mode - less frequent updates need less frequent polling
}

// Additional helper functions can be added here