#include <Arduino.h>
#include <hd44780.h>
#define BLYNK_TEMPLATE_ID "your blynk id"
#define BLYNK_TEMPLATE_NAME "your project name"
#define BLYNK_AUTH_TOKEN "your Blynk token"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>
#include "base64.h"


#define SENSOR_PIN A0
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int sampleWindow = 50;
unsigned int sample;
int db;
char auth[] = BLYNK_AUTH_TOKEN;



// Your network SSID and password
const char* ssid = "your WiFi name";
const char* password = "your Wifi password";

const char* account_sid = "your Twilio sid";
const char* auth_token = "your twilio token";
String from_number      = "twilio number";
String to_number        = "your number";
String message_body     = "type your message here";

// Find the api.twilio.com SHA1 fingerprint using,
//  echo | openssl s_client -connect api.twilio.com:443 | openssl x509 -fingerprint
const char fingerprint[] = "80 58 F5 0B A9 DF A0 B8 49 5B 87 7C 82 47 75 04 FB D2 9D FD";
// the above fingerprint changes everywhere, thiis is 2024 fingerprint

String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
  }
  return encodedString;
}

String get_auth_header(const String& user, const String& password) {
  size_t toencodeLen = user.length() + password.length() + 2;
  char toencode[toencodeLen];
  memset(toencode, 0, toencodeLen);
  snprintf(toencode, toencodeLen, "%s:%s", user.c_str(), password.c_str());
  String encoded = base64::encode((uint8_t*)toencode, toencodeLen - 1);
  String encoded_string = String(encoded);
  std::string::size_type i = 0;
  // Strip newlines (after every 72 characters in spec)
  while (i < encoded_string.length()) {
    i = encoded_string.indexOf('\n', i);
    if (i == -1) {
      break;
    }
    encoded_string.remove(i, 1);
  }
  return "Authorization: Basic " + encoded_string;
}
void setup() {
   Serial.begin(115200);// 115200 or 9600
  pinMode (SENSOR_PIN, INPUT);
  lcd.init();
  lcd.backlight();

  // Display "Inrtro"
  lcd.setCursor(3, 0);
  lcd.print("Project name");
  lcd.setCursor(3, 1);
  lcd.print(" team name");
  delay(2000); // Wait for 3 seconds

  // Clear the LCD and display "Noise Pollution Monitoring"
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Noise Pollution ");
  lcd.setCursor(3, 1);
  lcd.print("Monitoring ");
   
 
  delay(2000);
lcd.clear();

  lcd.clear();
  Blynk.begin(auth, ssid, password);
  Blynk.virtualWrite(V0, db);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
  Serial.println("+ Connect to WiFi. ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("+ Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() {
  Blynk.run();
  unsigned long startMillis = millis(); // Start of sample window
  float peakToPeak = 0; // peak-to-peak level
  unsigned int signalMax = 0; //minimum value
  unsigned int signalMin = 1024; //maximum value
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(A0); //get reading from microphone
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample; // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample; // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
  Serial.println(peakToPeak);
  db = map(peakToPeak, 20, 900, 0, 90); //calibrate for deciBels
  // db = map(peakToPeak, 20, 900, 49.5, 90); //calibrate for deciBels
 // Blynk.virtualWrite(V0, db);
  lcd.setCursor(0, 0);
  lcd.print("Loudness: ");
  lcd.print(db);
  lcd.print("dB");
  if (db <= 50)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: Quite");
    Serial.println("Level: Quite");
    Serial.println(db);
  }
  else if (db > 50 && db < 75)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: Moderate");
    Serial.println("Level: Moderate");
    Serial.println(db);
  }
  else if (db >= 75)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: High");
    Serial.println("Level: High");
    Serial.println(db);
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    Serial.printf("+ Using fingerprint '%s'\n", fingerprint);
    const char* host = "api.twilio.com";
    const int   httpsPort = 443;
    Serial.print("+ Connecting to ");
    Serial.println(host);
    if (!client.connect(host, httpsPort)) {
      Serial.println("- Connection failed.");
      return; // Skips to loop();
    }
    Serial.println("+ Connected.");
    Serial.println("+ Post an HTTP send SMS request.");
    String post_data = "To=" + urlencode(to_number)
                       + "&From=" + urlencode(from_number)
                       + "&Body=" + urlencode(message_body);
    String auth_header = get_auth_header(account_sid, auth_token);
    String http_request = "POST /2010-04-01/Accounts/" + String(account_sid) + "/Messages HTTP/1.1\r\n"
                          + auth_header + "\r\n"
                          + "Host: " + host + "\r\n"
                          + "Cache-control: no-cache\r\n"
                          + "User-Agent: ESP8266 Twilio Example\r\n"
                          + "Content-Type: application/x-www-form-urlencoded\r\n"
                          + "Content-Length: " + post_data.length() + "\r\n"
                          + "Connection: close\r\n"
                          + "\r\n"
                          + post_data
                          + "\r\n";
    client.println(http_request);
  // Read the response.
    String response = "";
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      response += (line);
      response += ("\r\n");
    }
    Serial.println("+ Connection is closed.");
    Serial.println("+ Response:");
    Serial.println(response);

    }
  delay(600);
  Blynk.virtualWrite(V0, db);
  lcd.clear();
}

