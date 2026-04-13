#include <Wire.h>
#include <RDA5807M.h>

RDA5807M radio;

#define FIX_BAND RADIO_BAND_FM  ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION 9280        ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME 3           ///< The volume that will be set by this sketch is level 4.

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Wire.begin();
  Serial.println("RDA5807M Radio...");
  delay(200);

  radio.debugEnable(true);
  radio._wireDebug(false);

  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_100);   // for EUROPE
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);  // for EUROPE

  if (!radio.initWire(Wire)) {
    Serial.println("no radio chip found.");
    while(true)
    {
    };
  };

  radio.setBandFrequency(FIX_BAND, FIX_STATION);
  radio.setMono(false);
  radio.setMute(false);
  radio.setVolume(FIX_VOLUME);
}

unsigned long previousMillis = 0;
const unsigned long interval = 3000; // 3 секунды

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    char s[12];
    radio.formatFrequency(s, sizeof(s));
    Serial.print("Station: ");
    Serial.println(s);

    Serial.print("Radio: ");
    radio.debugRadioInfo();

    Serial.print("Audio: ");
    radio.debugAudioInfo();
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int freq = input.toInt();
    
    if (freq >= 8700 && freq <= 10800) {
      radio.setFrequency(freq);
      Serial.print("Переключено на ");
      Serial.print(freq / 100.0);
      Serial.println(" MHz");
    } else {
      Serial.println("Ошибка: частота вне диапазона 87.0-108.0 MHz");
    }
  }
}
