#include <VirtualWire.h>
#include <Vector.h>

const int rfTxPin = 12;     // PIN RF transmitter pin
const int rfRxPin = 11;     // PIN RF receiver pin
const int rfBaud = 2000;    // Bits per second of RF

Vector<String> messageHistory; // Vector to store messages

void setup() {
  Serial.begin(9600);       // Start serial communication
  vw_set_tx_pin(rfTxPin);   // Set transmitter pin
  vw_set_rx_pin(rfRxPin);   // Set receiver pin
  vw_setup(rfBaud);         // Bits per second speed
  vw_rx_start();            // Start the receiver PLL running
}

void transmitData(const char* msg) {
  //messageHistory.push_back(msg);  // Store the message in the vector
  Serial.print("TX:");
  Serial.print(msg);
  Serial.print("[");
  Serial.print(strlen(msg));
  Serial.print("]");

  vw_send((uint8_t *)msg, strlen(msg));
  vw_send((byte*)128,2);
  vw_wait_tx();  // Wait for the message to be sent
}


String readData() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  String outputString = "";

  if (vw_get_message(buf, &buflen)) {  // Non-blocking
    for (int i = 0; i < buflen; i++) {
      outputString += (char)buf[i];
    }
  }

  return outputString;
}

void loop() {
  const char *msg = "____Hello, World!";
  transmitData(msg);

  String rfData = readData();
  Serial.println(rfData);

  delay(2000);  // Send message every 2 seconds
}
