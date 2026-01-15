#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

const char* ssid = "Virijevic";
const char* password = "virijevic000";

#define LDR_PIN 23
#define DHTPIN 18
#define DHTTYPE DHT11
#define BUTTON_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Inicijalizacija objekata
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Promenljive za kontrolu ekrana
unsigned long displayTimer = 0;
bool isDisplayOn = false;
const long displayTimeout = 5000;

// JSON API
void handleJSON() {
  StaticJsonDocument<200> doc;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int darkStatus = digitalRead(LDR_PIN);

  doc["temp"] = isnan(t) ? 0 : t;
  doc["vlaga"] = isnan(h) ? 0 : h;
  doc["status"] = (darkStatus == HIGH) ? "NOC" : "DAN";

  String jsonResponse;
  serializeJson(doc, jsonResponse);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", jsonResponse);
}

// Crtanje na ekran
void azurirajEkran(float t, float h, int dark) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("IP: "); display.print(WiFi.localIP());
  
  display.drawFastHLine(0, 12, 128, WHITE);

  display.setCursor(0, 22);
  display.setTextSize(2);
  display.print(t, 1); display.print(" C");

  display.setCursor(0, 42);
  display.setTextSize(1);
  display.print("Vlaga: "); display.print((int)h); display.print("%");

  display.setCursor(0, 54);
  display.print("Status: "); display.print(dark == HIGH ? "NOC" : "DAN");
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED nije pronadjen!");
    for(;;);
  }

  // Povezivanje na WiFi
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("WiFi povezivanje...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Definisanje API rute
  server.on("/data", handleJSON);
  server.begin();

  // Prikaži IP adresu pre nego što se ekran ugasi
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("POVEZANO!");
  display.println(WiFi.localIP());
  display.display();
  delay(3000);
  
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void loop() {

  server.handleClient();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int darkStatus = digitalRead(LDR_PIN);

  // LOGIKA DUGMETA
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!isDisplayOn) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      isDisplayOn = true;
    }
    displayTimer = millis();
  }

  // OSVEŽAVANJE I GAŠENJE EKRANA
  if (isDisplayOn) {
    azurirajEkran(t, h, darkStatus);

    if (millis() - displayTimer > displayTimeout) {
      display.ssd1306_command(SSD1306_DISPLAYOFF);
      isDisplayOn = false;
    }
  }
}