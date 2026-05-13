#include <Arduino.h>

#include "audio.h"
#include "config.h"
#include "wifi_net.h"

enum WakeState {
  IDLE_LISTEN = 0,
  ACTIVE_WINDOW = 1
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!audio_init()) {
    Serial.println("Audio init failed.");
    while (true) {
      delay(1000);
    }
  }

  if (wifi_connect()) {
    Serial.println("WiFi connected.");
    wifi_print_status();
  } else {
    Serial.println("WiFi connect failed.");
    wifi_print_status();
  }
}

void loop() {
  static int16_t audio_frame[AUDIO_FRAME_SAMPLES];
  static int smooth_level = 0;
  static WakeState current_state = IDLE_LISTEN;
  static int trigger_frame_count = 0;
  static unsigned long last_active_time = 0;

  if (!audio_read_frame(audio_frame, AUDIO_FRAME_SAMPLES)) {
    return;
  }

  int level = audio_calculate_level(audio_frame, AUDIO_FRAME_SAMPLES);
  smooth_level = (smooth_level * 8 + level * 2) / 10;

  if (current_state == IDLE_LISTEN) {
    if (smooth_level >= WAKE_THRESHOLD) {
      trigger_frame_count++;
    } else {
      trigger_frame_count = 0;
    }

    if (trigger_frame_count >= WAKE_TRIGGER_FRAMES) {
      current_state = ACTIVE_WINDOW;
      last_active_time = millis();
      trigger_frame_count = 0;
      Serial.println("STATE:ACTIVE_WINDOW");
      if (udp_send_message("WAKE_DETECTED")) {
        Serial.println("UDP:WAKE_DETECTED sent");
      } else {
        Serial.println("UDP send failed");
      }
    }
  } else {
    if (smooth_level >= ACTIVE_THRESHOLD) {
      last_active_time = millis();
    }

    if (millis() - last_active_time >= ACTIVE_TIMEOUT_MS) {
      current_state = IDLE_LISTEN;
      Serial.println("STATE:IDLE_LISTEN");
    }
  }

  Serial.print(">env:");
  Serial.print(smooth_level);
  Serial.print(",state:");
  Serial.println((int)current_state);
}
