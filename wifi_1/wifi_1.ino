#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR SSID HERE";
const char* password = "YOUR PASSWORD HERE";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
}


void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  Serial.println("Connected");
  server.send(200, "text/html", "<h1>ESP32</h1>");
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
