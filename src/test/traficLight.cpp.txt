#include <Arduino.h>

// Pin definitions for traffic lights
int ledPins[] = {19, 14, 13, 5, 25, 26};
// red yellow green red2 yellow2 green2
const int numPins = sizeof(ledPins) / sizeof(ledPins[0]);

// Timing parameters (in milliseconds)
unsigned long greenTime = 5000;  // Green light duration
unsigned long yellowTime = 2000; // Yellow light duration
unsigned long allRedTime = 1000; // Time when all lights are red (safety interval)

// Variables for non-blocking operation
unsigned long previousMillis = 0;  // will store last time lights were updated
unsigned long currentInterval = 0; // current interval to wait
int currentStep = 0;               // current step in the traffic sequence
bool lightIsOn = false;            // track if a light is currently on

// void setup()
// {
//     Serial.begin(115200); // Initialize serial for debugging

//     // Initialize all pins as outputs
//     for (int i = 0; i < numPins; i++)
//     {
//         pinMode(ledPins[i], OUTPUT);
//         digitalWrite(ledPins[i], LOW); // Start with all lights off
//     }

//     // Initially set both red lights on
//     digitalWrite(ledPins[0], HIGH); // red1
//     digitalWrite(ledPins[3], HIGH); // red2

//     // Initialize timing variables
//     previousMillis = millis();
//     currentInterval = 2000; // Initial delay before sequence starts
// }

// Function to handle the traffic light sequence without blocking
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
            currentInterval = 2000; // Time to keep light on

            Serial.print("Turning ON light at pin ");
            Serial.println(ledPins[i]);
        }
        else
        {
            // Turn off current light and move to next step
            digitalWrite(ledPins[i], LOW);
            lightIsOn = false;
            currentStep++;
            currentInterval = 100; // Short delay before turning on next light

            Serial.print("Turning OFF light at pin ");
            Serial.println(ledPins[i]);
        }
    }
}

void loop()
{
    // Update traffic lights (non-blocking)
    updateTrafficLights();

    // You can add other code here that will run without being blocked

    // For example:
    // checkPedestrianButton();
    // readSensors();
    // updateDisplays();

    // Small delay to prevent CPU hogging
    delay(10);
}