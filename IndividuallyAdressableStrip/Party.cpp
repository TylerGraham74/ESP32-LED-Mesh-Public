/*
   Party.cpp - Party pattern
   Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"

#define DELAY 500 //milliseconds


static uint8_t partyColor = 0;

class Party : public LEDManager
{
  private:
  
  public:
    Party(CRGB* leds, uint8_t maxLeds) : LEDManager(leds, maxLeds) {
    }

    void play()
    {
      changeColor(partyColor);
      delay(DELAY);
      partyColor++;
      if(partyColor > 5)
        partyColor = 0;
    }

    void changeColor(uint8_t i)
    {
      uint8_t hue = hueList[i];
      HSVAll(hue, FULL_SATURATION, brightness);
      /*switch (i)
      {
        case 0:
          //RGBAll(200, 0, 0); //red
          HSVAll(0, FULL_SATURATION, brightness);
          break;
        case 1:
          //RGBAll(200, 200, 0); //lime
          HSVAll(64, FULL_SATURATION, brightness);
          break;
        case 2:
          //RGBAll(0, 200, 0); //green
          HSVAll(96, FULL_SATURATION, brightness);
          break;
        case 3:
          //RGBAll(0, 200, 200);//cyan
          HSVAll(128, FULL_SATURATION, brightness);
          break;
        case 4:
          //RGBAll(0, 0, 200); //blue
          HSVAll(160, FULL_SATURATION, brightness);
          break;
        case 5:
          //RGBAll(230, 0, 195); //purple
          HSVAll(192, FULL_SATURATION, brightness);
          break;
      }*/
    }
};
