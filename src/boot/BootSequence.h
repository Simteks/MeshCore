#pragma once

#include <Arduino.h>

namespace boot {

// Start of boot sequence. Prints header with role/firmware.
void begin(const char* role, const char* fw_ver, const char* build_date);

// Set default pin states (CS lines high, inputs pullups) if pins are defined.
// Returns true if executed (always true even if some pins are undefined).
bool init_pins();

// Initialize I2C bus on given pins and report status.
bool init_i2c(int sda, int scl);

// Initialize SPI bus on given pins and report status.
bool init_spi(int sclk, int miso, int mosi);

// Log a boot step with a checkmark or cross.
void step(const char* name, bool ok);

// Print quick HW summary (chip/flash/psram/heaps).
void print_hw_summary();

// Log LoRa params being used.
void log_radio_params(float freq, float bw, uint8_t sf, uint8_t cr, int8_t tx_dbm);

// End of boot sequence.
void end();

}  // namespace boot
