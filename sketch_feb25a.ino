#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- PINS (STRICTLY FROM YOUR SCHEMATIC) ---
#define GSM_RX 3   // Arduino RX  <- GSMTXD
#define GSM_TX 2   // Arduino TX  -> GSMRXD

#define GPS_RX 5   // Arduino RX  <- GPSRXD
#define GPS_TX 4   // Arduino TX  -> GPSTXD

#define BUTTON 7   // PB
#define BUZZER 6   // BZ

SoftwareSerial gsmSerial(GSM_RX, GSM_TX);
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String nmeaLine = "";
String latitude = "0.0000";
String longitude = "0.0000";

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  Serial.begin(9600);
  gsmSerial.begin(9600);
  gpsSerial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Emergency Alert");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();

  initGSM();
}

void loop() {
  readGPS();
  showGSMResponse();

  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(BUZZER, HIGH);
    lcd.clear();
    lcd.print("Sending SMS...");
    sendSMS();
    digitalWrite(BUZZER, LOW);
    lcd.clear();
    lcd.print("SMS Sent");
    delay(2000);
    lcd.clear();
  }
}

void initGSM() {
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // SMS text mode
  delay(1000);
}

void sendSMS() {
  gsmSerial.println("AT+CMGS=\"0716834559\"");
  delay(1000);

  gsmSerial.print("EMERGENCY ALERT!\n");
  gsmSerial.print("Lat: ");
  gsmSerial.print(latitude);
  gsmSerial.print("\nLon: ");
  gsmSerial.print(longitude);
  gsmSerial.print("\nhttps://maps.google.com/?q=");
  gsmSerial.print(latitude);
  gsmSerial.print(",");
  gsmSerial.print(longitude);

  gsmSerial.write(26); // CTRL+Z
  delay(5000);
}

void readGPS() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    nmeaLine += c;

    if (c == '\n') {
      if (nmeaLine.indexOf("$GPGGA") >= 0) {
        parseGPGGA(nmeaLine);
      }
      nmeaLine = "";
    }
  }
}

void parseGPGGA(String s) {
  // GPGGA: $GPGGA,time,lat,N,lon,E,fix,...
  int p1 = s.indexOf(',') + 1;         // time start
  int p2 = s.indexOf(',', p1) + 1;     // lat start
  int p3 = s.indexOf(',', p2);         // lat end
  String latRaw = s.substring(p2, p3);

  int p4 = s.indexOf(',', p3 + 1) + 1; // lon start
  int p5 = s.indexOf(',', p4);         // lon end
  String lonRaw = s.substring(p4, p5);

  if (latRaw.length() > 4 && lonRaw.length() > 4) {
    latitude = latRaw;
    longitude = lonRaw;

    lcd.setCursor(0, 0);
    lcd.print("Lat:");
    lcd.print(latitude);

    lcd.setCursor(0, 1);
    lcd.print("Lon:");
    lcd.print(longitude);
  }
}

void showGSMResponse() {
  while (gsmSerial.available()) {
    char c = gsmSerial.read();
    Serial.write(c);   // also view in Arduino Serial Monitor
  }
}