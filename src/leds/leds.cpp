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

void Leds::drawAngle(float angle, float strength, CRGB color) {
  if (strength < 0) {
    strength = 0;
  } else if (strength > 1) {
    strength = 1;
  }

  // Each ring has its own mounting offset; normalize per ring into [0,360).
  float aSmall = fmodf(angle + LED_SMALL_BASE_ANGLE, 360.0f);
  while (aSmall < 0) {
    aSmall += 360.0f;
  }
  float aBig = fmodf(angle + LED_BIG_BASE_ANGLE, 360.0f);
  while (aBig < 0) {
    aBig += 360.0f;
  }

  // Nearest LED on each ring for this direction.
  uint16_t big = (uint16_t)lroundf(aBig / 360.0f * BIG_RING_LEDS) % BIG_RING_LEDS;
  uint16_t small = (uint16_t)lroundf(aSmall / 360.0f * SMALL_RING_LEDS) % SMALL_RING_LEDS;

  // Two-phase intensity: the upper half of strength controls the inner ring
  // (full=on, fading to off at the midpoint); the lower half then dims the
  // outer ring from full down to a faint floor.
  uint8_t outerBrightness;
  uint8_t innerBrightness;
  if (strength >= 0.5f) {
    outerBrightness = 255;
    innerBrightness = (uint8_t)lroundf((strength - 0.5f) / 0.5f * 255.0f);
  } else {
    innerBrightness = 0;
    outerBrightness = (uint8_t)lroundf(LED_OUTER_MIN_BRIGHTNESS +
                                       strength / 0.5f * (255.0f - LED_OUTER_MIN_BRIGHTNESS));
  }

  CRGB outerColor = color;
  outerColor.nscale8_video(outerBrightness);
  Leds::setBig(big, outerColor);

  if (innerBrightness > 0) {
    CRGB innerColor = color;
    innerColor.nscale8_video(innerBrightness);
    Leds::setSmall(small, innerColor);
  }
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
