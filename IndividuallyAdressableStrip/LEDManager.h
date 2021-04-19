/*
   LEDManager.h - A library to handle basic LED operations
   Tyler  W. Graham, April 11, 2020
*/

#ifndef ledman
#define ledman

#include "Arduino.h"
#include <FastLED.h>

#define FULL_SATURATION 255
#define MAX_BRIGHTNESS 200

class LEDManager
{
  public:
    //Constructor
    LEDManager(CRGB *leds, int maxLeds);
    
    //Variables
    static double brightness;
    
    //Methods
    void begin();
    void RGBAll(uint8_t r, uint8_t g, uint8_t b);
    void HSVAll(double h, double s, double v);
    void clearAll();
    int getBrightness();
    void setBrightness(int modifier);
    uint8_t getMaxLeds();

  private:
    //Variables

  protected:
    uint8_t _maxLeds;
    CRGB *_leds;
    
    
    const uint8_t hueList[6] = {0, 64, 96, 128, 160, 192};
    
};

#endif
