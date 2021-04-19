/*
   LEDManager.cpp - A library to handle basic LED operations
   Tyler  W. Graham, April 11, 2020
*/

#include "LEDManager.h"

LEDManager::LEDManager(CRGB *leds, int maxLeds)
{
  _leds = leds;
  _maxLeds = maxLeds;
}

double LEDManager::brightness = 200;

void LEDManager::begin()
{
}

void LEDManager::clearAll()
{
  for (uint8_t i = 0; i < _maxLeds; i++)
  {
    _leds[i] = CHSV(0, 0, 0);
    delay(0);
  }
  FastLED.show();
}

void LEDManager::RGBAll(uint8_t r, uint8_t g, uint8_t b)
{
  for (uint8_t i = 0; i < _maxLeds; i++)
  {
    _leds[i] = CRGB(r, g, b);
    delay(0);
  }
  FastLED.show();
}

void LEDManager::HSVAll(double h, double s, double v)
{
  for (uint8_t i = 0; i < _maxLeds; i++)
  {
    _leds[i] = CHSV(h, s, v);
    delay(0);
  }
  FastLED.show();
}

int LEDManager::getBrightness()
{
  return brightness;
}

void LEDManager::setBrightness(int modifier)
{
  if ((brightness + modifier) >= 0 && (brightness + modifier) <= 200)
    brightness += modifier;
}

uint8_t LEDManager::getMaxLeds()
{
  return _maxLeds;
}
