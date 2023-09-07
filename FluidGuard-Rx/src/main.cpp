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

void updateRelayState(int percentage)
{
  if (isTemporaryFailDisplayed)
  {
    relayState = LOW; // Turn off the relay
  }
  else if (percentage >= 117)
  {
    relayState = LOW; // Turn off the relay
  }
  else if (percentage <= 50)
  {
    relayState = HIGH; // Turn on the relay
  }

  digitalWrite(relayPin, relayState);
}

void displayTemporaryFail()
{
  if (!isTemporaryFailDisplayed)
  {
    lcd.clear(); // Clear the LCD screen
    lcd.print("Temporary Fail"); // Display the warning message
    isTemporaryFailDisplayed = true;
  }
}

void clearTemporaryFail()
{
  if (isTemporaryFailDisplayed)
  {
    lcd.clear(); // Clear the LCD screen
    isTemporaryFailDisplayed = false;
  }
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
    displayBarGraph(distance, 120); // Assuming a maximum distance of 100 cm

    updateRelayState(distance); // Update the relay state based on distance percentage
  }

  // Check if the timeout duration has elapsed since the last received percentage value
  if (millis() - lastReceivedTime >= timeoutDuration)
  {
    displayTemporaryFail(); // Display the "Temporary Fail" message
    updateRelayState(0);    // Stop relay operation (set relayState to LOW)
    // Perform any additional actions as needed
  }

  // Read the button A0 state
  bool currentButtonStateA0 = digitalRead(buttonPinA0);

  // Check if the button A0 state has changed
  if (currentButtonStateA0 != buttonPressedA0)
  {
    // Update the button A0 state and toggle the relay
    buttonPressedA0 = currentButtonStateA0;
    relayState = (buttonPressedA0) ? HIGH : relayState;
    digitalWrite(relayPin, relayState);
  }

  // Read the button A1 state
  bool currentButtonStateA1 = digitalRead(buttonPinA1);

  // Check if the button A1 state has changed
  if (currentButtonStateA1 != buttonPressedA1)
  {
    // Update the button A1 state and toggle the relay
    buttonPressedA1 = currentButtonStateA1;
    relayState = (buttonPressedA1) ? HIGH : relayState;
    digitalWrite(relayPin, relayState);
  }
}
