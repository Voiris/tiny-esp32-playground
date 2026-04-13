#include <Adafruit_NeoPixel.h>

constexpr uint8_t RGB_LED_PIXEL_PIN = 48;
constexpr uint8_t RGB_LED_PIXELS_COUNT = 1;

Adafruit_NeoPixel rgb_led(RGB_LED_PIXELS_COUNT, RGB_LED_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

constexpr uint8_t MIN_IN_BRIGHTNESS = 0;
constexpr uint8_t MAX_IN_BRIGHTNESS = 100;
constexpr uint8_t MIN_OUT_BRIGHTNESS = 0;
constexpr uint8_t MAX_OUT_BRIGHTNESS = 255;
constexpr uint8_t BRIGHTNESS = 20; // 0-100 (%)

uint32_t RED_COLOR    = rgb_led.Color(255, 0, 0);
uint32_t ORANGE_COLOR = rgb_led.Color(255, 127, 0);
uint32_t YELLOW_COLOR = rgb_led.Color(255, 255, 0);
uint32_t GREEN_COLOR  = rgb_led.Color(0, 255, 0);
uint32_t CYAN_COLOR   = rgb_led.Color(0, 0, 255);
uint32_t INDIGO_COLOR = rgb_led.Color(75, 0, 130);
uint32_t VIOLET_COLOR = rgb_led.Color(148, 0, 211);

constexpr uint8_t RAINBOW_COLORS_COUNT = 7;
uint32_t rainbow[RAINBOW_COLORS_COUNT] = {
  RED_COLOR,
  ORANGE_COLOR,
  YELLOW_COLOR,
  GREEN_COLOR,
  CYAN_COLOR,
  INDIGO_COLOR,
  VIOLET_COLOR
};

constexpr uint32_t COLOR_UPDATE_PERIOD_MILLIS = 100;
constexpr uint8_t NEXT_COLOR_STEP = 1;

int8_t current_color = 0;
uint32_t next_color_update_millis = 0;

void setup() {
  rgb_led.begin();
  rgb_led.setBrightness(map(BRIGHTNESS, MIN_IN_BRIGHTNESS, MAX_IN_BRIGHTNESS, MIN_OUT_BRIGHTNESS, MAX_OUT_BRIGHTNESS));
}

void loop() {
  if (next_color_update_millis < millis()) {
    current_color = (current_color + NEXT_COLOR_STEP + RAINBOW_COLORS_COUNT) % RAINBOW_COLORS_COUNT;
    for(int i = 0; i < RGB_LED_PIXELS_COUNT; i++) {
      rgb_led.setPixelColor(i, rainbow[current_color]);
    }
    rgb_led.show();
    next_color_update_millis = millis() + COLOR_UPDATE_PERIOD_MILLIS;
  }
}
