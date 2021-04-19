/*
   Noise.cpp - Noise pattern
   Adapted from "Noise" FastLED example
   Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"
#include <FastLED.h>

static uint16_t z = 120;
static uint16_t x = 78;
static uint16_t y = 134;

//Color controlling variables
static double noiseHue;
static uint8_t noiseSaturation;
static uint8_t noiseMode = 0; //0 for static hue, 1 for rotating hue, 2 for all colors

class Noise : public LEDManager
{
  private:

    //Noise controlling variables
    uint16_t speed = 1;
    uint16_t scale = 10;
    uint8_t noise[100];

  public:
    Noise(CRGB* leds, uint8_t maxLeds, uint8_t hue, bool white, bool cycle, bool all) : LEDManager(leds, maxLeds)
    {
      if(!cycle)
        noiseHue = hue;
      noiseSaturation = white ? 0 : FULL_SATURATION;
      if (cycle)
        noiseMode = 1;
      else if (all)
        noiseMode = 2;
      else
        noiseMode = 0;
      if (noiseMode == 1)
      {
        noiseHue += .05;
        if (noiseHue >= 255)
          noiseHue = 0;
      }
    }

    //    PanelNoise(uint8_t rows, uint8_t columns, CRGB* leds, bool cycle) : LEDManager(rows, columns, leds)
    //    {
    //      matrixWidth = columns;
    //      matrixHeight = rows;
    //      noiseSaturation = FULL_SATURATION;
    //      noiseMode = (cycle) ? 1 : 2;
    //      if (cycle)
    //      {
    //        noiseHue += .05;
    //        if (noiseHue >= 255)
    //          noiseHue = 0;
    //      }
    //    }

    // Fill the x/y array of 8-bit noise values using the inoise8 function.
    void fillnoise8() {
      for (int i = 0; i < 1; i++) {
        int ioffset = scale * i;
        for (int j = 0; j < _maxLeds; j++) {
          int joffset = scale * j;
          noise[j] = inoise8(x + ioffset, y + joffset, z);
          //Serial.println(noise[i][j]);
        }
      }
      z += speed;
    }

    void play()
    {
      fillnoise8();
      for (int j = 0; j < _maxLeds; j++)
      {
        switch (noiseMode)
        {
          case 0: //static hue
            {
              _leds[j] = CHSV(noiseHue - 12 + (noise[j] / 3.25), noiseSaturation, (noise[j] <= brightness) ? noise[j] : brightness);
              break;
            }
          case 1: //rotating hues
            {
              _leds[j] = CHSV(noiseHue - 12 + (noise[j] / 3.25), noiseSaturation, (noise[j] <= brightness) ? noise[j] : brightness);
              break;
            }
          case 2: //all colors
            {
              //_leds[0] = CHSV(0, 0, 255);
              _leds[j] = CHSV(noise[j] * 1.7, noiseSaturation, (noise[j] <= brightness) ? noise[j] : brightness);
              break;
            }
        }
      }
      FastLED.show();
    }
};
