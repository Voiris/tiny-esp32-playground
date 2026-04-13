#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "audio.h"

AudioFileSourcePROGMEM *file;
AudioGeneratorWAV *wav;
AudioOutputI2S *out;

void setup() {
  Serial.begin(115200);

  file = new AudioFileSourcePROGMEM(audio_wav, audio_wav_len);

  out = new AudioOutputI2S(0, 1);
  out->SetGain(4.0);

  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
}

void loop() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      Serial.println("Done");
    }
  }
}