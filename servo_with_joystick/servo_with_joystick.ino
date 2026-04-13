#include <ESP32Servo.h>

// Joystick
const int VRx = 36;
const int VRy = 39;
const int SW = 26;

const int PASSIVE_CALIBRATION_TIME_PER_READ = 10;
const int PASSIVE_CALIBRATION_READS_COUNT = 100;
const int ACTIVE_CALIBRATION_TIME_PER_READ = 1;
const int ACTIVE_CALIBRATION_READS_COUNT = 5000;

typedef struct {
  int dx;
  int dy;
} Offset;
typedef struct {
  float dx;
  float dy;
} OffsetRatio;

class Calibration {
private:
  int maxDeadZoneX;
  int minDeadZoneX;
  int maxDeadZoneY;
  int minDeadZoneY;
  int maxXOffset;
  int minXOffset;
  int maxYOffset;
  int minYOffset;

  int computeOffset(int value, int minDeadZone, int maxDeadZone) const {
      if (value > maxDeadZone) return value - maxDeadZone;
      if (value < minDeadZone) return value - minDeadZone;
      return 0;
  }
  int computeXOffset(int x) const { return computeOffset(x, minDeadZoneX, maxDeadZoneX); }
  int computeYOffset(int y) const { return computeOffset(y, minDeadZoneY, maxDeadZoneY); }

public:
  Calibration() {}
  Calibration(int maxDriftX, int minDriftX, int maxDriftY, int minDriftY, int maxX, int minX, int maxY, int minY) {
    int medianX = (maxDriftX + minDriftX) / 2;
    int medianY = (maxDriftY + minDriftY) / 2;
    int offsetX = (maxDriftX - minDriftX) / 2 + 1;
    int offsetY = (maxDriftY - minDriftY) / 2 + 1;

    this->maxDeadZoneX = medianX + offsetX * 2;
    this->minDeadZoneX = medianX - offsetX * 2;
    this->maxDeadZoneY = medianY + offsetY * 2;
    this->minDeadZoneY = medianY - offsetY * 2;
    this->maxXOffset = maxX - this->maxDeadZoneX;
    this->minXOffset = this->minDeadZoneX - minX;
    this->maxYOffset = maxY - this->maxDeadZoneY;
    this->minYOffset = this->minDeadZoneY - minY;
  }

  Offset computeOffsetFrom(int x, int y) const {
    Offset o = {this->computeXOffset(x), this->computeYOffset(y)};
    return o;
  }
  OffsetRatio computeOffsetRatio(Offset o) const {
    float dx;
    float dy;
    if (o.dx > 0) {
      dx = static_cast<float>(o.dx) / static_cast<float>(this->maxXOffset);
    }
    else if (o.dx < 0) {
      dx = static_cast<float>(o.dx) / static_cast<float>(this->minXOffset);
    }
    else {
      dx = 0.0;
    }
    if (o.dy > 0) {
      dy = static_cast<float>(o.dy) / static_cast<float>(this->maxYOffset);
    }
    else if (o.dy < 0) {
      dy = static_cast<float>(o.dy) / static_cast<float>(this->minYOffset);
    }
    else {
      dy = 0.0;
    }
    OffsetRatio offset_ratio = {dx, dy};
    return offset_ratio;
  }
};

Calibration calibration;

// RGB LED
typedef struct {
  int pin;
  byte channel;
} LEDChannel;

const int LED_PINS_COUNT = 3;
const LEDChannel ledChannels[LED_PINS_COUNT] = {
  {5,  0}, // red
  {13, 1}, // green
  {12, 2}  // blue
};

// Servo
Servo servo;
const float MAX_OFFSET_ANGLE = 45;
const int SERVO_PIN = 4;
const int minPulseWidth = 500; // 0.5 ms
const int maxPulseWidth = 2500; // 2.5 ms

const int START_ANGLE = 45;
int current_angle = START_ANGLE;

void setup() {
  Serial.begin(115200);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);

  servo.attach(SERVO_PIN, minPulseWidth, maxPulseWidth);c

  for (int i = 0; i < LED_PINS_COUNT; i++) {
    ledcAttachChannel(ledChannels[i].pin, 1000, 8, ledChannels[i].channel);
  }
  setColor(0, 0, 0);
  while(!Serial);
  Serial.println("Calibration...");

  int maxDriftX = analogRead(VRx);
  int minDriftX = maxDriftX;
  int maxX = maxDriftX;
  int minX = maxDriftX;
  int maxDriftY = analogRead(VRy);
  int minDriftY = maxDriftY;
  int maxY = maxDriftY;
  int minY = maxDriftY;

  setColor(128, 0, 0);
  for (int i = 0; i < PASSIVE_CALIBRATION_READS_COUNT; i++) {
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
    delay(PASSIVE_CALIBRATION_TIME_PER_READ);
  }
  
  setColor(0, 0, 128);
  for (int i = 0; i < ACTIVE_CALIBRATION_READS_COUNT; i++) {
    int x = analogRead(VRx);
    int y = analogRead(VRy);
    if (x > maxX) {
      maxX = x;
    }
    else if (x < minX) {
      minX = x;
    }
    if (y > maxY) {
      maxY = y;
    }
    else if (y < minY) {
      minY = y;
    }
    delay(ACTIVE_CALIBRATION_TIME_PER_READ);
  }

  calibration = Calibration(maxDriftX, minDriftX, maxDriftY, minDriftY, maxX, minX, maxY, minY);

  setColor(0, 128, 0);

  Serial.println("Ready!");
}

void loop() {
  int x = analogRead(VRx);
  int y = analogRead(VRy);
  Offset o = calibration.computeOffsetFrom(x, y);
  int newAngle;
  if (o.dx != 0) {
    OffsetRatio offset_ratio = calibration.computeOffsetRatio(o);
    int angle_from_start = static_cast<int>(MAX_OFFSET_ANGLE * offset_ratio.dx);
    newAngle = START_ANGLE + angle_from_start;
  }
  else {
    newAngle = START_ANGLE;
  }
  updateServoAngle(newAngle);
}

void updateServoAngle(int newAngle) {
  if (current_angle != newAngle) {
    servo.write(newAngle);
    current_angle = newAngle;
  }
}

void setColor(int red, int green, int blue) {
  ledcWriteChannel(ledChannels[0].channel, red);
  ledcWriteChannel(ledChannels[1].channel, green);
  ledcWriteChannel(ledChannels[2].channel, blue);
}
