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

bool startChecking = false;

HardwareSerial pmsSerial(2);

WiFiClientSecure ssl_client;
SMTPClient smtp(ssl_client);

void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600, SERIAL_8N1, 16, 17);
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
  if (pmsSerial.available()){

    if (startChecking){
      if (pmsSerial.available() >= 31) {  // Wait until at least 32 bytes are ready
        uint8_t buffer[31];
        int index = 0;
        Serial.println(pmsSerial.available());
        while (index < 31) {
          buffer[index++] = pmsSerial.read();
        }

        if (buffer[0] == 0x4d) {
          uint16_t pm25 = (buffer[11] << 8) + buffer[12];
          Serial.print("PM2.5: ");
          Serial.print(pm25);
          Serial.println(" ug/m3");
          startChecking = false;
        }
      }

    }else{
      uint8_t first_byte = 0x42;
      if (pmsSerial.read() == first_byte){
        startChecking = true;
        Serial.println(startChecking);
      }
    }
  }
  
}