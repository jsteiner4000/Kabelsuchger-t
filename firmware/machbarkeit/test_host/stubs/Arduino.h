// Minimaler Ersatz für Arduino.h – nur für die Syntaxprüfung der Sketche auf dem PC.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string>
#define INPUT_PULLUP 0x05
#define OUTPUT 0x03
#define LOW 0
#define HIGH 1
#define IRAM_ATTR
class String {
 public:
  String() {}
  String(const char* c) : s(c) {}
  String(const std::string& x) : s(x) {}
  unsigned int length() const { return (unsigned)s.size(); }
  char operator[](unsigned int i) const { return s[i]; }
  String& operator+=(char c) { s += c; return *this; }
  bool operator==(const char* c) const { return s == c; }
  String substring(unsigned int a) const { return a < s.size() ? String(s.substr(a)) : String(""); }
  String substring(unsigned int a, unsigned int b) const { return a < s.size() ? String(s.substr(a, b - a)) : String(""); }
  int indexOf(char c) const { size_t p = s.find(c); return p == std::string::npos ? -1 : (int)p; }
  float toFloat() const { return (float)atof(s.c_str()); }
  std::string s;
};
struct SerialStub {
  void begin(unsigned long) {}
  int available() { return 0; }
  int read() { return -1; }
  size_t printf(const char* f, ...) { va_list a; va_start(a, f); int n = vprintf(f, a); va_end(a); return n; }
  size_t println(const char* s = "") { return ::printf("%s\n", s); }
  size_t print(const char* s) { return ::printf("%s", s); }
};
extern SerialStub Serial;
unsigned long millis();
void delay(unsigned long);
void pinMode(int, int);
int digitalRead(int);
void digitalWrite(int, int);
// LEDC (arduino-esp32 3.x, esp32-hal-ledc.h)
bool ledcAttach(uint8_t pin, uint32_t freq, uint8_t resolution);
bool ledcWrite(uint8_t pin, uint32_t duty);
uint32_t ledcChangeFrequency(uint8_t pin, uint32_t freq, uint8_t resolution);
// ADC einfach (arduino-esp32 3.x, esp32-hal-adc.h)
typedef enum { ADC_0db, ADC_2_5db, ADC_6db, ADC_11db, ADC_ATTENDB_MAX } adc_attenuation_t;
void analogSetPinAttenuation(uint8_t pin, adc_attenuation_t attenuation);
uint32_t analogReadMilliVolts(uint8_t pin);
#include "esp_err.h"
// FreeRTOS
typedef int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL(x) (void)(x)
#define portEXIT_CRITICAL(x) (void)(x)
typedef void (*TaskFunction_t)(void*);
typedef void* TaskHandle_t;
int xTaskCreatePinnedToCore(TaskFunction_t, const char*, uint32_t, void*, unsigned, TaskHandle_t*, int);
void vTaskDelay(int);
