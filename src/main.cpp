#include <Arduino.h>

#include "audio.h"
#include "config.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!audio_init()) {
    Serial.println("Audio init failed.");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  static int16_t audio_frame[AUDIO_FRAME_SAMPLES];
  static int smooth_level = 0;

  if (!audio_read_frame(audio_frame, AUDIO_FRAME_SAMPLES)) {
    return;
  }

  int level = audio_calculate_level(audio_frame, AUDIO_FRAME_SAMPLES);
  smooth_level = (smooth_level * 8 + level * 2) / 10;

  Serial.print(">env:");
  Serial.println(smooth_level);
}
