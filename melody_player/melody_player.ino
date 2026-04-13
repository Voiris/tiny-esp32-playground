#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <vector>

// Notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

// Melodies
struct Melody {
  const char* name;
  std::vector<int> melody;
  uint32_t tempo;
};

std::vector<Melody> melodies;
uint32_t active_melody_idx = 0;
uint32_t active_note = 0;
uint32_t next_note_millis = 0;
bool melody_enabled = false;

// Passive Buzzer
const uint8_t BUZZER_PIN = 12;

// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// IR receiver
const uint8_t RECEIVER_PIN = 5;
IRrecv irrecv(RECEIVER_PIN);
decode_results results;

void setup() {
  // Melodies from https://github.com/robsoncouto/arduino-songs
  melodies.push_back({
    "Super Mario Bros",
    {
      // Super Mario Bros theme
      // Score available at https://musescore.com/user/2123/scores/2145
      // Theme by Koji Kondo
      
      NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
      NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, 
      NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
      NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
      NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
      REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
      NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 3
      NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
      NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
      REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

      
      REST,4, NOTE_G5,8, NOTE_FS5,8, NOTE_F5,8, NOTE_DS5,4, NOTE_E5,8,//7
      REST,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C4,8, REST,8, NOTE_A4,8, NOTE_C5,8, NOTE_D5,8,
      REST,4, NOTE_DS5,4, REST,8, NOTE_D5,-4,
      NOTE_C5,2, REST,2,

      REST,4, NOTE_G5,8, NOTE_FS5,8, NOTE_F5,8, NOTE_DS5,4, NOTE_E5,8,//repeats from 7
      REST,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C4,8, REST,8, NOTE_A4,8, NOTE_C5,8, NOTE_D5,8,
      REST,4, NOTE_DS5,4, REST,8, NOTE_D5,-4,
      NOTE_C5,2, REST,2,

      NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4,//11
      NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,

      NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,8, NOTE_E5,8,//13
      REST,1, 
      NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4,
      NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,
      NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,4,
      NOTE_G5,4, REST,4, NOTE_G4,4, REST,4, 
      NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 19
      
      NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
      NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
      REST,8, NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

      NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 19
      NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
      NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
      REST,8, NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

      NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,//23
      NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
      NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
      
      NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //26
      NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
      NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
      NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
      NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,

      NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,//repeats from 23
      NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
      NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
      
      NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //26
      NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
      NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
      NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
      NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,
      NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,8, NOTE_E5,8,
      REST,1,

      NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4, //33
      NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,
      NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,4,
      NOTE_G5,4, REST,4, NOTE_G4,4, REST,4, 
      NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
      NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
      NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
      
      NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //40
      NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
      NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
      NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
      NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,
    },
    200
  });
  melodies.push_back({
    "Doom",
    {
      // At Doom's Gate (E1M1)
      // Score available at https://musescore.com/pieridot/doom

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //1
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //5
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //9
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //13
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_FS3, -16, NOTE_D3, -16, NOTE_B2, -16, NOTE_A3, -16, NOTE_FS3, -16, NOTE_B2, -16, NOTE_D3, -16, NOTE_FS3, -16, NOTE_A3, -16, NOTE_FS3, -16, NOTE_D3, -16, NOTE_B2, -16,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //17
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //21
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_B3, -16, NOTE_G3, -16, NOTE_E3, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_B3, -16, NOTE_G4, -16, NOTE_B4, -16,

      NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8, //25
      NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_E3, 8, NOTE_F3, 8,
      NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8,
      NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, -2,

      NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8, //29
      NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_E3, 8, NOTE_F3, 8,
      NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8,
      NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16, NOTE_C4, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //33
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //37
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_CS3, 8, NOTE_CS3, 8, NOTE_CS4, 8, NOTE_CS3, 8, NOTE_CS3, 8, NOTE_B3, 8, NOTE_CS3, 8, NOTE_CS3, 8, //41
      NOTE_A3, 8, NOTE_CS3, 8, NOTE_CS3, 8, NOTE_G3, 8, NOTE_CS3, 8, NOTE_CS3, 8, NOTE_GS3, 8, NOTE_A3, 8,
      NOTE_B2, 8, NOTE_B2, 8, NOTE_B3, 8, NOTE_B2, 8, NOTE_B2, 8, NOTE_A3, 8, NOTE_B2, 8, NOTE_B2, 8,
      NOTE_G3, 8, NOTE_B2, 8, NOTE_B2, 8, NOTE_F3, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //45
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_B3, -16, NOTE_G3, -16, NOTE_E3, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_B3, -16, NOTE_G4, -16, NOTE_B4, -16,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //49
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //53
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_FS3, -16, NOTE_DS3, -16, NOTE_B2, -16, NOTE_FS3, -16, NOTE_DS3, -16, NOTE_B2, -16, NOTE_G3, -16, NOTE_D3, -16, NOTE_B2, -16, NOTE_DS4, -16, NOTE_DS3, -16, NOTE_B2, -16,

    // -/-

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //57
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //61
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_E4, -16, NOTE_B3, -16, NOTE_G3, -16, NOTE_G4, -16, NOTE_E4, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_D4, -16, NOTE_E4, -16, NOTE_G4, -16, NOTE_E4, -16, NOTE_G3, -16,  

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //65
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8, //69
      NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_E3, 8, NOTE_F3, 8,
      NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8,
      NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16, NOTE_C4, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //73
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //77
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //81
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //73
      NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
      NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
      NOTE_B3, -16, NOTE_G3, -16, NOTE_E3, -16, NOTE_B2, -16, NOTE_E3, -16, NOTE_G3, -16, NOTE_C4, -16, NOTE_B3, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_G3, -16, NOTE_E3, -16,  
    },
    225
  });
  melodies.push_back({
    "Nokia",
    {
      // Nokia Ringtone 
      // Score available at https://musescore.com/user/29944637/scores/5266155
      
      NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
      NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
      NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
      NOTE_A4, 2, 
    },
    180
  });
  melodies.push_back({
    "Tetris",
    {
      //Based on the arrangement at https://www.flutetunes.com/tunes.php?id=192
      
      NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
      NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
      NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
      NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST,4,

      REST,8, NOTE_D5, 4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
      NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
      NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
      NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,

      NOTE_E5,2, NOTE_C5,2,
      NOTE_D5,2, NOTE_B4,2,
      NOTE_C5,2, NOTE_A4,2,
      NOTE_B4,1,

      NOTE_E5,2, NOTE_C5,2,
      NOTE_D5,2, NOTE_B4,2,
      NOTE_C5,4, NOTE_E5,4, NOTE_A5,2,
      NOTE_GS5,1,

      NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
      NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
      NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
      NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST,4,

      REST,8, NOTE_D5, 4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
      REST,8, NOTE_E5, 4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
      REST,8, NOTE_B4, 4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
      REST,8, NOTE_C5, 4,  NOTE_A4,8,  NOTE_A4,4, REST, 4,
    },
    144
  });
  melodies.push_back({
    "Harry Potter",
    {
      // Hedwig's theme fromn the Harry Potter Movies
      // Socre from https://musescore.com/user/3811306/scores/4906610
      
      REST, 2, NOTE_D4, 4,
      NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
      NOTE_G4, 2, NOTE_D5, 4,
      NOTE_C5, -2, 
      NOTE_A4, -2,
      NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
      NOTE_F4, 2, NOTE_GS4, 4,
      NOTE_D4, -1, 
      NOTE_D4, 4,

      NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
      NOTE_G4, 2, NOTE_D5, 4,
      NOTE_F5, 2, NOTE_E5, 4,
      NOTE_DS5, 2, NOTE_B4, 4,
      NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
      NOTE_CS4, 2, NOTE_B4, 4,
      NOTE_G4, -1,
      NOTE_AS4, 4,
        
      NOTE_D5, 2, NOTE_AS4, 4,//18
      NOTE_D5, 2, NOTE_AS4, 4,
      NOTE_DS5, 2, NOTE_D5, 4,
      NOTE_CS5, 2, NOTE_A4, 4,
      NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
      NOTE_CS4, 2, NOTE_D4, 4,
      NOTE_D5, -1, 
      REST,4, NOTE_AS4,4,  

      NOTE_D5, 2, NOTE_AS4, 4,//26
      NOTE_D5, 2, NOTE_AS4, 4,
      NOTE_F5, 2, NOTE_E5, 4,
      NOTE_DS5, 2, NOTE_B4, 4,
      NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
      NOTE_CS4, 2, NOTE_AS4, 4,
      NOTE_G4, -1, 
    },
    144
  });
  melodies.push_back({
    "Never Gonna Give You Up",
    {
      // Never Gonna Give You Up - Rick Astley
      // Score available at https://musescore.com/chlorondria_5/never-gonna-give-you-up_alto-sax
      // Arranged by Chlorondria

      NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //1
      NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
      NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
      NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
      NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //repeat from 1
      NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
      NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
      NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
      REST,4, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_D5,8, NOTE_E5,8, NOTE_CS5,-8,
      NOTE_B4,16, NOTE_A4,2, REST,4, 

      REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,4, NOTE_A4,8, //7
      NOTE_A5,8, REST,8, NOTE_A5,8, NOTE_E5,-4, REST,4, 
      NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,8, REST,8,
      REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
      REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
      NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, REST,4,
      
      NOTE_D5,2, NOTE_E5,8, NOTE_FS5,8, NOTE_D5,8, //13
      NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, NOTE_A4,4,
      REST,2, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8,
      REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

      NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,-8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //18
      NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,8, NOTE_A4,8, NOTE_A4,8, 
      NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

      NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  //23
      NOTE_E5,4, NOTE_D5,2, REST,4,
      REST,8, NOTE_B4,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,4, REST,8,
      REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
      REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
      REST,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_D5,8,
      
      REST,8, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //29
      REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
      NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4, REST,8,
      REST,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,4, NOTE_E5,-4, 
      NOTE_D5,2, NOTE_D5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, 
      NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_A4,8, NOTE_A4,4,

      REST,-4, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //35
      REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

      NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //40
      NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
      NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      
      NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
      NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
      NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
      
      NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
      NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

      NOTE_E5,4, NOTE_D5,2, REST,4
    },
    114
  });
  melodies.push_back({
    "Marry Christmas",
    {
      // We Wish You a Merry Christmas
      // Score available at https://musescore.com/user/6208766/scores/1497501
      
      NOTE_C5,4, //1
      NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
      NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
      NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
      NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
      NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
      NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,

      NOTE_F5,2, NOTE_C5,4, //8 
      NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
      NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
      NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
      NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
      NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
      NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
      NOTE_F5,2, NOTE_C5,4,

      NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,//17
      NOTE_E5,2, NOTE_E5,4,
      NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,
      NOTE_C5,2, NOTE_A5,4,
      NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
      NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
      NOTE_F5,2, NOTE_C5,4, 
      NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
      NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
      
      NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8, //27
      NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
      NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
      NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
      NOTE_F5,2, NOTE_C5,4,
      NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,
      NOTE_E5,2, NOTE_E5,4,
      NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,
      
      NOTE_C5,2, NOTE_A5,4,//36
      NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
      NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
      NOTE_F5,2, NOTE_C5,4, 
      NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
      NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
      NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8, 
      NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
      
      NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,//45
      NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
      NOTE_F5,2, NOTE_C5,4,
      NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
      NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
      NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
      NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
      
      NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, //53
      NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
      NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
      NOTE_F5,2, REST,4
    },
    140
  });
  melodies.push_back({
    "Happy Birthday",
    {
      // Happy Birthday
      // Score available at https://musescore.com/user/8221/scores/26906

      NOTE_C4,4, NOTE_C4,8, 
      NOTE_D4,-4, NOTE_C4,-4, NOTE_F4,-4,
      NOTE_E4,-2, NOTE_C4,4, NOTE_C4,8, 
      NOTE_D4,-4, NOTE_C4,-4, NOTE_G4,-4,
      NOTE_F4,-2, NOTE_C4,4, NOTE_C4,8,

      NOTE_C5,-4, NOTE_A4,-4, NOTE_F4,-4, 
      NOTE_E4,-4, NOTE_D4,-4, NOTE_AS4,4, NOTE_AS4,8,
      NOTE_A4,-4, NOTE_F4,-4, NOTE_G4,-4,
      NOTE_F4,-2,
    },
    140
  });

  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial started");
  
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  updateDisplay();
  Serial.println("Display started");

  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Buzzer started");

  irrecv.enableIRIn();
  Serial.println("Reveiver started");
}

void loop() {
  // Buzzer
  if (melody_enabled && millis() > next_note_millis) {
    Melody& melody = melodies[active_melody_idx];
    if (melody.melody.size() <= active_note) {
      active_note = 0;
    }
    int wholenote = (60000 * 4) / melody.tempo;
    int divider = melody.melody[active_note + 1];
    int duration;
    if (divider > 0) {
      duration = wholenote / divider;
    }
    else if (divider < 0) {
      duration = wholenote / abs(divider);
      duration *= 1.5;
    }
    tone(BUZZER_PIN, melody.melody[active_note]);
    active_note = active_note + 2;
    next_note_millis = millis() + duration;
  }

  // IR receiver
  if (irrecv.decode(&results)) {
    switch (results.value) {
    // right
    case 0xFFC23D:
      noTone(BUZZER_PIN);
      active_note = 0;
      active_melody_idx++;
      if (active_melody_idx >= melodies.size()) {
        active_melody_idx = 0;
      }
      updateDisplay();
      break;
    // left
    case 0xFF22DD:
      noTone(BUZZER_PIN);
      active_note = 0;
      if (active_melody_idx == 0) {
        active_melody_idx = melodies.size() - 1;
      }
      else {
        active_melody_idx--;
      }
      updateDisplay();
      break;
    // ok (pause)
    case 0xFF02FD:
      noTone(BUZZER_PIN);
      melody_enabled = !melody_enabled;
      updateDisplay();
      break;
    }
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(melodies[active_melody_idx].name);
  if (!melody_enabled) {
    display.println("PAUSED");
  }
  display.display();
}
