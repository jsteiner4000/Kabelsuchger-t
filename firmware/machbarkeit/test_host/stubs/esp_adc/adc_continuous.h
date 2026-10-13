// Ersatz mit Typen und Prototypen aus ESP-IDF v5.3 esp_adc (adc_continuous.h, hal/adc_types.h),
// Zielchip ESP32-S3 – nur für die Syntaxprüfung auf dem PC.
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
typedef enum { ADC_UNIT_1, ADC_UNIT_2 } adc_unit_t;
typedef enum { ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3, ADC_CHANNEL_4,
               ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9 } adc_channel_t;
typedef enum { ADC_CONV_SINGLE_UNIT_1 = 1, ADC_CONV_SINGLE_UNIT_2 = 2, ADC_CONV_BOTH_UNIT, ADC_CONV_ALTER_UNIT } adc_digi_convert_mode_t;
typedef enum { ADC_DIGI_OUTPUT_FORMAT_TYPE1, ADC_DIGI_OUTPUT_FORMAT_TYPE2 } adc_digi_output_format_t;
typedef struct { uint8_t atten; uint8_t channel; uint8_t unit; uint8_t bit_width; } adc_digi_pattern_config_t;
typedef struct {
  union {
    struct { uint32_t data: 12; uint32_t reserved12: 1; uint32_t channel: 4; uint32_t unit: 1; uint32_t reserved17_31: 14; } type2;
    uint32_t val;
  };
} adc_digi_output_data_t;
typedef struct adc_continuous_ctx_t* adc_continuous_handle_t;
typedef struct {
  uint32_t max_store_buf_size;
  uint32_t conv_frame_size;
  struct { uint32_t flush_pool: 1; } flags;
} adc_continuous_handle_cfg_t;
typedef struct {
  uint32_t pattern_num;
  adc_digi_pattern_config_t* adc_pattern;
  uint32_t sample_freq_hz;
  adc_digi_convert_mode_t conv_mode;
  adc_digi_output_format_t format;
} adc_continuous_config_t;
typedef struct { uint8_t* conv_frame_buffer; uint32_t size; } adc_continuous_evt_data_t;
typedef bool (*adc_continuous_callback_t)(adc_continuous_handle_t handle, const adc_continuous_evt_data_t* edata, void* user_data);
typedef struct { adc_continuous_callback_t on_conv_done; adc_continuous_callback_t on_pool_ovf; } adc_continuous_evt_cbs_t;
esp_err_t adc_continuous_new_handle(const adc_continuous_handle_cfg_t* hdl_config, adc_continuous_handle_t* ret_handle);
esp_err_t adc_continuous_config(adc_continuous_handle_t handle, const adc_continuous_config_t* config);
esp_err_t adc_continuous_register_event_callbacks(adc_continuous_handle_t handle, const adc_continuous_evt_cbs_t* cbs, void* user_data);
esp_err_t adc_continuous_start(adc_continuous_handle_t handle);
esp_err_t adc_continuous_read(adc_continuous_handle_t handle, uint8_t* buf, uint32_t length_max, uint32_t* out_length, uint32_t timeout_ms);
esp_err_t adc_continuous_stop(adc_continuous_handle_t handle);
esp_err_t adc_continuous_io_to_channel(int io_num, adc_unit_t* const unit_id, adc_channel_t* const channel);
