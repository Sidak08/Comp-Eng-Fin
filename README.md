# Display Project

This project demonstrates a display system for an ESP32 microcontroller with an ILI9341 display. The system cycles through various screens showing weather information, time, population statistics, and speed indicators. It also controls traffic light LEDs.

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

- `src/main.cpp`: Main program file
- `src/displayFunctions.h`: Header file containing display function declarations
- `src/displayFunctions.cpp`: Implementation of display functions
- `src/trafficLight.cpp`: Implementation of traffic light control

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