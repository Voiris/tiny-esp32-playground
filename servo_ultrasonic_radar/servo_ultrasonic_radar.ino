#include <ESP32Servo.h>

constexpr uint32_t BAUD_RATE = 115200;

// Servo
constexpr uint8_t MIN_ANGLE = 0;
constexpr uint8_t MAX_ANGLE = 180;
constexpr uint8_t ANGLE_BETWEEN_SCANS = 3;
constexpr uint8_t SERVO_PIN = 4;
constexpr uint16_t PERIOD_HERTZ = 50;
constexpr uint32_t SCAN_DELAY_MICROS = 500;

int16_t current_angle = MIN_ANGLE;
int8_t direction = 1;
uint32_t last_scan_micros = 0;
Servo servo;

// Ultrasonic
constexpr uint8_t TRIG_PIN = 13;
constexpr uint8_t ECHO_PIN = 12;
constexpr float TRIGGER_DISTANCE = 15.0;

// Buzzer
constexpr uint8_t BUZZER_PIN = 16;

void setup() {
  Serial.begin(BAUD_RATE);

  // Servo
  servo.attach(SERVO_PIN, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  servo.setPeriodHertz(PERIOD_HERTZ);
  servo.write(current_angle);
  delay(10);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  while (!Serial);

  Serial.println("Ready!");
}

void loop() {
  if (last_scan_micros + SCAN_DELAY_MICROS <= micros()) {
    current_angle += direction * ANGLE_BETWEEN_SCANS;
    if (current_angle >= MAX_ANGLE) {
      current_angle = MAX_ANGLE;
      direction = -direction;
    }
    else if (current_angle <= MIN_ANGLE) {
      current_angle = MIN_ANGLE;
      direction = -direction;
    }
    servo.write(current_angle);
    scan();
    last_scan_micros = micros();
  }
}

float ultrasonicRead() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, LOW);
  
  uint32_t microseconds = pulseIn(ECHO_PIN, HIGH);

  float distance = microseconds / 29.00 / 2.0;

  return distance;
}

void scan() {
  float distance = ultrasonicRead();
  if (distance <= TRIGGER_DISTANCE) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}
