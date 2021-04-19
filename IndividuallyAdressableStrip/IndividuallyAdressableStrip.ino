/*
   DeviceTest.ino - Reads broadcasted messages and creates patterns on LED strips 
   Tyler W. Graham, March 7, 2021
*/

/*
   To Do:
   Pulse pattern replacement
*/

#include "painlessMesh.h"
#include <FastLED.h>
#include "LEDManager.h"
#include "Party.cpp"
#include "Rainbow.cpp"
#include "Rainbow2.cpp"
#include "Pulse.cpp"
#include "Confetti.cpp"
#include "Noise.cpp"
#include "Atmosphere.cpp"

//Everything you need to enter:
#define   MESH_PREFIX     "Prefix"    //Make sure these are the same as defined in MeshServer
#define   MESH_PASSWORD   "Password"
#define   MESH_PORT       5555

#define NUM_LEDS    100     //Number of LEDs on the strip
#define VOLTS       5       //How many volts are being supplied
#define AMPS        10      //How many amps are being supplied
#define LED_PIN     12      //Data pin for strip
#define STRIP_TYPE  WS2812  //The type of strip you have, WS2812 is most common
#define COLOR_ORDER GRB     //The order of colors depending on your strip
//-----------------------------

CRGB leds[NUM_LEDS];

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

TaskHandle_t meshTask;

//LED/Animation variables
uint8_t hue = 0;
uint8_t sat = 255;
uint8_t numColorPresets = 0;
uint8_t hour;
uint8_t minute;
uint8_t temperature;
String weatherConditions;
String weatherDescription;
uint16_t sunriseTime;
uint16_t sunsetTime;
bool power = false;
LEDManager ledManager(leds, NUM_LEDS);

//Message protocols
int deviceNumber = 0;
String message;
String oldMessage;
String sender;
String receiver;
String command;
String pattern;
String color;
String previousMessage;
bool newMessage = false;
uint8_t commandInt;
uint8_t colorInt;
uint8_t patternInt;

void setup() {
  Serial.begin(115200);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  FastLED.addLeds<STRIP_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, AMPS * 1000);
  FastLED.show();

  xTaskCreatePinnedToCore(meshTaskCode, "meshTask", 10000, NULL, 1, &meshTask, 0);
}

void loop() {
  // it will run the user scheduler as well
  //printMessageComponents();
  //printDWMComponents();
  if (newMessage)
  {
    interpretMessage();
    newMessage = false;
  }
  if (isReceiver())
  {
    commandInt = command.toInt();
    colorInt = color.toInt();
    patternInt = pattern.toInt();
    assignHueSaturation(colorInt);
    if (commandInt == 1)
      power = true;
    if (commandInt == 0)
      power = false;

    if (commandInt == 2)
    {
      ledManager.setBrightness(20);
    }
    else if (commandInt == 3)
    {
      ledManager.setBrightness(-20);
    }
    command = power ? "1" : "0";
  }
  if (power)
  {
    switch (patternInt)
    {
      case 0:
        {
          uint8_t b = ledManager.getBrightness();
          ledManager.HSVAll(hue, sat, b);
          break;
        }
      case 1:
        {
          doParty();
          break;
        }
      case 2:
        {
          doRainbow();
          break;
        }
      case 3:
        {
          doRainbow2();
          break;
        }
      case 4:
        {
          if (colorInt < numColorPresets)
          {
            doConfettiRanged(hue, (colorInt == numColorPresets - 1));
          }
          else if (colorInt == numColorPresets)
            doConfettiRanged(random8(255), false);
          else
            doConfetti();

          break;
        }
      case 5:
        {
          /*if (colHue >= -1)
            doLife(colHue);
            else if (colHue == -2)
            doLife();
            else
            doLife(colHue);*/
          break;
        }
      case 6:
        {
          doAtmosphere();
          break;
        }
      case 7:
        {
          //doClock(colHue);
          break;
        }
      case 8:
        {
          if (colorInt < numColorPresets)
          {
            doNoise(hue, (colorInt == numColorPresets - 1), false, false);
          }
          else if (colorInt == numColorPresets)
            doNoise(hue, false, false, true);
          else
            doNoise(hue, false, true, false);
          break;
        }

    }
  }
  else
    ledManager.clearAll();
}

void interpretMessage()
{
  if (message.indexOf("DWM") >= 0)
  {
    String remainingMessage = message.substring(message.indexOf(";") + 1);
    Serial.println(remainingMessage);
    uint8_t hourStop = remainingMessage.indexOf(":");
    hour = remainingMessage.substring(0, hourStop).toInt();
    remainingMessage = remainingMessage.substring(hourStop + 1);
    //Serial.println(remainingMessage);

    uint8_t minuteStop = remainingMessage.indexOf(";");
    minute = remainingMessage.substring(0, minuteStop).toInt();
    remainingMessage = remainingMessage.substring(minuteStop + 1);
    //Serial.println(remainingMessage);

    uint8_t tempStop = remainingMessage.indexOf(";");
    temperature = remainingMessage.substring(0, tempStop).toDouble();
    remainingMessage = remainingMessage.substring(tempStop + 1);
    //Serial.println(remainingMessage);

    uint8_t conditionsStop = remainingMessage.indexOf(";");
    weatherConditions = remainingMessage.substring(0, conditionsStop);
    remainingMessage = remainingMessage.substring(conditionsStop + 1);
    //Serial.println(remainingMessage);

    uint8_t descriptionStop = remainingMessage.indexOf(";");
    weatherDescription = remainingMessage.substring(0, descriptionStop);
    remainingMessage = remainingMessage.substring(descriptionStop + 1);
    //Serial.println(remainingMessage);

    uint8_t sunriseStop = remainingMessage.indexOf(";");
    sunriseTime = remainingMessage.substring(0, sunriseStop).toInt();
    remainingMessage = remainingMessage.substring(sunriseStop + 1);
    //Serial.println(remainingMessage);

    sunsetTime = remainingMessage.toInt();

    message = "";

  }
  else
  {
    if (message.length() > 0 && !message.equalsIgnoreCase("done"))
    {
      uint8_t senderStop = message.indexOf(";") + 1;
      sender = message.substring(0, senderStop);
      String remainingMessage = message.substring(senderStop);

      uint8_t receiverStop = remainingMessage.indexOf(";") + 1;
      receiver = remainingMessage.substring(0, receiverStop);
      remainingMessage = remainingMessage.substring(receiverStop);

      uint8_t commandStop = remainingMessage.indexOf(";") + 1;
      command = remainingMessage.substring(0, commandStop);
      remainingMessage = remainingMessage.substring(commandStop);

      uint8_t patternStop = remainingMessage.indexOf(";") + 1;
      pattern = remainingMessage.substring(0, patternStop);
      remainingMessage = remainingMessage.substring(patternStop);

      color = remainingMessage;
      //message = "done";
    }
  }
}

bool isReceiver() //REWRITE THIS SO YOU DON'T LOOK BRAINDEAD
{
  String modifiedReceiver = receiver;
  while (modifiedReceiver.length() > 0)
  {
    int numberStop = modifiedReceiver.indexOf(",");
    if (numberStop > 0)
    {
      if (modifiedReceiver.substring(0, numberStop).toInt() == deviceNumber)
        return true;
      modifiedReceiver = modifiedReceiver.substring(numberStop + 1);
    }
    else
    {
      return modifiedReceiver.toInt() == deviceNumber;
      break;
    }
    //Serial.println(modifiedReceiver);
  }
  return false;
}

void printMessageComponents()
{
  Serial.print("Sender: ");
  Serial.print(sender);
  Serial.println();
  Serial.print("Receiver: ");
  Serial.print(receiver);
  Serial.println();
  Serial.print("Command: ");
  Serial.print(command);
  Serial.println();
  Serial.print("Pattern: ");
  Serial.print(pattern);
  Serial.println();
  Serial.print("Color: ");
  Serial.print(color);
  Serial.println();
}

void printDWMComponents()
{
  Serial.println(message);
  Serial.print("hour: ");
  Serial.println(hour);
  Serial.print("minute: ");
  Serial.println(minute);
  Serial.print("temp: ");
  Serial.println(temperature);
  Serial.print("condiitons: ");
  Serial.println(weatherConditions);
  Serial.print("description: ");
  Serial.println(weatherDescription);
  Serial.print("sunrise: ");
  Serial.println(sunriseTime);
  Serial.print("sunset: ");
  Serial.println(sunsetTime);
}

void assignHueSaturation(int colorInt)
{
  //red, orange, yellow orange, yellow, green, pale green, teal, light blue, blue, indigo, purple, magenta, white,...random, N/A or cycle
  int hueValues [15] = {0, 21, 32, 42, 85, 106, 127, 149, 160, 192, 212, 233, 0, 0, 0}; //LAST 2 MUST ALWAYS BE 0 AND 0
  hue = hueValues[colorInt];
  sat = (colorInt == 12) ? 0 : 255;
  numColorPresets = sizeof(hueValues) / sizeof(hueValues[0]) - 2;
}

void meshTaskCode( void * pvParameters)
{
  Serial.print("serialTask running on core: ");
  Serial.println(xPortGetCoreID());
  const TickType_t xDelay = 50;
  for (;;)
  {
    mesh.update();
    vTaskDelay( xDelay );
  }
}

//Mesh Methods
void sendMessage() {
  /*String msg = "Hi from device";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));*/
}

// Needed for painless mesh library
void receivedCallback( uint32_t from, String & msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  message = String(msg.c_str());
  newMessage = true;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

//LED Methods
void doParty()
{
  Party party(leds, NUM_LEDS);
  party.play();
}

void doRainbow()
{
  Rainbow rainbow(leds, NUM_LEDS);
  rainbow.play();
}

void doRainbow2()
{
  Rainbow2 rainbow2(leds, NUM_LEDS);
  rainbow2.play();
}

void doConfetti()
{
  Confetti confetti(leds, NUM_LEDS);
  confetti.play();
}

void doConfettiRanged(int hue, bool white)
{
  Confetti confetti(leds, NUM_LEDS, Confetti::Ranged, hue, white);
  confetti.play();
}

void doNoise(int hue, bool white, bool cycle, bool all)
{
  Noise noise(leds, NUM_LEDS, hue, white, cycle, all);
  noise.play();
}

void doAtmosphere()
{
  //Atmosphere atmos(leds, NUM_LEDS, 20, 1, 10, "clouds", "clear", 60, 1410);
  Atmosphere atmos(leds, NUM_LEDS, hour, minute, temperature, weatherConditions, weatherDescription, sunriseTime, sunsetTime);
  atmos.play();
}
