
  //i used a esp32-C3 and a display SSD1306
  //all the library you will need
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// oled display wiring, and dimension (i have a SSD1306 128*64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 8         //wiring
#define OLED_SCL 9         //wiring
#define OLED_ADDR 0x3C     //code behind the mini display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// wireless config.
const char* ssid = "home wifi ssid";              //modify this!!!
const char* password = "home wifi password";     //modify this!!!
const char* shellyIP = "local shelly ip";       //modify this!!!
const unsigned long updateInterval = 1500;     //modify this if you want, now 1.5 second after refreshing againt the display


unsigned long lastUpdate = 0;
float power1 = 0;
float power2 = 0;

void setup() {
  Serial.begin(115200);
  

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("connection to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

}

void loop() {
  if (millis() - lastUpdate >= updateInterval) {
    if (WiFi.status() == WL_CONNECTED) {
      String payload = getShellyData();
      if (!payload.isEmpty()) {
        parsePowerValues(payload);
      }
    }
    updateDisplay();
    lastUpdate = millis();
  }
}

String getShellyData() {
  HTTPClient http;
  String url = "http://" + String(shellyIP) + "/status";
  http.begin(url);
  
  if (http.GET() == HTTP_CODE_OK) {
    String payload = http.getString();
    http.end();
    return payload;
  }
  http.end();
  return "";
}

void parsePowerValues(String json) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  
  if (!error) {
    power1 = fabs(doc["emeters"][0]["power"].as<float>());
    power2 = fabs(doc["emeters"][1]["power"].as<float>());
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  // top text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Shelly Power Monitor");

  // line
  display.drawLine(0, 12, 128, 12, SSD1306_WHITE);

  // power meter
  display.setTextSize(1.8);
  display.setCursor(0, 23);
  display.print("power1: ");
  display.print(power1, 1);
  display.print(" W");

  display.setCursor(0, 40);
  display.print("power2: ");
  display.print(power2, 1);
  display.print(" W");

    //EXTRA
  //display.setCursor(0, 53);
  //display.print("difference: ");
  //display.print(power2-power1, 1);
  //display.print(" W");
 
  display.display();
}