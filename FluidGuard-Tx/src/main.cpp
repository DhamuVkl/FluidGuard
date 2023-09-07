#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);  // Set CE and CSN pins for NRF24L01+ module

const byte address[6] = "00001";  // Set the transmitting address as a byte array

const int triggerPin = 2;  // Connect the trigger pin of the HC-SR04 to digital pin 2
const int echoPin = 3;  // Connect the echo pin of the HC-SR04 to digital pin 3

unsigned long previousMillis = 0;  // Stores the previous time
const long interval = 1000;  // Interval between measurements in milliseconds

const int numSamples = 5;  // Number of distance samples to collect
int distances[numSamples];  // Array to store the distance samples
int currentIndex = 0;  // Index of the current distance sample

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(120); 
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize the distances array with an initial value
  for (int i = 0; i < numSamples; i++) {
    distances[i] = -1;
  }
}

void loop() {
  unsigned long currentMillis = millis();  // Current time
  
  // Check if the interval has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Update the previous time
    
    long duration, distance;
    
    // Send a pulse to the trigger pin to start the measurement
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    
    // Measure the duration of the pulse from the echo pin
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance in centimeters
    distance = duration / 58.2;
    
    // Apply error reduction using indexing approach
    if (distance >= 2 && distance <= 400) {
      distances[currentIndex] = distance;
      currentIndex = (currentIndex + 1) % numSamples;
    }
    
    // Calculate the average distance
    int sum = 0;
    int validSamples = 0;
    for (int i = 0; i < numSamples; i++) {
      if (distances[i] != -1) {
        sum += distances[i];
        validSamples++;
      }
    }
    int averageDistance = (validSamples > 0) ? (sum / validSamples) : -1;
    
    radio.write(&averageDistance, sizeof(averageDistance));  // Send the average distance value wirelessly to the receiver
  }
}
