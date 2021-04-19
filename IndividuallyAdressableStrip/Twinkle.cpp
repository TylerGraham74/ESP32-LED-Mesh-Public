/*
   Twinkle.cpp - Twinkle pattern
   Adapted by Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"

#define NUM_LEDS 100

static bool initialized = false;
static byte twinkleBrightness[NUM_LEDS];      // The brightness of each LED is controlled individually through this array
static byte twinkleHue[NUM_LEDS];             // The hue of each LED is controlled individually through this array
static byte startVal[NUM_LEDS];    // The startVal array helps to randomise the LED pattern

class Twinkle : public LEDManager
{
  private:
    uint16_t _minHue;
    uint16_t _maxHue;
    uint8_t _saturation;
    uint8_t maxBrightness;


  public:
    Twinkle(CRGB* leds, uint8_t maxLeds, uint16_t minHue, uint16_t maxHue, uint8_t saturation) : LEDManager(leds, maxLeds) {
      _minHue = minHue;
      _maxHue = maxHue;
      _saturation = saturation;
      maxBrightness = getBrightness();

      if (!initialized)
      {
        for (int i = 0; i < NUM_LEDS; i++) {
          startVal[i] = random8();
          twinkleHue[i] = random(minHue, maxHue);
        }
        initialized = true;
      }
    }

    void play()
    {
      for (int i = 0; i < NUM_LEDS; i++) {

        //The brightness of each LED is based on the startValue,
        //and has a sinusoidal pattern
        startVal[i]++;
        twinkleBrightness[i] = sin8(startVal[i]);

        //Set the hue and brightness of each LED
        _leds[i] = CHSV(twinkleHue[i], _saturation, map(twinkleBrightness[i], 0, 255, 0, maxBrightness));

        //This helps to further randomise the pattern
        if (random(1000) < 100) {
          startVal[i] = startVal[i] + 2;
        }

        //The hue will only change when the LED is "off"
        if (twinkleBrightness[i] < 3) {
          //Randomise the hue for the next on off cycle
          twinkleHue[i] = random(_minHue, _maxHue);
        }
      }

      FastLED.show();   // Show the next frame of the LED pattern
      delay(10);        // Slow down the animation slightly
    }


};
