/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at: https://RandomNerdTutorials.com/esp32-send-email-smtp-server-arduino-ide/  
  Based on the example provided by the ReadyMail library: https://github.com/mobizt/ReadyMail/
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HardwareSerial.h>

#define ENABLE_SMTP
#define ENABLE_DEBUG
#include <ReadyMail.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "a";
const char* password = "b";

// Sender SMTP settings (GMAIL)
// Change if using a different provider
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

// Sender email, app password, and name
#define AUTHOR_EMAIL "centsteam43@gmail.com"
#define AUTHOR_APP_PASS "apiw zcts zzpt bmnh"
#define AUTHOR_NAME "STEAM Team"

//Recipient's email
#define RECIPIENT_EMAIL "potatogonehaywire@gmail.com"
#define RECIPIENT_NAME "steam team"

#define RXD2 16
#define TXD2 17

HardwareSerial pmsSerial(2);

WiFiClientSecure ssl_client;
SMTPClient smtp(ssl_client);

void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.print("hello, warming up");
  delay(90000); 
  Serial.print("warmup complete");
}

void SendEmail(){
  Serial.println(RECIPIENT_EMAIL);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  ssl_client.setInsecure();

  auto statusCallback = [](SMTPStatus status) {
    Serial.println(status.text);
  };

  smtp.connect(SMTP_HOST, SMTP_PORT, statusCallback);

  if (smtp.isConnected()) {
    smtp.authenticate(AUTHOR_EMAIL, AUTHOR_APP_PASS, readymail_auth_password);

    SMTPMessage msg;

    msg.headers.add(rfc822_from, String(AUTHOR_NAME) + " <" + AUTHOR_EMAIL + ">");
    msg.headers.add(rfc822_to, String(RECIPIENT_NAME) + " <" + RECIPIENT_EMAIL + ">");
    msg.headers.add(rfc822_subject, "Vape detector");
    msg.text.body("This is a test to see if the district's teacher accounts also filter out emails from our vape detector, if you see this email, please Vannah know. Thanks!");
    //msg.html.body("<html><body><h1>Hello!</h1></body></html>");
     
    // Set NTP config time
    /* For times east of the Prime Meridian use 0-12
    For times west of the Prime Meridian add 12 to the offset.
    Ex. American/Denver GMT would be -6. 6 + 12 = 18 */
    const int gmtOffset_sec = 0; //offset time in seconds
    const int daylightOffset_sec = 0; //daylight saving time offset in seconds

    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
    // Set timestamp for the email
    while (time(nullptr) < 100000) delay(100);
    msg.timestamp = time(nullptr);

    smtp.send(msg);
  }
}

void loop() {
  // Wait for start byte
  if (!pmsSerial.available()) return;
  uint8_t byte1 = pmsSerial.read();
  if (byte1 != 0x42) return;

  // Wait for second start byte with timeout
  unsigned long t = millis();
  while (!pmsSerial.available()) {
    if (millis() - t > 50) return;
  }
  uint8_t byte2 = pmsSerial.read();
  if (byte2 != 0x4D) return;

  // Read remaining 30 bytes with timeout
  uint8_t frame[30];
  int index = 0;
  t = millis();
  while (index < 30) {
    if (pmsSerial.available()) {
      frame[index++] = pmsSerial.read();
    }
    if (millis() - t > 100) {
      Serial.println("Timeout");
      return;
    }
  }

  // Checksum: sum of 0x42 + 0x4D + first 28 bytes of frame
  uint16_t checksum = 0x42 + 0x4D;
  for (int i = 0; i < 28; i++) {
    checksum += frame[i];
  }
  uint16_t frameChecksum = (frame[28] << 8) | frame[29];

  Serial.print("Calculated: ");
  Serial.println(checksum);
  Serial.print("Frame checksum: ");
  Serial.println(frameChecksum);

  if (checksum != frameChecksum) {
    Serial.println("Checksum failed");
    return;
  }

  // Parse PM2.5 (bytes 10-11 of frame, which is bytes 12-13 of full packet)
  int pm25 = (frame[10] << 8) | frame[11];
  if (pm25 >= 0 && pm25 <= 500) {
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.println(" ug/m3");
  }
}