/*
 * Ripples
 * 
 * Originally by: Reko Meriö
 * Highly modified by: Andrew Tuline
 * Mofified again by: Tyler Graham
 * Date: February 18, 2021
 */
/*
  Rain.cpp - Rain pattern
  Originally created by Reko Meriö
  Modified by Andrew Tuline
  Modified again by Tyler Graham, March 7, 2021
*/

#include "LEDManager.h"

#define MAX_RIPPLES     6
#define REFRESH_SPEED   60
#define maxRipples      6
#define FADE_TIME       80

static uint8_t NUM_LEDS;
static CRGBPalette16 currentPalette = OceanColors_p;
static bool isRain;
static bool isStorm;

//----------------- Ripple structure definition ----------------------------------------------------------------

struct ripple {                                                                 // Reko Meriö's structures

  // Local definitions

  // Persistent local variables

  // Temporary local variables
  uint8_t brightness;                                   // 0 to 255
  int8_t color;                                         // 0 to 255
  int8_t pos;                                           // -1 to NUM_LEDS  (up to 127 LED's)
  int8_t velocity;                                      // 1 or -1
  uint8_t life;                                         // 0 to 20
  uint8_t maxLife = 5;                                      // 10. If it's too long, it just goes on and on. . .
  uint8_t fade;                                         // 192
  bool exist;                                           // 0 to 1


  void Move() {

    pos += velocity;
    life++;

    if (pos > NUM_LEDS - 1) {                           // Bounce back from far end.
      velocity *= -1;
      pos = NUM_LEDS - 1;
    }

    if (pos < 0) {                                      // Bounce back from 0.
      velocity *= -1;
      pos = 0;
    }

    brightness = scale8(brightness, fade);              // Adjust brightness accordingly to strip length

    if (life > maxLife) exist = false;                  // Kill it once it's expired.

  } // Move()



  void Init(uint8_t Fade, uint8_t MaxLife, uint8_t Brightness) {                 // Typically 216, 20

    pos = random8(0, NUM_LEDS); // Avoid spawning too close to edge.
    velocity = 1;                                       // -1 or 1
    life = 0;                                           // 0 to Maxlife
    maxLife = MaxLife;                                  // 10 called
    exist = true;                                       // 0, 1
    brightness = Brightness;                                   // 0 to 255
    color = millis();                                   // hue;
    fade = Fade;                                        // 192 called

  } // Init()

}; // struct ripple

typedef struct ripple Ripple;

static Ripple ripples[maxRipples];

class Rain : public LEDManager
{
  private:

    void animateRipples()
    {
      for (int i = 0; i < maxRipples; i += 2) {                 // Check to see if ripple has expired, and if so, create a new one.
        if (random8() > 220 && !ripples[i].exist) {             // Randomly create a new ripple if this one has expired.
          if(isRain)
            ripples[i].Init(192, 5, getBrightness());                             // Initialize the ripple array with Fade and MaxLife values.
          else
            ripples[i].Init(192, 1, getBrightness());
          ripples[i + 1] = ripples[i];                          // Everything except velocity is the same for the ripple's other half. Position starts out the same for both halves.
          ripples[i + 1].velocity *= -1;                        // We want the other half of the ripple to go opposite direction.
        }
      }

      for (int i = 0; i < maxRipples; i++) {                    // Move the ripple if it exists
        if (ripples[i].exist) {
          if(isRain)
            _leds[ripples[i].pos] = ColorFromPalette(currentPalette, ripples[i].color, ripples[i].brightness, LINEARBLEND);
          else
            _leds[ripples[i].pos] = CHSV(0, 0, ripples[i].brightness);
          ripples[i].Move();
        }
      }

      fadeToBlackBy(_leds, NUM_LEDS, FADE_TIME);
    }

    void animateLightning()
    {
      if(random(200) < 1)
      {
        HSVAll(0, 0, getBrightness());
        fadeToBlackBy(_leds, NUM_LEDS, FADE_TIME/2);
      }
    }

  public:
    Rain(CRGB* leds, uint8_t maxLeds, bool rain, bool storm) : LEDManager(leds, maxLeds) {
      NUM_LEDS = maxLeds;
      isRain = rain;
      isStorm = storm;
    }

    void play()
    {
      EVERY_N_MILLIS(REFRESH_SPEED)
      {
        animateRipples();
        if(isStorm)
          animateLightning();
      }
      FastLED.show();
    }
};
