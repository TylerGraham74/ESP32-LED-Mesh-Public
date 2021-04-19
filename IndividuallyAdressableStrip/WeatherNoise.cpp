/*
   WeatherNoise.cpp - Noise pattern for use with Atmosphere pattern
   Adapted from "Noise" FastLED example
   Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"
#include <FastLED.h>

#define CLOUD_SATURATION  110

static uint16_t zOff = 120;
static uint16_t xOff = 78;
static uint16_t yOff = 134;

//Color controlling variables
static double weatherNoiseHue;
static uint8_t weatherNoiseSaturation;
static uint8_t weatherNoiseMode = 0; //0 for static hue, 1 for rotating hue, 2 for all colors

class WeatherNoise : public LEDManager
{
  private:

    //Noise controlling variables
    uint16_t speed = 1;
    uint16_t scale = 10;
    uint8_t noise[100];

  public:
    WeatherNoise(CRGB* leds, uint8_t maxLeds, uint8_t hue, bool white, bool cycle, bool all, bool clouds) : LEDManager(leds, maxLeds)
    {
      if(!cycle)
        weatherNoiseHue = hue;
      weatherNoiseSaturation = white ? 0 : FULL_SATURATION;
      if (cycle)
        weatherNoiseMode = 1;
      else if (all)
        weatherNoiseMode = 2;
      else
        weatherNoiseMode = 0;
      if (weatherNoiseMode == 1)
      {
        weatherNoiseHue += .05;
        if (weatherNoiseHue >= 255)
          weatherNoiseHue = 0;
      }
     weatherNoiseSaturation = clouds ? CLOUD_SATURATION : FULL_SATURATION;
    }

    // Fill the x/y array of 8-bit noise values using the inoise8 function.
    void fillnoise8() {
      for (int i = 0; i < 1; i++) {
        int ioffset = scale * i;
        for (int j = 0; j < _maxLeds; j++) {
          int joffset = scale * j;
          noise[j] = inoise8(xOff+ ioffset, yOff + joffset, zOff);
          //Serial.println(noise[i][j]);
        }
      }
      zOff += speed;
    }

    void play()
    {
      fillnoise8();
      for (int j = 0; j < _maxLeds; j++)
      {
        switch (weatherNoiseMode)
        {
          case 0: //static hue
            {
              _leds[j] = CHSV(weatherNoiseHue - 12 + (noise[j] / 3.25), weatherNoiseSaturation, (noise[j] <= brightness) ? noise[j] : brightness);
              break;
            }
          case 1: //rotating hues
            {
              _leds[j] = CHSV(weatherNoiseHue - 12 + (noise[j] / 3.25), weatherNoiseSaturation, (noise[j] <= brightness) ? noise[j] : brightness);
              break;
            }
          case 2: //all colors
            {
              //_leds[0] = CHSV(0, 0, 255);
              _leds[j] = CHSV(noise[j] * 1.7, weatherNoiseSaturation, (noise[j] <= brightness) ? noise[j] : brightness);
              break;
            }
        }
      }
      FastLED.show();
    }
};
