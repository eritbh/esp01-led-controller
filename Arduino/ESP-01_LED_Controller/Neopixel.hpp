// Stuff for controlling the LEDs.

#ifndef NEOPIXEL_HPP
#define NEOPIXEL_HPP

#include "Colors.hpp"

#define PIN 0
#define PIXELS 180

// The amount of time between LED updates
#define DELAY 100

#define DEFAULT_PATTERN_LENGTH 24
#define DEFAULT_PATTERN {       \
  C_BLUE,   C_BLUE,   C_BLUE,   \
  C_BLUE,   C_BLUE,   C_BLUE,   \
  C_PINK,   C_PINK,   C_PINK,   \
  C_PINK,   C_PINK,   C_PINK,   \
  C_WHITE,  C_WHITE,  C_WHITE,  \
  C_WHITE,  C_WHITE,  C_WHITE,  \
  C_PINK,   C_PINK,   C_PINK,   \
  C_PINK,   C_PINK,   C_PINK,   \
}

Adafruit_NeoPixel pixels(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

// An array of colors that make up the pattern. Each color
// is a 32-bit unsigned integer where the 8 most significant
// bits are ignored, the next 8 bits are red, the next 8
// blue, and the 8 least significant bits are blue. Think of
// hex integer literals like `0x0094FF`. Up to 256 colors
// can be in the pattern because that's a nice round number
// that we can keep track of with an 8-bit integer.
uint8_t patternMaxIndex = DEFAULT_PATTERN_LENGTH - 1;
uint32_t pattern[256] = DEFAULT_PATTERN;

// The current offset of the pattern - to scroll the pattern
// across the strip, this is incremented up to the last index
// of the pattern, then reset to 0, repeating forever. Since
// the offset is subtracted below, the whole pattern is
// shifted right each frame.
int offset = 0;

void updatePattern(uint8_t maxIndex, uint32_t newPattern[256])
{
  patternMaxIndex = maxIndex;
  for (int i = 0; i < 256; i += 1) {
    pattern[i] = newPattern[i];
  }
  offset = 0;
}

// Displays the next frame of the LED animation
void updateLEDs() {
  // Update each pixel
  for (int px = 0; px < PIXELS; px += 1) {
    int i = (px - offset) % (patternMaxIndex + 1);
    while (i < 0) {
      i += patternMaxIndex + 1;
    }
    pixels.setPixelColor(px, pattern[i]);
  }

  // Display the new values
  pixels.show();

  // Increment/loop offset for next update
  offset += 1;
  if (offset > patternMaxIndex) {
    offset = 0;
  }
}

#endif
