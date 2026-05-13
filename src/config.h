#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <driver/i2s.h>

static const i2s_port_t I2S_PORT = I2S_NUM_0;

static const uint32_t SAMPLE_RATE = 16000;
static const uint32_t RECORD_SECONDS = 5;
static const uint32_t RECORD_SAMPLES = SAMPLE_RATE * RECORD_SECONDS;
static const int AUDIO_FRAME_SAMPLES = 128;
static const int WAKE_THRESHOLD = 1200;
static const int ACTIVE_THRESHOLD = 800;
static const int WAKE_TRIGGER_FRAMES = 6;
static const unsigned long ACTIVE_TIMEOUT_MS = 5000;

static const char WIFI_SSID[] = "YOUR_WIFI_NAME";
static const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
static const char UDP_TARGET_IP[] = "192.168.1.100";
static const uint16_t UDP_TARGET_PORT = 9000;

static const int I2S_WS_PIN = 4;
static const int I2S_BCK_PIN = 5;
static const int I2S_DATA_IN_PIN = 6;

#ifdef __has_include
#if __has_include("config.local.h")
#include "config.local.h"
#endif
#endif

#endif
