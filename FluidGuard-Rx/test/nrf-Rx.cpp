#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);                       // NRF24L01+ module connections
const byte address[6] = "00001";          // Set the receiving address
const int maxReceptionAttempts = 3;      // Maximum number of reception attempts

void setup() {
  Serial.begin(9600);
  radio.begin();                         // Initialize the NRF24L01+ module
  radio.openReadingPipe(0, address);      // Set the receiving address
  radio.setPALevel(RF24_PA_MIN);         // Set power amplifier level to high
 // radio.setChannel(76);                   // Set the RF channel (76)
  //radio.setDataRate(RF24_250KBPS);        // Set data rate to 250kbps
  radio.startListening();                 // Start listening for incoming data
}

void loop() {
  // Check if data is available to be read
  if (radio.available()) {
    // Receive the message
    String receivedMessage;
    radio.read(&receivedMessage, sizeof(receivedMessage));

    Serial.print("Received message: ");
    Serial.println(receivedMessage);
  }

 // delay(100);  // Add a small delay for stability
}
