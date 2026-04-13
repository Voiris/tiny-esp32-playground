#include <algorithm>
#include <random>
#include <array>
#include "esp_system.h"

const int D[7] = {19, 17, 14, 13, 5, 23, 27};
const int D_dp = 12;
const int G[4] = {18, 26, 25, 16};

const int trigPin = 32;
const int echoPin = 33;

unsigned char num[17][8] =
{
 //a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0},     //0
  {0, 1, 1, 0, 0, 0, 0},     //1
  {1, 1, 0, 1, 1, 0, 1},     //2
  {1, 1, 1, 1, 0, 0, 1},     //3
  {0, 1, 1, 0, 0, 1, 1},     //4
  {1, 0, 1, 1, 0, 1, 1},     //5
  {1, 0, 1, 1, 1, 1, 1},     //6
  {1, 1, 1, 0, 0, 0, 0},     //7
  {1, 1, 1, 1, 1, 1, 1},     //8
  {1, 1, 1, 1, 0, 1, 1},     //9
};

typedef struct {
  uint8_t value;
  bool dot;
} Digit;
Digit digits[4];

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 7; i++) pinMode(D[i],OUTPUT);
  pinMode(D_dp,OUTPUT);

  for (int i = 0; i < 4; i++) pinMode(G[i],OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  while (!Serial);
  Serial.print("CPU Frequency: ");
  Serial.print(getCpuFrequencyMhz());
  Serial.println(" MHz");
  Serial.println("Ready!");
}

void loop() {
  float distance = readSensorData();
  displayFloat(distance);
  for (int i = 0; i < 5000; i++) display();
}

void displayFloat(float value) {
  int intPart = static_cast<int>(value);
  value -= intPart;
  if (intPart > 9999) intPart = 9999;
  
  int intDigitsCount = countDigits(intPart);

  for (int i = intDigitsCount - 1; i >= 0; i--) {
    digits[i] = {intPart % 10, false};
    intPart /= 10;
  }

  if (intDigitsCount != 4) digits[intDigitsCount - 1].dot = true;

  value -= intPart;

  for (int i = intDigitsCount; i < 4; i++) {
    value *= 10;
    int digit = static_cast<int>(value);
    digits[i] = {digit, false};
    value -= digit;
  }

  if (value > 0.5) {
    digits[3].value += 1;
  }
}

int countDigits(int n) {
  if (n == 0) return 1;
  return static_cast<int>(std::floor(std::log10(std::llabs(n)))) + 1;
}

std::array<int, 4> possitions = {0, 1, 2, 3};

std::random_device rd;
std::mt19937 g(rd());

void display() {
  std::shuffle(possitions.begin(), possitions.end(), g);
  for (int i = 0; i < 4; i++) {
    displayDigit(possitions[i], digits[possitions[i]].value, digits[possitions[i]].dot);
    delayMicroseconds(20);
  }
}

void displayDigit(uint8_t g, uint8_t n, bool dot) {
  for (int i = 0; i < 4; i++) digitalWrite(G[i], HIGH);
  for (int i = 0; i < 7; i++) digitalWrite(D[i], num[n][i]);
  digitalWrite(D_dp, dot);
  digitalWrite(G[g], LOW);
}

float readSensorData() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);

  unsigned long microsecond = pulseIn(echoPin, HIGH);

  float distance = microsecond / 29.00 / 2;
  
  return distance;
}
