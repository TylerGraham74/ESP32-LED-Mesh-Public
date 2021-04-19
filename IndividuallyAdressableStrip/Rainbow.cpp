/*
   Rainbow.cpp - Rainbow pattern
   Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"

#define DELAY 58 //milliseconds



static uint8_t rainbowHue = 0;

class Rainbow : public LEDManager
{
  private:
  
  public:
    Rainbow(CRGB* leds, uint8_t maxLeds) : LEDManager(leds, maxLeds) {
    }

    void play()
    {
      changeHue();
      delay(DELAY);
      rainbowHue++;
      if(rainbowHue > 255)
        rainbowHue = 0;
    }

    void changeHue()
    {
      HSVAll(rainbowHue, FULL_SATURATION, brightness);
    }
};
