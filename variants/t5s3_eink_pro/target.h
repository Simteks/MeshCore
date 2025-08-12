#pragma once

#include <Mesh.h>
#include <helpers/ESP32Board.h>
#include <helpers/radiolib/CustomSX1262.h>
#include <helpers/radiolib/CustomSX1262Wrapper.h>
#include <helpers/SensorManager.h>

// Pin map (from vendor factory demo utilities.h)
#ifndef P_LORA_NSS
  #define P_LORA_NSS   46
  #define P_LORA_DIO_1 10
  #define P_LORA_RESET 1
  #define P_LORA_BUSY  47
  #define P_LORA_SCLK  14
  #define P_LORA_MISO  21
  #define P_LORA_MOSI  13
#endif

#ifndef PIN_BOARD_SDA
  #define PIN_BOARD_SDA 39
  #define PIN_BOARD_SCL 40
#endif

// Optional BOARD_* aliases used by BootSequence for clearer logs
#ifndef BOARD_LORA_CS
  #define BOARD_LORA_CS P_LORA_NSS
#endif
#ifndef BOARD_SD_CS
  #define BOARD_SD_CS 12
#endif
#ifndef BOARD_SPI_SCLK
  #define BOARD_SPI_SCLK P_LORA_SCLK
#endif
#ifndef BOARD_SPI_MISO
  #define BOARD_SPI_MISO P_LORA_MISO
#endif
#ifndef BOARD_SPI_MOSI
  #define BOARD_SPI_MOSI P_LORA_MOSI
#endif

// Optional: define PIN_VBAT_READ if available in future (using ADC divider)

extern ESP32Board board;
extern ESP32RTCClock rtc_clock;
extern CustomSX1262 radio;
extern CustomSX1262Wrapper radio_driver;
extern SensorManager& sensors;

bool radio_init();
void radio_set_params(float freq, float bw, uint8_t sf, uint8_t cr);
void radio_set_tx_power(int8_t dbm);
uint32_t radio_get_rng_seed();
mesh::LocalIdentity radio_new_identity();
