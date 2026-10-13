// =============================================================================
// Kabelsuchgerät – Empfänger-Testfirmware für den Machbarkeitstest (Phase 3)
// -----------------------------------------------------------------------------
// Hardware:  ESP32-S3 (z. B. ESP32-S3 SuperMini) + PCM1808-Modul + 2 Vorverstärker
//            + H-Spule (Kanal L) + V-Spule oder E-Spitze (Kanal R) + optional OLED 1,3"
// Anleitung: docs/machbarkeitstest.md
//
// Was die Firmware macht:
//   - liest den PCM1808 per I²S (96 kHz, 24 bit, Stereo)
//   - wertet Ortungston, Richtungston und eine leere Nachbarfrequenz per
//     Lock-in aus (lockin.h)
//   - sucht die Sendefrequenz auf Knopfdruck und führt sie nach (AFC)
//   - gibt Werte für den seriellen Plotter der Arduino IDE aus und zeigt sie
//     auf dem OLED
//
// Arduino IDE:  Board "ESP32S3 Dev Module", "USB CDC On Boot: Enabled",
//               Bibliothek "U8g2" (von olikraus) über den Bibliotheksverwalter.
//               Serieller Monitor/Plotter: 115200 Baud.
//
// Bedienung (seriell, Zeichen + Enter) bzw. BOOT-Taste:
//   s  Frequenzsuche (Gerät dabei nah an die Leitung halten)   | BOOT lang
//   r  Referenz für die Stromrichtung setzen (nahe am Sender)  | BOOT kurz
//   f  nächster Frequenzsatz (Standard / Tief / Hoch)
//   t  Glättung umschalten (schnell 2 x 0,06 s / ruhig 2 x 0,3 s)
//   z  Stromzangen-Modus (Spule 5 cm neben eine einzelne Leitung)
//   c<I_mA> <r_cm>   Kalibrieren an bekanntem Strom, z. B. "c10 10"
//   k<Wert>          Kalibrierfaktor direkt setzen (µV pro nT und kHz)
//   l  Links/Rechts umdrehen     a  Frequenznachführung an/aus
//   v  ausführliche Ausgabe an/aus (1 Zeile pro Sekunde)       ?  Hilfe
// =============================================================================

#include <Arduino.h>
#include <ESP_I2S.h>
#include "lockin.h"

#define USE_OLED 1          // 0 = ohne Display
#if USE_OLED
#include <U8g2lib.h>
#endif

// ------------------------------------------------------------- Einstellungen
constexpr uint32_t FS = 96000;              // Abtastrate PCM1808 (Hz)
constexpr int BLOCK = FS / 50;              // 20 ms pro Auswerteblock
constexpr float BLOCK_SEC = (float)BLOCK / (float)FS;

// Pins (ESP32-S3 SuperMini; bei anderen Boards anpassen)
constexpr int PIN_MCLK = 4;                 // -> PCM1808 SCK (Systemtakt)
constexpr int PIN_BCK  = 5;                 // -> PCM1808 BCK
constexpr int PIN_WS   = 6;                 // -> PCM1808 LRC (LRCK)
constexpr int PIN_DIN  = 7;                 // <- PCM1808 OUT (DOUT)
constexpr int PIN_SDA  = 8;                 // OLED
constexpr int PIN_SCL  = 9;                 // OLED
constexpr int PIN_BTN  = 0;                 // BOOT-Taste

// Analogkette (für die Umrechnung in µV an der Spule)
constexpr float ADC_FS_VPEAK = 1.5f;        // PCM1808: 3 Vss Vollausschlag bei 5 V
constexpr float GAIN_L = 34.0f;             // Vorverstärker H-Kanal (1 + 33k/1k)
constexpr float GAIN_R = 34.0f;             // Vorverstärker R-Kanal
constexpr float ZANGE_R_M = 0.05f;          // Abstand Spule-Leitung im Zangen-Modus

// Kalibrierfaktor: µV (eff) an der Spule pro nT und pro kHz.
// Theorie für 1000 Wdg. auf 10 x 80 mm Ferrit: ca. 17,3. Nach Messung M3 ersetzen.
float calK = 17.3f;

// ------------------------------------------------------------- Frequenzsätze
// Müssen zu sender_test.ino passen (dort aus DAC-Takt / Tabellenlänge berechnet).
struct Satz {
  const char* name;
  const char* kurz;
  double fL;                 // Ortungston (Hz); Richtungston = fL/2
  bool cd;                   // Richtungston vorhanden?
  lockin::ScanStage grob, fein;
};
const Satz SAETZE[] = {
  {"Standard", "STD",  200000.0 / 24.0, true,  {60.0f, 3.0f, 8},   {4.0f, 0.4f, 60}},
  {"Tief",     "TIEF",  50000.0 / 48.0, true,  {6.0f, 0.5f, 50},   {0.6f, 0.2f, 125}},
  {"Hoch",     "HOCH", 400000.0 / 12.0, false, {250.0f, 10.0f, 2}, {15.0f, 1.0f, 25}},
};
constexpr int N_SAETZE = sizeof(SAETZE) / sizeof(SAETZE[0]);

// ------------------------------------------------------------- Objekte
I2SClass i2s;
lockin::Receiver rx;
lockin::Scanner scanner;
#if USE_OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, PIN_SCL, PIN_SDA);
#endif

// Austausch zwischen Rechen-Task (Kern 0) und Anzeige (Kern 1)
struct Shared {
  lockin::Result r;
  bool scanning = false;
  bool scanNew = false;
  double scanF = 0;
  float scanContrast = 0;
  bool clipL = false, clipR = false;
  bool noData = false;
};
Shared shared;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Befehle an den Rechen-Task
volatile bool cmdScan = false;
volatile int cmdSatz = -1;
volatile bool cmdTauSlow = false;
volatile bool cmdTauChanged = false;
volatile bool cmdAfc = true;

// Zustand der Anzeige
int satz = 0;
bool refSet = false;
float refDelta = 0;
bool zange = false;
bool verbose = false;
bool lrInvert = false;
bool tauSlow = false;

// ------------------------------------------------------------- Umrechnungen
float toUv(float amp, float gain) {                  // Spitzenwert (ADC-Einheiten) -> µV eff an der Spule
  return amp * ADC_FS_VPEAK / gain * 0.70710678f * 1e6f;
}
float dBuV(float uv) { return lockin::dB(uv); }      // dB bezogen auf 1 µV
float toNT(float uv, double f) { return uv / (calK * (float)(f / 1000.0)); }
float nTtoMA(float nT, float rM) { return 5.0f * nT * rM; }   // B = 0,2 nT*m/mA * I / r

// ------------------------------------------------------------- Rechen-Task
void dspTask(void*) {
  static int32_t buf[BLOCK * 2];
  const float* T = lockin::sinTab();
  int zeroBlocks = 0;
  for (;;) {
    // einen Block (L, R, L, R ...) einlesen
    size_t need = sizeof(buf);
    uint8_t* p = (uint8_t*)buf;
    while (need > 0) {
      size_t got = i2s.readBytes((char*)p, need);
      if (got == 0) { vTaskDelay(1); continue; }
      p += got;
      need -= got;
    }

    // Befehle übernehmen
    if (cmdSatz >= 0) {
      int s = cmdSatz;
      cmdSatz = -1;
      rx.begin(FS, SAETZE[s].fL, SAETZE[s].cd);
    }
    if (cmdTauChanged) { rx.setTau(cmdTauSlow ? 0.3f : 0.06f); cmdTauChanged = false; }
    rx.setAfc(cmdAfc);
    if (cmdScan) {
      cmdScan = false;
      const Satz& s = SAETZE[satz];
      scanner.start(s.fL, s.grob, s.fein, FS);
    }

    // Abtastwerte verarbeiten (24 bit linksbündig im 32-bit-Wort)
    float maxL = 0, maxR = 0;
    for (int i = 0; i < BLOCK; i++) {
      float l = (float)(buf[2 * i] >> 8) * (1.0f / 8388608.0f);
      float r = (float)(buf[2 * i + 1] >> 8) * (1.0f / 8388608.0f);
      rx.sample(l, r, T);
      scanner.sample(l, T);
      float al = fabsf(l), ar = fabsf(r);
      if (al > maxL) maxL = al;
      if (ar > maxR) maxR = ar;
    }
    rx.endBlock(BLOCK_SEC);
    scanner.endBlock();
    zeroBlocks = (maxL == 0 && maxR == 0) ? zeroBlocks + 1 : 0;

    bool scanFinished = false;
    double fNew = 0;
    float contrast = 0;
    if (scanner.done()) {
      scanFinished = true;
      fNew = scanner.result();
      contrast = scanner.contrast();
      rx.setLocate(fNew);
      scanner.reset();
    }

    // Ergebnis veröffentlichen
    portENTER_CRITICAL(&mux);
    shared.r = rx.result();
    shared.scanning = scanner.active();
    if (scanFinished) { shared.scanNew = true; shared.scanF = fNew; shared.scanContrast = contrast; }
    shared.clipL = maxL > 0.98f;
    shared.clipR = maxR > 0.98f;
    shared.noData = zeroBlocks > 50;
    portEXIT_CRITICAL(&mux);
  }
}

// ------------------------------------------------------------- Anzeige
#if USE_OLED
void drawOled(const lockin::Result& r, bool scanning, bool clip, int dir) {
  char s[40];
  float uvH = toUv(r.hAmp, GAIN_L);
  float uvN = toUv(r.noise, GAIN_L);
  float nT = toNT(uvH, r.fL);
  float snr = dBuV(uvH) - dBuV(uvN);

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  snprintf(s, sizeof(s), "%s %.1fHz", SAETZE[satz].kurz, r.fL);
  oled.drawStr(0, 9, s);
  if (refSet) oled.drawStr(110, 9, "REF");
  oled.drawHLine(0, 12, 128);

  if (scanning) {
    oled.setFont(u8g2_font_logisoso16_tr);
    oled.drawStr(0, 38, "Suche...");
    oled.setFont(u8g2_font_6x10_tf);
    oled.drawStr(0, 54, "nah an die Leitung");
    oled.sendBuffer();
    return;
  }

  // großer Wert: Feld in nT oder Strom in mA (Zangen-Modus)
  oled.setFont(u8g2_font_logisoso18_tr);
  if (zange) snprintf(s, sizeof(s), "%.2f", nTtoMA(nT, ZANGE_R_M));
  else if (nT >= 10) snprintf(s, sizeof(s), "%.0f", nT);
  else snprintf(s, sizeof(s), "%.2f", nT);
  oled.drawStr(0, 36, s);
  int wNum = oled.getStrWidth(s);                    // Breite in der großen Schrift
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(wNum + 4, 36, zange ? "mA" : "nT");

  // Richtung (Strom) rechts oben
  if (SAETZE[satz].cd) {
    if (!refSet) oled.drawStr(90, 24, "r=Ref");
    else if (dir > 0) oled.drawStr(98, 24, "HIN");
    else if (dir < 0) oled.drawUTF8(86, 24, "ZURÜCK");
    else oled.drawStr(110, 24, "?");
  }
  // Links/Rechts (V/H) rechts unten
  float lr = lrInvert ? -r.lr : r.lr;
  if (snr > 10) {
    if (lr > 0.1f) oled.drawTriangle(124, 31, 114, 26, 114, 36);
    else if (lr < -0.1f) oled.drawTriangle(92, 31, 102, 26, 102, 36);
    else oled.drawBox(106, 28, 4, 6);
  }

  // Balken: Signal über Rauschen, 0 ... 60 dB
  int w = (int)(snr / 60.0f * 126.0f);
  if (w < 0) w = 0;
  if (w > 126) w = 126;
  oled.drawFrame(0, 42, 128, 8);
  oled.drawBox(1, 43, w, 6);

  snprintf(s, sizeof(s), "%.0fdBuV SNR %.0fdB", dBuV(uvH), snr);
  oled.drawStr(0, 62, clip ? "UEBERSTEUERT!" : s);
  oled.sendBuffer();
}
#endif

// ------------------------------------------------------------- Ausgabe
void printHelp() {
  Serial.println();
  Serial.println("Kabelsuchgeraet Empfaenger-Test (Phase 3)");
  Serial.println(" s  Frequenzsuche (nah an die Leitung)   r  Referenz Stromrichtung");
  Serial.println(" f  Frequenzsatz wechseln                t  Glaettung schnell/ruhig");
  Serial.println(" z  Stromzangen-Modus (5 cm)              l  Links/Rechts umdrehen");
  Serial.println(" a  AFC an/aus   v  ausfuehrlich an/aus   ? Hilfe");
  Serial.println(" c<I_mA> <r_cm>  kalibrieren, z. B. c10 10");
  Serial.println(" k<Wert>         Kalibrierfaktor setzen (uV pro nT und kHz)");
  Serial.printf(" Satz: %s, fL = %.2f Hz, calK = %.2f\n\n", SAETZE[satz].name, SAETZE[satz].fL, calK);
}

void handleCommand(const String& line, const lockin::Result& r) {
  if (line.length() == 0) return;
  char c = line[0];
  switch (c) {
    case 's': cmdScan = true; Serial.println("# Frequenzsuche gestartet"); break;
    case 'r':
      refDelta = r.delta; refSet = true;
      Serial.printf("# Referenz gesetzt (Delta = %.0f Grad)\n", r.delta * 57.2958f);
      break;
    case 'f':
      satz = (satz + 1) % N_SAETZE; cmdSatz = satz; refSet = false;
      Serial.printf("# Satz %s, fL = %.2f Hz\n", SAETZE[satz].name, SAETZE[satz].fL);
      break;
    case 't':
      tauSlow = !tauSlow; cmdTauSlow = tauSlow; cmdTauChanged = true;
      Serial.printf("# Glaettung %s\n", tauSlow ? "ruhig (2 x 0,3 s)" : "schnell (2 x 0,06 s)");
      break;
    case 'z': zange = !zange; Serial.printf("# Zangen-Modus %s\n", zange ? "an" : "aus"); break;
    case 'l': lrInvert = !lrInvert; Serial.println("# Links/Rechts umgedreht"); break;
    case 'a': cmdAfc = !cmdAfc; Serial.printf("# AFC %s\n", cmdAfc ? "an" : "aus"); break;
    case 'v': verbose = !verbose; break;
    case 'k': {
      float k = line.substring(1).toFloat();
      if (k > 0) { calK = k; Serial.printf("# calK = %.3f\n", calK); }
      break;
    }
    case 'c': {
      // c<I_mA> <r_cm>: bekannter Strom in einem geraden Draht, Abstand zur Spulenmitte
      int sp = line.indexOf(' ');
      float iMA = line.substring(1, sp > 0 ? sp : line.length()).toFloat();
      float rCM = sp > 0 ? line.substring(sp + 1).toFloat() : 0;
      if (iMA > 0 && rCM > 0) {
        float bNT = 0.2f * iMA / (rCM / 100.0f);
        float uv = toUv(r.hAmp, GAIN_L);
        calK = uv / (bNT * (float)(r.fL / 1000.0));
        Serial.printf("# Kalibriert: B = %.3f nT, U = %.2f uV -> calK = %.3f uV/(nT*kHz)\n", bNT, uv, calK);
        Serial.println("#   Wert notieren und oben im Sketch bei calK eintragen.");
      } else {
        Serial.println("# Format: c<I_mA> <r_cm>, z. B. c10 10");
      }
      break;
    }
    case '?': printHelp(); break;
    default: break;
  }
}

// ------------------------------------------------------------- setup / loop
void setup() {
  Serial.begin(115200);
  pinMode(PIN_BTN, INPUT_PULLUP);
  delay(300);

#if USE_OLED
  oled.setBusClock(400000);
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(0, 12, "Empfaenger-Test");
  oled.drawStr(0, 28, "startet ...");
  oled.sendBuffer();
#endif

  // I²S: ESP32 ist Taktgeber (BCK, LRCK, MCLK = 256 x fs), PCM1808 im Slave-Modus
  i2s.setPins(PIN_BCK, PIN_WS, -1, PIN_DIN, PIN_MCLK);
  if (!i2s.begin(I2S_MODE_STD, FS, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("FEHLER: I2S konnte nicht gestartet werden");
    for (;;) delay(1000);
  }

  rx.begin(FS, SAETZE[satz].fL, SAETZE[satz].cd);
  rx.setTau(0.06f);
  xTaskCreatePinnedToCore(dspTask, "dsp", 8192, nullptr, 5, nullptr, 0);
  printHelp();
}

void loop() {
  static uint32_t tPlot = 0, tOled = 0, tVerb = 0, tBtn = 0;
  static bool btnDown = false;
  static String line;

  Shared s;
  portENTER_CRITICAL(&mux);
  s = shared;
  shared.scanNew = false;
  portEXIT_CRITICAL(&mux);
  const lockin::Result& r = s.r;

  // serielle Befehle
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') { handleCommand(line, r); line = ""; }
    else line += c;
  }

  // BOOT-Taste: kurz = Referenz, lang = Frequenzsuche
  bool down = digitalRead(PIN_BTN) == LOW;
  uint32_t now = millis();
  if (down && !btnDown) tBtn = now;
  if (!down && btnDown) {
    if (now - tBtn > 1000) handleCommand("s", r);
    else if (now - tBtn > 30) handleCommand("r", r);
  }
  btnDown = down;

  if (s.scanNew) {
    Serial.printf("# Frequenz gefunden: %.2f Hz (Kontrast %.0f)\n", s.scanF, s.scanContrast);
  }
  if (s.noData && now - tVerb > 2000) {
    Serial.println("# WARNUNG: nur Nullen vom PCM1808 - Verdrahtung OUT/DIN, SCK, Versorgung pruefen");
    tVerb = now;
  }

  int dir = lockin::direction(r.delta, refDelta, refSet && r.cdValid);
  float uvH = toUv(r.hAmp, GAIN_L), uvV = toUv(r.vAmp, GAIN_R), uvN = toUv(r.noise, GAIN_L);
  float uvC = toUv(r.cdAmp, GAIN_L);
  float lr = lrInvert ? -r.lr : r.lr;

  // serieller Plotter: dB bezogen auf 1 µV an der Spule
  if (now - tPlot >= 60) {
    tPlot = now;
    Serial.printf("H:%.1f V:%.1f CD:%.1f Rauschen:%.1f Richtung:%d LR:%.1f\n",
                  dBuV(uvH), dBuV(uvV), dBuV(uvC), dBuV(uvN), dir * 20, lr * 20.0f);
  }
  // ausführliche Zeile
  if (verbose && now - tVerb >= 1000) {
    tVerb = now;
    float nT = toNT(uvH, r.fL);
    Serial.printf("# %s fL=%.2fHz H=%.1fdBuV (%.3f nT) V=%.1fdBuV SNR=%.1fdB CD=%.1fdBuV "
                  "Delta=%.0fdeg Richtung=%s LR=%+.2f%s%s\n",
                  SAETZE[satz].name, r.fL, dBuV(uvH), nT,
                  dBuV(uvV), dBuV(uvH) - dBuV(uvN), dBuV(uvC),
                  r.delta * 57.2958f, dir > 0 ? "HIN" : dir < 0 ? "ZURUECK" : "?", lr,
                  s.clipL ? " UEBERSTEUERT-L" : "", s.clipR ? " UEBERSTEUERT-R" : "");
    if (zange) Serial.printf("#   Zange (%.0f cm): I = %.3f mA\n", ZANGE_R_M * 100, nTtoMA(nT, ZANGE_R_M));
  }

#if USE_OLED
  if (now - tOled >= 120) {
    tOled = now;
    drawOled(r, s.scanning, s.clipL || s.clipR, dir);
  }
#endif
  delay(5);
}
