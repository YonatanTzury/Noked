#include <Arduino.h>
#include <FastLED.h>

#include "../pins.h"

// Physical mounting offset of LED index 0 on each ring, in degrees clockwise
// from the unit's forward direction. The two rings can be mounted at different
// rotations, so each has its own offset. Override per-unit at build time, e.g.
// -DLED_SMALL_BASE_ANGLE=90 -DLED_BIG_BASE_ANGLE=10.
//
// LED_BASE_ANGLE remains as a shared fallback for both rings.
#ifndef LED_BASE_ANGLE
#define LED_BASE_ANGLE 0
#endif
#ifndef LED_SMALL_BASE_ANGLE
#define LED_SMALL_BASE_ANGLE LED_BASE_ANGLE
#endif
#ifndef LED_BIG_BASE_ANGLE
#define LED_BIG_BASE_ANGLE LED_BASE_ANGLE
#endif

// Brightness floor (0-255) for the outer ring at minimum strength, so the
// nearest-direction pixel stays faintly visible instead of going dark.
#define LED_OUTER_MIN_BRIGHTNESS 30

// Wraps the two WS2812 LED rings on a single FastLED data line.
// Exposes a generic pixel/ring API; higher-level status logic is layered
// on top of this elsewhere.
class Leds {
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

  // Light the nearest LED on each ring toward `angle` (degrees, clockwise from
  // the unit's forward direction; each ring's own base angle is added on top).
  // `strength`
  // in [0,1] controls intensity: at full strength both rings are fully lit; as
  // it drops the inner ring fades out first, then the outer ring dims toward a
  // faint floor. Writes into the framebuffer; caller owns clear()/show().
  void drawAngle(float angle, float strength, CRGB color);

  void clear();
  void setBrightness(uint8_t brightness);
  void show();

private:
  CRGB leds[TOTAL_LEDS];
};
