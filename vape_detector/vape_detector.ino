#include <SoftwareSerial.h>

SoftwareSerial pmsSerial(4, 7); // RX, TX

void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600);
}

void loop() {
  if (pmsSerial.available() >= 32) {  // Wait until at least 32 bytes are ready
    uint8_t buffer[32];
    int index = 0;
    while (index < 32) {
      buffer[index++] = pmsSerial.read();
    }

    if (buffer[0] == 0x42 && buffer[1] == 0x4D) {
      int pm25 = (buffer[12] << 8) + buffer[13];
      Serial.print("PM2.5: ");
      Serial.print(pm25);
      Serial.println(" ug/m3");
    }
  }
}

// void loop() {

//   if (pmsSerial.available()) {

//     // Read data from the sensor
//     uint8_t buffer[32];
//     int index = 0;
//     while (pmsSerial.available() && index < 32) {
//       buffer[index++] = pmsSerial.read();
//     }
    
//     // Process the data (assuming the data starts with 0x42 0x4d)
//     if (index == 32 && buffer[0] == 0x42 && buffer[1] == 0x4D) {
//       int pm25 = (buffer[12] << 8) + buffer[13];
//       Serial.print("PM2.5: ");
//       Serial.print(pm25);
//       Serial.println(" ug/m3");
//     }
//   }
// }