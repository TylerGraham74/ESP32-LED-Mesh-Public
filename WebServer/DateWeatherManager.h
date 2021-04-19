/*
   DateWeatherManger.h - A library to manage all date and weather functions
   Tyler W. Graham, April 2, 2020
*/

#ifndef dwm
#define dwm

#define DDMMYY  0
#define MMDDYY  1
#define YYMMDD  2

#include "Arduino.h"
#include <NTPClient.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <TimeLib.h>

class DateWeatherManager
{
  public:
    //Constructor
    DateWeatherManager(String city, String state, String APIKey, char* ssid, char* password, int timezone);

    //Variables
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    //Methods
    void begin();
    void updateTime();
    void updateWeather();
    String getFormattedDate();
    String getWeatherConditions();
    String getWeatherDescription();
    double getTemperature(bool celsius);
    uint8_t getHours();
    uint8_t getMinutes();
    uint8_t getSeconds();
    String getWeatherCode();
    String getBasicDate(uint8_t format);
    String toString(bool celsius);
    uint16_t getSunriseTime();
    uint16_t getSunsetTime();

  private:
    //Variables
    int _timezone;
    String endpoint;
    String _city;
    String _state;
    String _APIKey;
    char* _ssid;
    char* _password; //CHANGE TO PROTECTED
    String weatherCode;
    unsigned long lastUpdated;

    //Methods
    void setLocation();
    void setAPIKey();

};

#endif
