# Display Project

This project demonstrates a display system for an ESP32 microcontroller with an ILI9341 display. The system cycles through various screens showing weather information, time, population statistics, and speed indicators. It also controls traffic light LEDs and includes darkness and speed detection functionality.

## Required Libraries

Make sure to install the following libraries via the Arduino Library Manager or PlatformIO:

- Adafruit GFX Library
- Adafruit ILI9341
- TimeLib (by Paul Stoffregen)
- SPI (built into Arduino)

## Hardware Setup

### Display Connection

The ILI9341 display should be connected to the ESP32 using the following pins:

- CS: GPIO 15
- DC: GPIO 2
- RST: GPIO 4
- MOSI: GPIO 23
- CLK: GPIO 18
- MISO: GPIO 19

### Traffic Light LEDs

The traffic light LEDs should be connected to the following GPIO pins:

- Red light 1: GPIO 19
- Yellow light 1: GPIO 14
- Green light 1: GPIO 13
- Red light 2: GPIO 5
- Yellow light 2: GPIO 25
- Green light 2: GPIO 26

## Project Structure

### Files Organization

- `src/main.cpp`: Main program file
- `src/displayFunctions.h`: Header file containing display function declarations
- `src/displayFunctions.cpp`: Implementation of display functions
- `lib/TrafficLight/TrafficLight.cpp`: Implementation of traffic light control
- `lib/TrafficLight/TrafficLight.h`: Header file for traffic light class

### Code Structure Overview

1. **Main Program (`src/main.cpp`)**
   - System initialization and main loop
   - Mode switching between different display screens
   - Non-blocking state machines for various features

2. **Display Functions (`src/displayFunctions.cpp` and `.h`)**
   - Rendering functions for different information screens
   - Helper functions for drawing graphics (icons, gauges)
   - Color and layout definitions

3. **Traffic Light Module (`lib/TrafficLight/`)**
   - Object-oriented implementation for traffic light control
   - State management for light sequences
   - Timing control for different light phases

### Main Program Architecture

The main program uses a non-blocking architecture with several state machines running concurrently:

- **Display rotation system** - Cycles through 4 information screens (Weather, Time, Population, Speed)
- **Traffic light controller** - Manages multiple traffic lights with proper sequencing
- **Darkness detection** - Monitors ambient light and triggers outputs when dark
- **Speed detection** - Uses dual sensors to calculate object speed

### Hardware Integration

The code interfaces with several hardware components:

- **ILI9341 TFT Display** - Shows graphical information via SPI
- **LED outputs** - Controls traffic light LEDs
- **Photoresistors** - For darkness and speed detection
- **Timer system** - Manages non-blocking operations

### Data Management

Data is organized using simple structures:
- `WeatherData` structure for temperature, humidity and conditions
- `SpeedDetector` structure for speed sensing state machine
- Enums for display modes and detector states

### Rendering Approach

The display rendering follows a pattern where:
1. Screen is cleared
2. Title and divider line are drawn
3. Data-specific information is displayed
4. Graphical elements (icons, gauges) are added
5. Demo indicators are shown if applicable

## Building the Project

### Using PlatformIO

1. Install Visual Studio Code and the PlatformIO extension
2. Open this project folder in VSCode
3. Click on the PlatformIO icon in the sidebar
4. Click "Build" to compile the project
5. Click "Upload" to upload the project to your ESP32

### Using Arduino IDE

1. Install the Arduino IDE
2. Install the ESP32 board support package
3. Install the required libraries
4. Create a new sketch and copy the contents of the files from the `src` folder
5. Select your ESP32 board and port
6. Click "Upload"

## Troubleshooting

### Import Issues

If you encounter import issues, make sure:

1. All required libraries are installed
2. You're using the correct library versions (see `platformio.ini` for version information)
3. Your Arduino IDE or PlatformIO is up to date

### Display not working

1. Check the wiring connections
2. Verify the SPI pins are correctly defined in the code
3. Ensure the display is powered with the appropriate voltage

### Traffic lights not working

1. Verify the GPIO pin connections
2. Check that the LED pins are correctly defined in the code
3. Make sure the LEDs are connected with appropriate current-limiting resistors

## License

This project is open source and available under the MIT License.