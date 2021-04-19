/*
   Pulse pattern for LEDs
*/

#include "LEDManager.h"
#include <cmath>

#define TRANSITION_DELAY  2
#define FADE_DELAY 1000
#define FADE_TIME  2000
#define STABILIZATION_THRESHOLD 20
#define MINIMUM_BRIGHTNESS 100

static uint8_t pulseHue = 0;
static uint8_t pulseSat = FULL_SATURATION;
static uint8_t pulseRainbowColor = 0;
static bool atColor = false;
static uint8_t currBrightness = 0;

class Pulse : public LEDManager
{
  private:

    void fadeIn()
    {
      currBrightness += ceil(brightness / FADE_TIME);
      HSVAll(pulseHue, pulseSat, currBrightness);
      if (currBrightness >= brightness - STABILIZATION_THRESHOLD)
      {
        atColor = true;
        //HSVAll(pulseHue, FULL_SATURATION, brightness);
        delay(FADE_DELAY);
      }
      delay(TRANSITION_DELAY);
      FastLED.show();
    }

    void fadeOut()
    {
      currBrightness -= ceil(brightness / FADE_TIME);
      HSVAll(pulseHue, pulseSat, currBrightness);
      if (currBrightness <= STABILIZATION_THRESHOLD)
      {
        atColor = false;
        currBrightness = 0;
        clearAll();
        delay(FADE_DELAY);
      }
      delay(TRANSITION_DELAY);
      FastLED.show();
    }

  public:

    enum pulseType
    {
      Given = 0,
      Random,
      Rainbow
    };

    pulseType _type;

    Pulse(CRGB* leds, uint8_t maxLeds) : LEDManager(leds, maxLeds) {
      _type = Rainbow;
      pulseSat = FULL_SATURATION;
    }

    Pulse(CRGB* leds, uint8_t maxLeds, pulseType pulse, int hue) : LEDManager(leds, maxLeds) {
      _type = pulse;
      if (hue >= -1)
        pulseHue = hue;
      pulseSat = (hue == -1 && _type != Random) ? 0 : FULL_SATURATION;
    }

    void play()
    {
      if (brightness < MINIMUM_BRIGHTNESS)
        setBrightness(MINIMUM_BRIGHTNESS);
      if (currBrightness == 0)
      {
        switch (_type)
        {
          case 0:
            break;
          case 1:
            pulseHue = random8(255);
            break;
          case 2:
            pulseHue = hueList[pulseRainbowColor];
            pulseRainbowColor = (pulseRainbowColor >= 5) ? 0 : pulseRainbowColor + 1;
            break;
        }
      }
      (!atColor) ? fadeIn() : fadeOut();
    }
};
