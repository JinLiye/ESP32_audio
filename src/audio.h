#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

bool audio_init();
int16_t audio_read_pcm16_sample();
bool audio_read_frame(int16_t *buffer, int sample_count);
int audio_calculate_level(const int16_t *buffer, int sample_count);

#endif
