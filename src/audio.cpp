#include "audio.h"

#include <driver/i2s.h>

#include "config.h"

static int16_t to_pcm16(int32_t raw32) {
  int32_t value = raw32 >> 14;

  if (value > 32767) {
    value = 32767;
  }

  if (value < -32768) {
    value = -32768;
  }

  return (int16_t)value;
}

bool audio_init() {
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 64};

  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_BCK_PIN,
      .ws_io_num = I2S_WS_PIN,
      .data_out_num = -1,
      .data_in_num = I2S_DATA_IN_PIN};

  esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    return false;
  }

  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    return false;
  }

  return true;
}

int16_t audio_read_pcm16_sample() {
  int32_t raw_sample = 0;
  size_t bytes_read = 0;

  esp_err_t err = i2s_read(I2S_PORT, &raw_sample, sizeof(raw_sample), &bytes_read, portMAX_DELAY);
  if (err != ESP_OK || bytes_read != sizeof(raw_sample)) {
    return 0;
  }

  return to_pcm16(raw_sample);
}

bool audio_read_frame(int16_t *buffer, int sample_count) {
  if (buffer == NULL || sample_count <= 0) {
    return false;
  }

  for (int i = 0; i < sample_count; i++) {
    buffer[i] = audio_read_pcm16_sample();
  }

  return true;
}

int audio_calculate_level(const int16_t *buffer, int sample_count) {
  if (buffer == NULL || sample_count <= 0) {
    return 0;
  }

  long sum = 0;
  for (int i = 0; i < sample_count; i++) {
    sum += buffer[i];
  }

  int mean = (int)(sum / sample_count);

  long abs_sum = 0;
  for (int i = 0; i < sample_count; i++) {
    int centered = buffer[i] - mean;
    if (centered < 0) {
      centered = -centered;
    }
    abs_sum += centered;
  }

  return (int)(abs_sum / sample_count);
}
