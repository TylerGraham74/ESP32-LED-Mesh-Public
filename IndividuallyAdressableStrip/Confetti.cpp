/*
   Confetti.cpp - Confetti pattern
   Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"

#define DELAY  17 //approx 60 fps
#define MINIMUM_BRIGHTNESS 100

static uint8_t confettiHue = 0;
static uint8_t confettiSat = FULL_SATURATION;

class Confetti: public LEDManager {

  private:

  public:
    enum confettiType
    {
      Solid,
      Ranged,
      Rainbow
    };

    confettiType _type;

    Confetti(CRGB* leds, uint8_t maxLeds) : LEDManager(leds, maxLeds) {
      _type = Rainbow;
      confettiSat = FULL_SATURATION;
    }

    Confetti(CRGB* leds, uint8_t maxLeds, confettiType type, int hue, bool white) : LEDManager(leds, maxLeds)
    {
      _type = type;
      confettiHue = hue;
      confettiSat = (white) ? 0 : FULL_SATURATION;
    }

    void play()
    {
      delay(DELAY);
      if (brightness < MINIMUM_BRIGHTNESS)
        setBrightness(MINIMUM_BRIGHTNESS);
      fadeToBlackBy( _leds, _maxLeds, ((brightness * 1.0) / 20));
      uint8_t pos = random8(_maxLeds);
      switch (_type)
      {
        case 0:
          _leds[pos] = CHSV(confettiHue, confettiSat, brightness);
          break;
        case 1:
          _leds[pos] += CHSV(confettiHue + random8(64), confettiSat, brightness);
          break;
        case 2:
          _leds[pos] += CHSV(confettiHue + random8(64), confettiSat, brightness);
          confettiHue++;
          break;
      }
      confettiHue = (confettiHue > 255) ? 0 : confettiHue;
      FastLED.show();
    }

};
