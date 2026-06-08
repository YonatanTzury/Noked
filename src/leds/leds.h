#include <Arduino.h>
#include <FastLED.h>

#include "../pins.h"

// Wraps the two WS2812 LED rings on a single FastLED data line.
// Exposes a generic pixel/ring API; higher-level status logic is layered
// on top of this elsewhere.
class Leds
{
public:
  static const uint16_t SMALL_RING_LEDS = 18;
  static const uint16_t BIG_RING_LEDS = 36;
  static const uint16_t TOTAL_LEDS = SMALL_RING_LEDS + BIG_RING_LEDS;

  void init();

  // Absolute index across both rings [0, TOTAL_LEDS).
  void set(uint16_t index, CRGB color);
  // Ring-relative helpers.
  void setSmall(uint16_t index, CRGB color);
  void setBig(uint16_t index, CRGB color);

  void clear();
  void setBrightness(uint8_t brightness);
  void show();

private:
  CRGB leds[TOTAL_LEDS];
};
