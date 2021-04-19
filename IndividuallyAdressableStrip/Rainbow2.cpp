/*
   Rainbow.cpp - Rainbow pattern
   Tyler W. Graham, April 18, 2021
*/

#include "LEDManager.h"

static uint8_t rainbow2Hue = 0;

class Rainbow2 : public LEDManager
{
  private:
  
  public:
    Rainbow2(CRGB* leds, uint8_t maxLeds) : LEDManager(leds, maxLeds) {
    }

    void play()
    {
      for(int i = 0; i < _maxLeds; i++)
      {
        _leds[i] = CHSV(rainbow2Hue + i, FULL_SATURATION, brightness);
      }
      FastLED.show();
      rainbow2Hue++;
    }
};
