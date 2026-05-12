#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <driver/i2s.h>

static const i2s_port_t I2S_PORT = I2S_NUM_0;

static const uint32_t SAMPLE_RATE = 16000;
static const uint32_t RECORD_SECONDS = 5;
static const uint32_t RECORD_SAMPLES = SAMPLE_RATE * RECORD_SECONDS;
static const int AUDIO_FRAME_SAMPLES = 128;

static const int I2S_WS_PIN = 4;
static const int I2S_BCK_PIN = 5;
static const int I2S_DATA_IN_PIN = 6;

#endif
