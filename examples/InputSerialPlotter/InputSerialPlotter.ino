/** 
 * ESP32 I2S Serial Plotter Example.
 * Updated pins: WS=4, SCK=5, SD=6
 */

#include <driver/i2s.h>

const i2s_port_t I2S_PORT = I2S_NUM_0;

void setup() {
  Serial.begin(9600);
  Serial.println("Configuring I2S...");
  esp_err_t err;

  // I2S 配置
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // 接收模式
      .sample_rate = 16000,                         // 16KHz采样率
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 8
  };

  // ====================== 这里已经改成你的引脚 ======================
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 5,    // SCK → GPIO5
      .ws_io_num = 4,     // WS  → GPIO4
      .data_out_num = -1, // 不使用输出
      .data_in_num = 6    // SD  → GPIO6
  };
  // ================================================================

  // 安装驱动
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }

  // 设置引脚
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
  Serial.println("I2S driver installed.");
}

void loop() {
  int32_t sample = 0;
  int bytes_read = i2s_pop_sample(I2S_PORT, (char *)&sample, portMAX_DELAY);
  if (bytes_read > 0) {
    Serial.println(sample);
  }
}