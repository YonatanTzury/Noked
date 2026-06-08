#include "leds.h"

void Leds::init() {
  FastLED.addLeds<WS2812, LED_DATA, GRB>(Leds::leds, TOTAL_LEDS);
  Leds::clear();
  Leds::show();
}

void Leds::set(uint16_t index, CRGB color) {
  if (index >= TOTAL_LEDS) {
    return;
  }
  Leds::leds[index] = color;
}

void Leds::setSmall(uint16_t index, CRGB color) {
  if (index >= SMALL_RING_LEDS) {
    return;
  }
  Leds::leds[index] = color;
}

void Leds::setBig(uint16_t index, CRGB color) {
  if (index >= BIG_RING_LEDS) {
    return;
  }
  Leds::leds[SMALL_RING_LEDS + index] = color;
}

void Leds::clear() {
  for (uint16_t i = 0; i < TOTAL_LEDS; i++) {
    Leds::leds[i] = CRGB::Black;
  }
}

void Leds::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
}

void Leds::show() {
  FastLED.show();
}
