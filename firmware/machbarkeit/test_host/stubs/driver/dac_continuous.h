// Ersatz mit Typen und Prototypen aus ESP-IDF v5.3 esp_driver_dac (nur Syntaxprüfung)
#pragma once
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
typedef enum { DAC_CHANNEL_MASK_CH0 = 1, DAC_CHANNEL_MASK_CH1 = 2, DAC_CHANNEL_MASK_ALL = 3 } dac_channel_mask_t;
typedef enum { DAC_CHANNEL_MODE_SIMUL, DAC_CHANNEL_MODE_ALTER } dac_continuous_channel_mode_t;
typedef enum { DAC_DIGI_CLK_SRC_APLL = 1, DAC_DIGI_CLK_SRC_APB = 2, DAC_DIGI_CLK_SRC_DEFAULT = 2 } dac_continuous_digi_clk_src_t;
typedef struct dac_continuous_s* dac_continuous_handle_t;
typedef struct {
  dac_channel_mask_t chan_mask;
  uint32_t desc_num;
  size_t buf_size;
  uint32_t freq_hz;
  int8_t offset;
  dac_continuous_digi_clk_src_t clk_src;
  dac_continuous_channel_mode_t chan_mode;
} dac_continuous_config_t;
esp_err_t dac_continuous_new_channels(const dac_continuous_config_t* cont_cfg, dac_continuous_handle_t* ret_handle);
esp_err_t dac_continuous_enable(dac_continuous_handle_t handle);
esp_err_t dac_continuous_disable(dac_continuous_handle_t handle);
esp_err_t dac_continuous_del_channels(dac_continuous_handle_t handle);
esp_err_t dac_continuous_write_cyclically(dac_continuous_handle_t handle, uint8_t* buf, size_t buf_size, size_t* bytes_loaded);
