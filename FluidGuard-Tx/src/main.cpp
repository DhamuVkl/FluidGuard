#include <NewPing.h>
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define TRIGGER_PIN  3  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     4  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 120 // Maximum distance we want to ping for (in centimetres). Maximum sensor distance is rated at 400-500cm.
 
int i;
int distanceCM;
RF24 radio(9, 10); // CE, CSN
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
const byte address[6] = "00001";


void setup() {
   Serial.begin(9600);
   radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}


void loop() {
  distanceCM = sonar.ping_cm();   // get distance in cm
  i = (120 - distanceCM);
  radio.write(&i,sizeof(i));
  Serial.println(i);
  delay(250);
}
