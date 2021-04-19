/*
   DateWeatherManger.cpp - Manages all date and weather functions
   Tyler W. Graham, April 2, 2020
*/

#include "DateWeatherManager.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
HTTPClient http;

DateWeatherManager::DateWeatherManager(String city, String state, String APIKey, char* ssid, char* password, int timezone)
{
  _city = city;
  _state = state;
  _APIKey = APIKey;
  _ssid = ssid;
  _password = password;
  _timezone = timezone;
  endpoint = "http://api.openweathermap.org/data/2.5/weather?q=" + _city + "," + _state + "&appid=" + _APIKey;
  lastUpdated = 0;
}

void DateWeatherManager::begin()
{
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(50);
  }
  timeClient.begin();
  timeClient.setTimeOffset(_timezone);
  updateWeather();
}

void DateWeatherManager::updateTime()
{
  timeClient.forceUpdate();
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  //formattedDate = timeClient.getFormattedDate();
  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  seconds = timeClient.getSeconds();
}

void DateWeatherManager::updateWeather()
{
  if ((millis() - lastUpdated) < 1000 && lastUpdated != 0) //protection for 60 call limit from OpenWeather
  {
    return;
  }
  http.begin(endpoint);
  if (http.GET() > 0)
    weatherCode = http.getString();
  else
    weatherCode = "HTTP GET ERROR";
  http.end();
  lastUpdated = millis();
}

uint8_t DateWeatherManager::getHours()
{
  updateTime();
  return timeClient.getHours();
}

uint8_t DateWeatherManager::getMinutes()
{
  updateTime();
  return timeClient.getMinutes();
}

uint8_t DateWeatherManager::getSeconds()
{
  updateTime();
  return timeClient.getSeconds();
}

String DateWeatherManager::getWeatherCode()
{
  return weatherCode;
}

String DateWeatherManager::getBasicDate(uint8_t format)
{
  int strEnd = getFormattedDate().indexOf("T");
  String temp = getFormattedDate().substring(0, strEnd);
  String day = temp.substring(8, temp.length());
  String month = temp.substring(5, 7);
  String year = temp.substring(0, 4);
  String date = "";
  switch (format)
  {
    case DDMMYY:
      date = day + "-" + month + "-" + year;
      //date = "ddmmyy";
      break;
    case MMDDYY:
      date = month + "-" + day + "-" + year;
      //date = "mmddyy";
      break;
    case YYMMDD:
      date = year + "-" + month + "-" + day;
      //date = "yymmdd";
      break;
  }
  return date;
}

String DateWeatherManager::getFormattedDate()
{
  return timeClient.getFormattedDate();
}

String DateWeatherManager::getWeatherConditions()
{
  updateWeather();
  int strStart = weatherCode.indexOf("weather") + 28;
  int strEnd = weatherCode.indexOf("description") - 3;
  return weatherCode.substring(strStart, strEnd);
}

String DateWeatherManager::getWeatherDescription()
{
  updateWeather();
  int strStart = weatherCode.indexOf("description") + 14;
  int strEnd = weatherCode.indexOf("icon") - 3;
  return weatherCode.substring(strStart, strEnd);
}

double DateWeatherManager::getTemperature(bool celsius)
{
  updateWeather();
  int strStart = weatherCode.indexOf("temp") + 6;
  int strEnd = weatherCode.indexOf("feels_like") - 2;
  double tempKelvin = weatherCode.substring(strStart, strEnd).toDouble();
  if (celsius)
    return tempKelvin - 273.15;
  else
    return (tempKelvin - 273.15) * 9 / 5 + 32;
}

String DateWeatherManager::toString(bool celsius)
{
  String hour = (String)(getHours());
  String minute = (String)(getMinutes());
  String temp = (String)(getTemperature(celsius));
  String weatherConditions = getWeatherConditions();
  String weatherDescription = getWeatherDescription();
  String sunriseTime = (String)getSunriseTime();
  String sunsetTime = (String)getSunsetTime();
  String weatherMessage = "DWM;" + hour + ":" + minute + ";" + temp + ";" + weatherConditions + ";" +  weatherDescription + ";" + sunriseTime + ";" + sunsetTime;
  return weatherMessage;
}

uint16_t DateWeatherManager::getSunriseTime()
{
  updateWeather();
  String sunriseString = weatherCode.substring(weatherCode.indexOf("sunrise"));
  uint16_t strStart = sunriseString.indexOf("sunrise") + 9;
  uint16_t strEnd = sunriseString.indexOf(",");
  long sunriseTime = sunriseString.substring(strStart, strEnd).toInt();
  char buff[32];
  sprintf(buff, "%02d:%02d", hour(sunriseTime), minute(sunriseTime));
  String buffer = buff;
  uint8_t sunriseHour = buffer.substring(0, buffer.indexOf(":")).toInt();
  sunriseHour = (sunriseHour == 0) ? 24 : sunriseHour;
  uint8_t sunriseMinute = buffer.substring(buffer.indexOf(":") + 1).toInt();
  return (sunriseHour * 60) + sunriseMinute + (_timezone / 60);
}

uint16_t DateWeatherManager::getSunsetTime()
{
  updateWeather();
  String sunsetString = weatherCode.substring(weatherCode.indexOf("sunset"));
  uint16_t strStart = sunsetString.indexOf("sunset") + 8;
  uint16_t strEnd = sunsetString.indexOf("}");
  long sunsetTime = sunsetString.substring(strStart, strEnd).toInt();
  char buff[32];
  sprintf(buff, "%02d:%02d", hour(sunsetTime), minute(sunsetTime));
  String buffer = buff;
  uint8_t sunsetHour = buffer.substring(0, buffer.indexOf(":")).toInt();
  sunsetHour = (sunsetHour == 0) ? 24 : sunsetHour;
  uint8_t sunsetMinute = buffer.substring(buffer.indexOf(":") + 1).toInt();
  return (sunsetHour * 60) + sunsetMinute + (_timezone / 60);
}
