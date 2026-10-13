// Ersatz mit den verwendeten U8g2-Methoden (nur Syntaxprüfung)
#pragma once
#include <stdint.h>
#define U8X8_PIN_NONE 255
struct u8g2_cb_t {};
extern const u8g2_cb_t* U8G2_R0;
extern const uint8_t u8g2_font_6x10_tf[];
extern const uint8_t u8g2_font_logisoso16_tr[];
extern const uint8_t u8g2_font_logisoso18_tr[];
extern const uint8_t u8g2_font_logisoso24_tr[];
class U8G2_SH1106_128X64_NONAME_F_HW_I2C {
 public:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C(const u8g2_cb_t* rotation, uint8_t reset = U8X8_PIN_NONE,
                                     uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE);
  void setBusClock(uint32_t);
  bool begin();
  void clearBuffer();
  void sendBuffer();
  void setFont(const uint8_t*);
  int drawStr(int x, int y, const char* s);
  int drawUTF8(int x, int y, const char* s);
  int getStrWidth(const char* s);
  void drawHLine(int x, int y, int w);
  void drawVLine(int x, int y, int h);
  void drawPixel(int x, int y);
  void drawLine(int x0, int y0, int x1, int y1);
  void drawFrame(int x, int y, int w, int h);
  void drawBox(int x, int y, int w, int h);
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
};
