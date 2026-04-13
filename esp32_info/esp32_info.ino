#include "esp_system.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("===== ESP32 Info =====");
  Serial.print("Chip revision: "); Serial.println(ESP.getChipRevision());
  Serial.print("CPU cores: "); Serial.println(ESP.getChipCores());
  Serial.print("CPU freq: "); Serial.println(ESP.getCpuFreqMHz());
  Serial.print("Flash size: "); Serial.println(ESP.getFlashChipSize());
  Serial.print("Free heap: "); Serial.println(ESP.getFreeHeap());
  Serial.print("PSRAM: "); Serial.println(ESP.getPsramSize());
}

void loop() {}
