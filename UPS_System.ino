// UPS System Coding

#define BLYNK_TEMPLATE_ID "TMPL3r6EaM24U"
#define BLYNK_TEMPLATE_NAME "industrial safety system"
#define BLYNK_AUTH_TOKEN "QOmsqVGEP9NcN86L5BBc_ClXZHkOODl3"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// WiFi & Blynk Credentials
char auth[] = "QOmsqVGEP9NcN86L5BBc_ClXZHkOODl3";
char ssid[] = "iot";
char pass[] = "123456789";

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensors
DHT dht(D3, DHT11);
BlynkTimer timer;

// Pin Definitions
#define BUZZER D0
#define MQ2 A0
#define TRIG D4
#define ECHO D5
#define PIR D6
#define RELAY1 D7
#define RELAY2 D8

bool pirbutton = 0;

// Blynk Button
BLYNK_WRITE(V0) {
  pirbutton = param.asInt();
}

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(BUZZER, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);

  Blynk.begin(auth, ssid, pass);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("Industrial UPS");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring");
  delay(3000);
  lcd.clear();

  timer.setInterval(100L, gasSensor);
  timer.setInterval(100L, dhtSensor);
  timer.setInterval(100L, pirSensor);
  timer.setInterval(100L, ultrasonicSensor);
}

void gasSensor() {
  int value = analogRead(MQ2);
  value = map(value, 0, 1024, 0, 100);

  if (value > 55) {
    digitalWrite(BUZZER, HIGH);
    Blynk.logEvent("battery_distilled", "Distilled water level low");
  } else {
    digitalWrite(BUZZER, LOW);
  }

  Blynk.virtualWrite(V1, value);
  lcd.setCursor(0, 0);
  lcd.print("G:");
  lcd.print(value);
}

void dhtSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) return;

  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);

  lcd.setCursor(8, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(h);
}

void pirSensor() {
  bool value = digitalRead(PIR);

  if (pirbutton && value) {
    digitalWrite(BUZZER, HIGH);
    Blynk.logEvent("security_alert", "Intruder Detected");
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

void ultrasonicSensor() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration / 29 / 2;

  Blynk.virtualWrite(V4, distance);

  lcd.setCursor(8, 1);
  lcd.print("W:");
  lcd.print(distance);
}

BLYNK_WRITE(V5) {
  digitalWrite(RELAY1, param.asInt() ? LOW : HIGH);
}

BLYNK_WRITE(V6) {
  digitalWrite(RELAY2, param.asInt() ? LOW : HIGH);
}

void loop() {
  Blynk.run();
  timer.run();
}
