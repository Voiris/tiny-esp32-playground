#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int VRx = 36;
const int VRy = 39;
const int SW = 26;

const int CALIBRATION_TIME_PER_READ = 10;
const int CALIBRATION_READS_COUNT = 10;

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

const int OLED_RESET = -1;
const int SCREEN_ADDRESS = 0x3C;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int gx = SCREEN_WIDTH / 2, gy = SCREEN_HEIGHT / 2;
const int STICK_X_PER_SCREEN_X = 500;
const int STICK_Y_PER_SCREEN_Y = 700;

typedef struct {
  int dx;
  int dy;
} Offset;

class Calibration {
private:
  int maxDeadZoneX;
  int minDeadZoneX;
  int maxDeadZoneY;
  int minDeadZoneY;

  int computeOffset(int value, int minDeadZone, int maxDeadZone) const {
      if (value > maxDeadZone) return value - maxDeadZone;
      if (value < minDeadZone) return value - minDeadZone;
      return 0;
  }
  int computeXOffset(int x) const { return computeOffset(x, minDeadZoneX, maxDeadZoneX); }
  int computeYOffset(int y) const { return computeOffset(y, minDeadZoneY, maxDeadZoneY); }

public:
  Calibration() {}
  Calibration(int maxDriftX, int minDriftX, int maxDriftY, int minDriftY) {
    int medianX = (maxDriftX + minDriftX) / 2;
    int medianY = (maxDriftY + minDriftY) / 2;
    int offsetX = (maxDriftX - minDriftX) / 2 + 1;
    int offsetY = (maxDriftY - minDriftY) / 2 + 1;

    this->maxDeadZoneX = medianX + offsetX * 2;
    this->minDeadZoneX = medianX - offsetX * 2;
    this->maxDeadZoneY = medianY + offsetY * 2;
    this->minDeadZoneY = medianY - offsetY * 2;
  }

  Offset computeOffsetFrom(int x, int y) const {
    Offset o = {this->computeXOffset(x), this->computeYOffset(y)};
    return o;
  }
};

Calibration calibration;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  updateScreen();

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);
  
  while (!Serial);
  Serial.println("Calibration...");

  int maxDriftX = analogRead(VRx);
  int minDriftX = maxDriftX;
  int maxDriftY = analogRead(VRy);
  int minDriftY = maxDriftY;

  for (int i = 0; i < CALIBRATION_READS_COUNT; i++) {
    int x = analogRead(VRx);
    int y = analogRead(VRy);
    if (x > maxDriftX) {
      maxDriftX = x;
    }
    else if (x < minDriftX) {
      minDriftX = x;
    }
    if (y > maxDriftY) {
      maxDriftY = y;
    }
    else if (y < minDriftY) {
      minDriftY = y;
    }
    delay(CALIBRATION_TIME_PER_READ);
  }

  calibration = Calibration(maxDriftX, minDriftX, maxDriftY, minDriftY);

  Serial.println("Ready!");
}

void updateScreen() {
  display.clearDisplay();
  display.drawRect(gx, gy, 2, 2, WHITE);
  display.display();
}

void loop() {
  int x = analogRead(VRx);
  int y = analogRead(VRy);
  Offset o = calibration.computeOffsetFrom(x, y);
  gx += o.dx / STICK_X_PER_SCREEN_X;
  gy -= o.dy / STICK_Y_PER_SCREEN_Y;
  updateScreen();
  delay(10);
}
