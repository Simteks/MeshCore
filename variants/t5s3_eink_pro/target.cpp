#include "target.h"
#include <CayenneLPP.h>
#include <Wire.h>

ESP32Board board;
ESP32RTCClock rtc_clock;

// RadioLib Module wiring: NSS, DIO1 (IRQ), RESET, BUSY
static Module module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY);
CustomSX1262 radio(&module);
CustomSX1262Wrapper radio_driver(radio, board);

// Provide a default no-op SensorManager so examples can link.
class NoopSensors : public SensorManager {
public:
  bool begin() override {
  // I2C is initialized during boot sequence.
    return true;
  }
  bool querySensors(uint8_t requester_permissions, CayenneLPP& telemetry) override { return false; }
  void loop() override {}
};
static NoopSensors _noopSensors;
SensorManager& sensors = _noopSensors;

bool radio_init() {
  board.begin();
  rtc_clock.begin();

  // Configure SPI pins if provided
#if defined(P_LORA_SCLK)
  SPI.begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI);
#endif
  // Init radio with defaults from macros (LORA_* from root platformio.ini)
  return radio.std_init(&SPI);
}

void radio_set_params(float freq, float bw, uint8_t sf, uint8_t cr) {
  radio.setFrequency(freq);
  radio.setBandwidth(bw);
  radio.setSpreadingFactor(sf);
  radio.setCodingRate(cr);
}

void radio_set_tx_power(int8_t dbm) {
  radio.setOutputPower(dbm);
}

uint32_t radio_get_rng_seed() {
  // Use a mix of analogRead noise and radio random byte
  uint32_t seed = ((uint32_t)esp_random());
  seed ^= ((uint32_t)radio.randomByte()) << 24;
  seed ^= ((uint32_t)radio.randomByte()) << 16;
  seed ^= ((uint32_t)radio.randomByte()) << 8;
  seed ^= ((uint32_t)radio.randomByte());
  return seed;
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);
}
