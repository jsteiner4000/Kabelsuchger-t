// Ersatz mit den Signaturen aus arduino-esp32 libraries/ESP_I2S/src/ESP_I2S.h (nur Syntaxprüfung)
#pragma once
#include <stdint.h>
#include <stddef.h>
typedef enum { I2S_MODE_STD, I2S_MODE_TDM, I2S_MODE_PDM_TX, I2S_MODE_PDM_RX } i2s_mode_t;
typedef enum { I2S_DATA_BIT_WIDTH_8BIT = 8, I2S_DATA_BIT_WIDTH_16BIT = 16, I2S_DATA_BIT_WIDTH_24BIT = 24, I2S_DATA_BIT_WIDTH_32BIT = 32 } i2s_data_bit_width_t;
typedef enum { I2S_SLOT_MODE_MONO = 1, I2S_SLOT_MODE_STEREO = 2 } i2s_slot_mode_t;
typedef enum { I2S_ROLE_MASTER, I2S_ROLE_SLAVE } i2s_role_t;
class I2SClass {
 public:
  void setPins(int8_t bclk, int8_t ws, int8_t dout, int8_t din = -1, int8_t mclk = -1);
  bool begin(i2s_mode_t mode, uint32_t rate, i2s_data_bit_width_t bits_cfg, i2s_slot_mode_t ch,
             int8_t slot_mask = -1, i2s_role_t role = I2S_ROLE_MASTER);
  size_t readBytes(char* buffer, size_t size);
};
