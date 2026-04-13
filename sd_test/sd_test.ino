#include <SPI.h>
#include "SdFat.h"

#define SPI_MISO 10 // 19
#define SPI_MOSI 11 // 23
#define SPI_SCK  12 // 18
#define SD_CS    13 // 5

SPIClass sdSPI(HSPI);
SdFat sd;

// Конфигурация для SdFat
#define SD_CONFIG SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(4), &sdSPI)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  sdSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);

  Serial.println("Initializing SD card...");
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
    return;
  }
  
  Serial.println("Success! Files on card:");
  sd.ls("/", LS_R | LS_SIZE);
}

void loop() {
}
