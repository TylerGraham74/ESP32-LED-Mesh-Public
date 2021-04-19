/*
   Atmosphere.cpp - Atmosphere pattern 
   Tyler W. Graham, March 7, 2021
*/

#include "LEDManager.h"
#include "WeatherNoise.cpp"
#include "Rain.cpp"
#include "Twinkle.cpp"

#define HOT_HUE   233
#define WARM_HUE  21
#define COOL_HUE  106
#define COLD_HUE  160
#define CLOUD_HUE 160

class Atmosphere : public LEDManager
{
  private:
    uint8_t _hour;
    uint8_t _minute;
    uint8_t _temperature;
    String _weatherConditions;
    String _weatherDescription;

    uint16_t totalMinutes;
    uint16_t _sunriseTime;
    uint16_t _sunsetTime;
    uint8_t timeOfDay; //0 = sunrise, 1 = day, 2 = sunset, 3 = night

    void checkTimeOfDay()
    {
      if (abs(totalMinutes - _sunriseTime) <= 15)
      {
        timeOfDay = 0;
      }
      else if ((totalMinutes > _sunriseTime + 15) && (totalMinutes < _sunsetTime - 15))
        timeOfDay = 1;
      else if (abs(totalMinutes - _sunsetTime) <= 15)
        timeOfDay = 2;
      else
        timeOfDay = 3;
    }

    void playClearPatterns()
    {

      switch (timeOfDay)
      {
        case 0:
          {
            Twinkle dawnTwinkle(_leds, _maxLeds, 224, 288, FULL_SATURATION);
            dawnTwinkle.play();
            break;
          }
        case 1:
          {
            uint8_t tempHue;
            if (_temperature > 90)
            {
              tempHue = HOT_HUE;
            }
            else if (_temperature > 60)
            {
              tempHue = WARM_HUE;
            }
            else if (_temperature > 40)
            {
              tempHue = COOL_HUE;
            }
            else
            {
              tempHue = COLD_HUE;
            }
            WeatherNoise weatherNoise(_leds, _maxLeds, tempHue, false, false, false, false);
            weatherNoise.play();
            break;
          }
        case 2:
          {
            Twinkle duskTwinkle(_leds, _maxLeds, 175, 280, FULL_SATURATION);
            duskTwinkle.play();
            break;
          }
        case 3:
          {
            Twinkle starTwinkle(_leds, _maxLeds, 0, 0, 0);
            starTwinkle.play();
            break;
          }
      }
    }

    void playCloudyPatterns()
    {
      WeatherNoise cloudNoise(_leds, _maxLeds, CLOUD_HUE, false, false, false, true);
      cloudNoise.play();
    }

    void playRainyPatterns()
    {
      Rain rain(_leds, _maxLeds, true, false);
      rain.play();
    }

    void playStormPatterns()
    {
      Rain storm(_leds, _maxLeds, true, true);
      storm.play();
    }

    void playSnowPatterns()
    {
      Rain snow(_leds, _maxLeds, false, true);
      snow.play();
    }

  public:
    Atmosphere(CRGB* leds, uint8_t maxLeds, uint8_t hour, uint8_t minute, uint8_t temperature, String weatherConditions, String weatherDescription, uint16_t sunriseTime, uint16_t sunsetTime) : LEDManager(leds, maxLeds) {
      _leds = leds;
      _hour = hour;
      _minute = minute;
      _temperature = temperature;
      _weatherConditions = weatherConditions;
      _weatherDescription = weatherDescription;
      _sunriseTime = sunriseTime;
      _sunsetTime = sunsetTime;
    }

    void play()
    {
      totalMinutes = (_hour * 60) + _minute;
      checkTimeOfDay();
      if (_weatherConditions.equalsIgnoreCase("clouds"))
      {
        playCloudyPatterns();
      }
      else if (_weatherConditions.equalsIgnoreCase("rain") || _weatherConditions.equalsIgnoreCase("drizzle"))
      {
        playRainyPatterns();
      }
      else if (_weatherConditions.equalsIgnoreCase("thunderstorm"))
      {
        playStormPatterns();
      }
      else if (_weatherConditions.equalsIgnoreCase("snow"))
      {
        playSnowPatterns();
      }
      else
      {
        playClearPatterns();
      }
    }

};
