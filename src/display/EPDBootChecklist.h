#pragma once

// Minimal EPDiy-backed boot checklist that mirrors vendor factory boot screen.
// Guarded by EPD_BOOTSCREEN build flag.

#ifdef EPD_BOOTSCREEN
#include <Arduino.h>
extern "C" {
  #include <epdiy.h>
}

namespace epdboot {

void begin();
void step(const char* name, bool ok);
void end();

} // namespace epdboot
#endif
