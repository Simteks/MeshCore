#include "BootSequence.h"
#include <SPI.h>
#include <Wire.h>
#ifdef EPD_BOOTSCREEN
#include <display/EPDBootChecklist.h>
#endif
#ifdef ESP32
#include <esp_log.h>
#endif

namespace boot {

static bool s_started = false;
static int s_step_no = 0;
static int s_ok = 0;
static int s_fail = 0;
static const int BOX_WIDTH = 64; // terminal characters

static const char* ok_cross(bool ok) { return ok ? "[\xE2\x9C\x94]" : "[\xE2\x9C\x96]"; } // ✔ / ✖
static const char* emoji_ok(bool ok) { return ok ? "✅" : "❌"; }

// Mirror output to both USB CDC and SerialJTAG (ESP32-S3) so whichever COM is open gets logs
static void tprint(const char* s) {
  Serial.print(s);
#if defined(ESP32) && defined(ARDUINO_USB_CDC_ON_BOOT)
  #ifdef SerialJTAG
    SerialJTAG.print(s);
  #endif
#endif
}
static void tprintln(const char* s) {
  Serial.println(s);
#if defined(ESP32) && defined(ARDUINO_USB_CDC_ON_BOOT)
  #ifdef SerialJTAG
    SerialJTAG.println(s);
  #endif
#endif
}
static void tprintf(const char* fmt, ...) {
  char buf[256];
  va_list ap; va_start(ap, fmt); vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
  Serial.print(buf);
#if defined(ESP32) && defined(ARDUINO_USB_CDC_ON_BOOT)
  #ifdef SerialJTAG
    SerialJTAG.print(buf);
  #endif
#endif
}
static void tflush() {
  Serial.flush();
#if defined(ESP32) && defined(ARDUINO_USB_CDC_ON_BOOT)
  #ifdef SerialJTAG
    SerialJTAG.flush();
  #endif
#endif
}

static void box_top(const char* title) {
  tprint("\r\n┌"); for (int i=0;i<BOX_WIDTH-2;i++) tprint("─"); tprintln("┐");
  int len = title ? strlen(title) : 0; int pad = BOX_WIDTH-2-len; int l= pad>0? pad/2:0; int r= pad>0? pad-l:0;
  tprint("│"); for (int i=0;i<l;i++) tprint(" "); tprint(title?title:""); for (int i=0;i<r;i++) tprint(" "); tprintln("│");
  tprint("├"); for (int i=0;i<BOX_WIDTH-2;i++) tprint("─"); tprintln("┤");
  tflush();
}

static void box_step(int idx, const char* name, bool ok) {
  char line[160]; snprintf(line, sizeof(line), "%2d. %s %s", idx, emoji_ok(ok), name?name:"");
  int l = strlen(line); tprint("│ "); tprint(line); int pad = BOX_WIDTH-3-l; if (pad < 0) pad = 0; for (int i=0;i<pad;i++) tprint(" "); tprintln("│");
  tflush();
}

static void box_summary() {
  tprint("├"); for (int i=0;i<BOX_WIDTH-2;i++) tprint("─"); tprintln("┤");
  char sum[80]; snprintf(sum, sizeof(sum), "Summary: %d ok, %d failed", s_ok, s_fail);
  int l = strlen(sum); tprint("│ "); tprint(sum); int pad = BOX_WIDTH-3-l; if (pad < 0) pad = 0; for (int i=0;i<pad;i++) tprint(" "); tprintln("│");
  tprint("└"); for (int i=0;i<BOX_WIDTH-2;i++) tprint("─"); tprintln("┘\r\n");
  tflush();
}

void begin(const char* role, const char* fw_ver, const char* build_date) {
  if (s_started) return;
  s_started = true;
  s_step_no = 0; s_ok = 0; s_fail = 0;
  tprintln("");
  tprintln("==== Boot ====");
  tprintf("Role: %s\r\n", role ? role : "(unknown)");
  tprintf("FW: %s (%s)\r\n", fw_ver ? fw_ver : "(n/a)", build_date ? build_date : "(n/a)");
#ifdef ESP32
  // Reduce noisy ROM/driver errors that are benign for our boot flow.
  esp_log_level_set("gpio", ESP_LOG_NONE);   // hide "isr service already installed"
  esp_log_level_set("epdiy", ESP_LOG_WARN);  // demote cache line size warning
#endif
  box_top("Initialize    System");
}

bool init_pins() {
#ifdef ESP32
  // If these are defined at build time we set safe defaults similar to vendor idf_setup().
  #ifdef BOARD_LORA_CS
    pinMode(BOARD_LORA_CS, OUTPUT);
    digitalWrite(BOARD_LORA_CS, HIGH);
  #endif
  #ifdef BOARD_SD_CS
    pinMode(BOARD_SD_CS, OUTPUT);
    digitalWrite(BOARD_SD_CS, HIGH);
  #endif
  #ifdef BOARD_PCA9535_INT
    if (BOARD_PCA9535_INT >= 0) pinMode(BOARD_PCA9535_INT, INPUT_PULLUP);
  #endif
#endif
  tprintf("%s pin defaults\r\n", ok_cross(true));
  // mirrored terminal checklist
  s_step_no++; s_ok++; box_step(s_step_no, "Pin defaults", true);
  return true;
}

bool init_i2c(int sda, int scl) {
  bool ok = false;
  if (sda >= 0 && scl >= 0) {
    ok = Wire.begin(sda, scl);
  } else {
    ok = Wire.begin();
  }
  tprintf("%s I2C on SDA=%d SCL=%d\r\n", ok_cross(ok), sda, scl);
#ifdef EPD_BOOTSCREEN
  // Start EPD boot UI after I2C is initialized to avoid duplicate driver install noise.
  epdboot::begin();
  epdboot::step("Pin defaults", true);
  epdboot::step("I2C init", ok);
#endif
  // mirrored terminal checklist
  s_step_no++; if (ok) s_ok++; else s_fail++; box_step(s_step_no, "I2C init", ok);
  return ok;
}

bool init_spi(int sclk, int miso, int mosi) {
#if defined(ESP32)
  SPI.end();
  SPI.begin(sclk, miso, mosi);
  bool ok = true;
#else
  bool ok = true;
#endif
  tprintf("%s SPI on SCLK=%d MISO=%d MOSI=%d\r\n", ok_cross(ok), sclk, miso, mosi);
#ifdef EPD_BOOTSCREEN
  epdboot::step("SPI init", ok);
#endif
  // mirrored terminal checklist
  s_step_no++; if (ok) s_ok++; else s_fail++; box_step(s_step_no, "SPI init", ok);
  return ok;
}

void step(const char* name, bool ok) {
  tprintf("%s %s\r\n", ok_cross(ok), name);
  s_step_no++; if (ok) s_ok++; else s_fail++; box_step(s_step_no, name, ok);
#ifdef EPD_BOOTSCREEN
  epdboot::step(name, ok);
#endif
}

void print_hw_summary() {
#ifdef ESP32
  tprintf("Chip: %s\r\n", ESP.getChipModel());
  tprintf("Flash: %u bytes\r\n", (unsigned)ESP.getFlashChipSize());
  tprintf("PSRAM: %u bytes\r\n", (unsigned)ESP.getPsramSize());
  tprintf("Heap: free=%u, min_free=%u\r\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMinFreeHeap());
#endif
}

void log_radio_params(float freq, float bw, uint8_t sf, uint8_t cr, int8_t tx_dbm) {
  tprintf("LoRa: f=%.3f MHz, bw=%.0f kHz, sf=%u, cr=4/%u, tx=%d dBm\r\n",
                freq, bw, (unsigned)sf, (unsigned)cr, (int)tx_dbm);
}

void end() {
  tprintln("==== Boot Done ====");
  box_summary();
#ifdef EPD_BOOTSCREEN
  epdboot::end();
#endif
}

} // namespace boot
