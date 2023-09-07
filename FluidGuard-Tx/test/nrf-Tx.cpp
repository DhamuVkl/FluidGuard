#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);                      // NRF24L01+ module connections
const byte address[6] = "00001";         // Set the transmitting address as a byte array
const int maxTransmissionAttempts = 3;  // Maximum number of transmission attempts
const int transmitInterval = 1000;      // Delay between transmissions (in milliseconds)

void setup() {
  Serial.begin(9600);
  radio.begin();                        // Initialize the NRF24L01+ module
  radio.openWritingPipe(address);       // Set the transmitting address
  radio.setPALevel(RF24_PA_MIN);        // Set power amplifier level to high
 // radio.setChannel(76);                  // Set the RF channel (76)
  //radio.setDataRate(RF24_250KBPS);       // Set data rate to 250kbps
  radio.stopListening();
}

void loop() {
  // Create a text message to transmit
  String message = "Hello, receiver!";

  // Transmit the message via NRF24L01+
  bool transmissionSuccess = false;
  for (int i = 0; i < maxTransmissionAttempts; i++) {
    if (radio.write(&message, sizeof(message))) {
      transmissionSuccess = true;
      break;
    }
    Serial.println("Transmission failed. Retrying...");
    delay(500);  // Delay between transmission retries
  }

  if (transmissionSuccess) {
    Serial.println("Message transmitted successfully.");
  } else {
    Serial.println("Max transmission attempts reached. Message not transmitted.");
  }

  delay(transmitInterval);  // Delay before next transmission
}
