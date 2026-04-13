#include <Keypad.h>

typedef struct {
  int pin;
  byte channel;
} LEDChannel;

// RGB LED
const int LED_PINS_COUNT = 3;
const LEDChannel ledChannels[LED_PINS_COUNT] = {
  {18, 0}, // red
  {19, 1}, // green
  {23, 2}  // blue
};
int rgb[3] = {255, 255, 255};
int target_color = 0;

// Keypad
const int KEYPAD_ROWS     = 4;
const int KEYPAD_COLUMNS  = 4;
char keys[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
byte rowPins[KEYPAD_ROWS] = {5, 13, 12, 14};
byte colPins[KEYPAD_COLUMNS] = {16, 17, 25, 26};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLUMNS);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < LED_PINS_COUNT; i++) {
    ledcAttachChannel(ledChannels[i].pin, 1000, 8, ledChannels[i].channel);
  }
  setColor(255, 255, 255);
  while(!Serial);
  Serial.println("Ready!");
}

void loop() {
  char keyPressed = keypad.getKey();
  if (keyPressed) {
    Serial.print("Key pressed: ");
    Serial.println(keyPressed);
    switch (keyPressed) {
      case '1':
        handleNumber(1);
        break;
      case '2':
        handleNumber(2);
        break;
      case '3':
        handleNumber(3);
        break;
      case '4':
        handleNumber(4);
        break;
      case '5':
        handleNumber(5);
        break;
      case '6':
        handleNumber(6);
        break;
      case '7':
        handleNumber(7);
        break;
      case '8':
        handleNumber(8);
        break;
      case '9':
        handleNumber(9);
        break;
      case '*':
        setColor(0, 0, 0);
        break;
      case '#':
        setColor(255, 255, 255);
        break;
      case 'A':
        target_color = 0;
        break;
      case 'B':
        target_color = 1;
        break;
      case 'C':
        target_color = 2;
        break;
      case 'D':
        rgb[target_color] = 0;
        updateColor();
        break;
    }
  }
}

void handleNumber(int number) {
  int value = rgb[target_color];
  value *= 10;
  value += number;
  if (value > 255) {
    value = 255;
  }
  rgb[target_color] = value;
  Serial.print(rgb[0]);
  Serial.print(rgb[1]);
  Serial.println(rgb[2]);
  updateColor();
}

void setColor(int red, int green, int blue) {
  rgb[0] = red;
  rgb[1] = green;
  rgb[2] = blue;
  updateColor();
}

void updateColor() {
  ledcWriteChannel(ledChannels[0].channel, rgb[0]);
  ledcWriteChannel(ledChannels[1].channel, rgb[1]);
  ledcWriteChannel(ledChannels[2].channel, rgb[2]);
}
