#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define MPU_ADDRESS    0x68
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define RADIUS         1

#define SPRING  0.15f   // spring stiffness
#define DAMP    0.82f   // dampening
#define MAXV    8.0f    // max speed pixels/frame
#define PUSH    0.08f   // sensitivity to gyroscope

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const uint8_t FONT[10][7] = {
  {0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110},
  {0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110},
  {0b01110,0b10001,0b00001,0b00110,0b01000,0b10000,0b11111},
  {0b11111,0b00010,0b00100,0b00110,0b00001,0b10001,0b01110},
  {0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010},
  {0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110},
  {0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110},
  {0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000},
  {0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110},
  {0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100},
};

#define SC       2
#define DIGIT_Y  ((SCREEN_HEIGHT - 7 * SC) / 2)
const int DIGIT_X[4] = {37, 51, 71, 85};
#define COLON_X  63
#define MAX_DOTS 80

struct Particle {
  float x, y;
  float vx, vy;
  float tx, ty;
  bool settled;
};

Particle particles[MAX_DOTS];
int numParticles = 0;

float gx_offset = 0, gy_offset = 0;
int msk_h = 12, msk_m = 0, msk_s = 0;
unsigned long lastSecMs = 0;
int lastM = -1;

void calibrate() {
  const int N = 300;
  long sumX = 0, sumY = 0;
  for (int i = 0; i < N; i++) {
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS, 6, true);
    int16_t rx = Wire.read() << 8 | Wire.read();
    int16_t ry = Wire.read() << 8 | Wire.read();
                 Wire.read(); Wire.read();
    sumX += rx; sumY += ry;
    delay(2);
  }
  gx_offset = (sumX / N) / 131.0f;
  gy_offset = (sumY / N) / 131.0f;
}

void collectTargets(int digit, int col, int &idx) {
  const uint8_t *rows = FONT[digit];
  int ox = DIGIT_X[col], oy = DIGIT_Y;
  for (int row = 0; row < 7; row++)
    for (int bit = 0; bit < 5; bit++)
      if (rows[row] & (1 << (4 - bit)))
        if (idx < MAX_DOTS) {
          particles[idx].tx = ox + bit * SC + 1;
          particles[idx].ty = oy + row * SC + 1;
          idx++;
        }
}

void updateTargets(int h, int m) {
  int idx = 0;
  collectTargets(h / 10, 0, idx);
  collectTargets(h % 10, 1, idx);
  collectTargets(m / 10, 2, idx);
  collectTargets(m % 10, 3, idx);
  if (idx < MAX_DOTS) { particles[idx].tx = COLON_X; particles[idx].ty = DIGIT_Y + 4;  idx++; }
  if (idx < MAX_DOTS) { particles[idx].tx = COLON_X; particles[idx].ty = DIGIT_Y + 10; idx++; }
  numParticles = idx;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(20, 21);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();

  Wire.beginTransmission(MPU_ADDRESS); Wire.write(0x6B); Wire.write(0x00); Wire.endTransmission(true);
  Wire.beginTransmission(MPU_ADDRESS); Wire.write(0x1C); Wire.write(0x00); Wire.endTransmission(true);
  Wire.beginTransmission(MPU_ADDRESS); Wire.write(0x1B); Wire.write(0x00); Wire.endTransmission(true);

  calibrate();
  display.invertDisplay(true); delay(150); display.invertDisplay(false);

  msk_h = 12; msk_m = 0; msk_s = 0;

  randomSeed(analogRead(0));
  updateTargets(msk_h, msk_m);

  for (int i = 0; i < numParticles; i++) {
    particles[i].x  = particles[i].tx + (random(7) - 3);
    particles[i].y  = particles[i].ty + (random(7) - 3);
    particles[i].vx = 0; particles[i].vy = 0;
    particles[i].settled = false;
  }

  lastSecMs = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastSecMs >= 1000) {
    lastSecMs += 1000;
    msk_s += 12;
    if (++msk_s >= 60) { msk_s = 0; if (++msk_m >= 60) { msk_m = 0; if (++msk_h >= 24) msk_h = 0; } }
  }

  if (msk_m != lastM) {
    lastM = msk_m;
    int oldCount = numParticles;
    updateTargets(msk_h, msk_m);
    for (int i = 0; i < numParticles; i++) {
      particles[i].settled = false;
      if (i >= oldCount) {
        particles[i].x = SCREEN_WIDTH / 2; particles[i].y = SCREEN_HEIGHT / 2;
        particles[i].vx = 0; particles[i].vy = 0;
      }
    }
  }

  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 6, true);
  int16_t raw_gx = Wire.read() << 8 | Wire.read();
  int16_t raw_gy = Wire.read() << 8 | Wire.read();
               Wire.read(); Wire.read();

  float gx_ds = raw_gx / 131.0f - gx_offset;
  float gy_ds = raw_gy / 131.0f - gy_offset;
  float px = -gx_ds * PUSH;
  float py =  gy_ds * PUSH;

  for (int i = 0; i < numParticles; i++) {
    Particle &p = particles[i];

    if (p.settled) {
      if (abs(px) > 0.3f || abs(py) > 0.3f) {
        p.vx += px * 0.3f; p.vy += py * 0.3f;
        p.settled = false;
      }
      continue;
    }

    float dx = p.tx - p.x;
    float dy = p.ty - p.y;

    p.vx += dx * SPRING + px;
    p.vy += dy * SPRING + py;
    p.vx *= DAMP;
    p.vy *= DAMP;

    float spd = sqrt(p.vx * p.vx + p.vy * p.vy);
    if (spd > MAXV) { p.vx = p.vx / spd * MAXV; p.vy = p.vy / spd * MAXV; }

    p.x += p.vx;
    p.y += p.vy;

    if (p.x < RADIUS)                { p.x = RADIUS;                p.vx *= -0.4f; }
    if (p.x > SCREEN_WIDTH  - RADIUS){ p.x = SCREEN_WIDTH  - RADIUS; p.vx *= -0.4f; }
    if (p.y < RADIUS)                { p.y = RADIUS;                p.vy *= -0.4f; }
    if (p.y > SCREEN_HEIGHT - RADIUS){ p.y = SCREEN_HEIGHT - RADIUS; p.vy *= -0.4f; }

    if (dx*dx + dy*dy < 0.5f && abs(p.vx) < 0.15f && abs(p.vy) < 0.15f) {
      p.x = p.tx; p.y = p.ty; p.vx = 0; p.vy = 0; p.settled = true;
    }
  }

  display.clearDisplay();
  for (int i = 0; i < numParticles; i++)
    display.drawPixel((int)particles[i].x, (int)particles[i].y, WHITE);
  display.display();

  delay(16);
}
