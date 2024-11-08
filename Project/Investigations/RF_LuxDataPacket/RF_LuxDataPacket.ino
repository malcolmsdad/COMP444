#include <VirtualWire.h>
#include <ArduinoJson.h>
#include <Vector.h>

const int rfTxPin = 12;   // PIN RF transmitter pin
const int rfRxPin = 11;   // PIN RF receiver pin
const int rfBaud = 2000;  // Bits per second of RF
const int lightSensorPin = A0;  // Light sensor is connected to analog pin A0


Vector<String> messageHistory;  // Vector to store messages

void setup() {
  Serial.begin(9600);      // Start serial communication
  vw_set_tx_pin(rfTxPin);  // Set transmitter pin
  vw_set_rx_pin(rfRxPin);  // Set receiver pin
  vw_setup(rfBaud);        // Bits per second speed
  vw_rx_start();           // Start the receiver PLL running
}

void transmitData(const char *msg) {
  //messageHistory.push_back(msg);  // Store the message in the vector
  Serial.print("TX:");
  Serial.print(msg);
  Serial.print("[");
  Serial.print(strlen(msg));
  Serial.print("]");

  // Create a new buffer with "1234" prepended
  char buffer[128];
  strcpy(buffer, "1234");
  strcat(buffer, msg);

  vw_send((uint8_t *)buffer, strlen(buffer));
  vw_wait_tx();  // Wait for the message to be sent
}

void prepareDataPacket(int lux) {
  // Create a JSON document
  StaticJsonDocument<200> doc;
  doc["lux"] = lux;  // Add the variable to the JSON document

  // Serialize the JSON document to a string
  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer);

  transmitData(jsonBuffer);
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
  // read ambient light level
  int sensorValue = analogRead(lightSensorPin);  // Read the value from the light sensor
  
  // prepare packet and send
  prepareDataPacket(sensorValue);  

  delay(2000);  // Send message every 2 seconds
}
