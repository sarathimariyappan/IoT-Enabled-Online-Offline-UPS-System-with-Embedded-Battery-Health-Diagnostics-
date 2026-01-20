// Battery Monitoring Coding

#define BLYNK_TEMPLATE_ID "TMPL3EhguZ-jF"
#define BLYNK_TEMPLATE_NAME "UPS Monitoring"
#define BLYNK_AUTH_TOKEN "pKuZlMkuFj4peYwqqAgbz0z2-PO3wQyu"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char auth[] = "pKuZlMkuFj4peYwqqAgbz0z2-PO3wQyu";
char ssid[] = "iot";
char pass[] = "123456789";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

DHT dht(D3, DHT11);
BlynkTimer timer;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int analogInPin = A0;
float calibration = 0.40;
float voltage;
int batteryPercent;

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(auth, ssid, pass);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  Blynk.run();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int sensorValue = analogRead(analogInPin);
  voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration);
  batteryPercent = mapfloat(voltage, 2.8, 4.2, 0, 100);

  batteryPercent = constrain(batteryPercent, 1, 100);

  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, voltage);
  Blynk.virtualWrite(V4, batteryPercent);

  if (batteryPercent <= 30) {
    Blynk.logEvent("battery_low", "Battery level below 30%");
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");

  display.setCursor(0, 20);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");

  display.setCursor(0, 40);
  display.print("Battery: ");
  display.print(batteryPercent);
  display.print(" %");

  display.display();
  delay(1500);
}
