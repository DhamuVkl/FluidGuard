#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <LiquidCrystal.h>

RF24 radio(9, 10); // Set CE and CSN pins for NRF24L01+ module

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // LCD module connections (change these pins as per your wiring)
const byte address[6] = "00001";      // Set the transmitting address as a byte array

const int buttonPinA0 = A0; // Push button A0 pin
const int buttonPinA1 = A1; // Push button A1 pin
const int relayPin = 8;     // Relay control pin

int relayState = LOW;           // Initial state of the relay
bool buttonPressedA0 = false;   // Track the button A0 state
bool buttonPressedA1 = false;   // Track the button A1 state
unsigned long lastReceivedTime; // Variable to store the last time a percentage value was received
const unsigned long timeoutDuration = 5000; // Timeout duration in milliseconds (e.g., 5000 = 5 seconds)

bool isTemporaryFailDisplayed = false; // Flag to track if "Temporary Fail" is currently displayed

const int distanceMax = 120; // Maximum distance for bar graph display
const int highThreshold = 117; // High threshold for relay state
const int lowThreshold = 50; // Low threshold for relay state

// Function to display the bar graph on the LCD
void displayBarGraph(int value, int maxValue)
{
  int numBars = map(value, 0, maxValue, 0, 16);     // Map the value to the number of bars (0-16)
  int percentage = map(value, 0, maxValue, 0, 100); // Calculate the percentage

  lcd.setCursor(0, 0); // Set the cursor to the first column of the first row

  for (int i = 0; i < numBars; i++)
  {
    lcd.write(0xFF); // Display a filled block character
  }

  lcd.setCursor(0, 1); // Set the cursor to the first column of the second row

  lcd.print(percentage); // Display the percentage
  lcd.print("%");        // Display the percentage symbol

  lcd.print("       "); // Clear any remaining characters on the line
}

// Function to update the relay state based on distance percentage
void updateRelayState(int percentage)
{
  if (isTemporaryFailDisplayed || percentage >= highThreshold)
  {
    relayState = LOW; // Turn off the relay
  }
  else if (percentage <= lowThreshold)
  {
    relayState = HIGH; // Turn on the relay
  }

  digitalWrite(relayPin, relayState);
}

// Function to display the "Temporary Fail" message
void displayTemporaryFail()
{
  if (!isTemporaryFailDisplayed)
  {
    lcd.clear(); // Clear the LCD screen
    lcd.print("Temporary Fail"); // Display the warning message
    isTemporaryFailDisplayed = true;
  }
}

// Function to clear the "Temporary Fail" message
void clearTemporaryFail()
{
  if (isTemporaryFailDisplayed)
  {
    lcd.clear(); // Clear the LCD screen
    isTemporaryFailDisplayed = false;
  }
}

// Function to read and debounce the button state
bool readButtonState(int buttonPin, bool& buttonState)
{
  bool currentState = digitalRead(buttonPin);
  if (currentState != buttonState)
  {
    delay(50); // Debounce delay
    currentState = digitalRead(buttonPin);
    if (currentState != buttonState)
    {
      buttonState = currentState;
      return true; // Button state changed
    }
  }
  return false; // Button state unchanged
}

void setup()
{
  lcd.begin(16, 2); // Set the LCD dimensions (16 columns and 2 rows)

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(75);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  pinMode(buttonPinA0, INPUT);
  pinMode(buttonPinA1, INPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(relayPin, LOW); // Set initial state of the relay

  lastReceivedTime = millis(); // Initialize the lastReceivedTime variable with the current time
}

void loop()
{
  if (radio.available())
  {
    unsigned long distance;
    radio.read(&distance, sizeof(distance)); // Receive the sonar distance value from the transmitter

    lastReceivedTime = millis(); // Update the lastReceivedTime variable

    clearTemporaryFail(); // Clear the "Temporary Fail" message if it was previously displayed

    lcd.clear();                    // Clear the LCD screen
    displayBarGraph(distance, distanceMax);

    updateRelayState(distance); // Update the relay state based on distance percentage
  }

  // Check if the timeout duration has elapsed since the last received percentage value
  if (millis() - lastReceivedTime >= timeoutDuration)
  {
    displayTemporaryFail(); // Display the "Temporary Fail" message
    updateRelayState(0);    // Stop relay operation (set relayState to LOW)
    // Perform any additional actions as needed
  }

  // Read and handle button A0 state change
  if (readButtonState(buttonPinA0, buttonPressedA0))
  {
    relayState = buttonPressedA0 ? HIGH : LOW;
    digitalWrite(relayPin, relayState);
  }

  // Read and handle button A1 state change
  if (readButtonState(buttonPinA1, buttonPressedA1))
  {
    relayState = buttonPressedA1 ? HIGH : LOW;
    digitalWrite(relayPin, relayState);
  }
}
