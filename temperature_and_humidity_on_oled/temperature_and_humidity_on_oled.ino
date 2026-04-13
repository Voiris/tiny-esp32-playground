#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"
#include "SPIFFS.h"

const char* ssid = "YOUR SSID HERE";
const char* password = "YOUR PASSWORD HERE";

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

const int OLED_RESET = -1;
const int SCREEN_ADDRESS = 0x3C;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTTYPE DHT11

uint8_t DHTPin = 13;

DHT dht(DHTPin, DHTTYPE);

const uint32_t FILE_SIZE_LIMIT = 1024 * 1024; // 1MB
File file;

const uint32_t DATA_COLLECT_TIMEOUT = 30000;
uint32_t lastCollect = 0;

WebServer server(80);

typedef struct {
  uint32_t timestamp;
  float temperature;
  float himidity;
} THData;

void setup() {
  Serial.begin(115200);

  // Display
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);
  // contrast
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(64); // 100% - 255, 50% - 128, 0% - 0
  
  // WiFi HTTP server
  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");

  // Sensors
  pinMode(DHTPin, INPUT);
  dht.begin();

  // File system
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount error");
    return;
  }
  // debug: SPIFFS.remove("/data.bin");
  // Creating file if it's not exists
  if (!SPIFFS.exists("/data.bin")) {
    File tmp = SPIFFS.open("/data.bin", "a");
    uint32_t tail_index = 0;
    tmp.write((uint8_t*)&tail_index, sizeof(uint32_t));
    tmp.close();
  }
  file = SPIFFS.open("/data.bin", "r+");
  if (!file) {
    Serial.println("File open error: /data.bin");
    return;
  }

  while (!Serial);
  Serial.println("Ready!");
}

void loop() {
  if (millis() > lastCollect + DATA_COLLECT_TIMEOUT) {
    THData data = {millis(), dht.readTemperature(), dht.readHumidity()};
    file.seek(0);
    uint32_t tail_index;
    file.read((uint8_t*)&tail_index, sizeof(uint32_t));
    file.seek(sizeof(uint32_t) + sizeof(THData) * tail_index);
    file.write((uint8_t*)&data, sizeof(THData));
    tail_index++;
    if (sizeof(uint32_t) + sizeof(THData) * tail_index > FILE_SIZE_LIMIT) {
      tail_index = 0;
    }
    file.seek(0);
    file.write((uint8_t*)&tail_index, sizeof(uint32_t));
    file.flush();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print(data.temperature);
    display.println("C");
    display.print(data.himidity);
    display.println("%");
    display.setTextSize(1);
    display.println(WiFi.localIP());
    display.display();
    file.seek(0);
    lastCollect = millis();
  }
  server.handleClient();
}

void handle_OnConnect() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  WiFiClient client = server.client();

  file.seek(sizeof(uint32_t));
  while (file.available()) {
    if (!client.connected()) break;
    THData data;
    file.read((uint8_t*)&data, sizeof(THData));
    server.sendContent(String(data.timestamp));
    server.sendContent(" | ");
    server.sendContent(String(data.temperature));
    server.sendContent("C | ");
    server.sendContent(String(data.himidity));
    server.sendContent("%<br>");
  }
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
