/*
   WebServer.ino - Hosts a webpage and reads input for LED Mesh Network
   Tyler W. Graham, March 7, 2021
*/

//Everything you need to enter:
char* ssid = "SSID";          //Your network SSID
char* password = "password";  //Your network password
#define NUM_DEVICES 3         //The number of LED receivers there will be
String deviceNames[NUM_DEVICES] = {"Bed Strip", "Panel", "Kitchen"};  //The names of those receivers; deviceNames[0] should be the name of the device with the deviceNumber 0
char* city = "City";      //Your current city
char* state = "State";    //Your current state
char* APIKey = "APIKey";  //Your OpenWeatherMap API key
int timezone = -21600;    //Based off of GMT
//-----------------------------

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "DateWeatherManager.h"

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

//Network/Web Interface
WiFiServer server(80);
String header;
const int ledOutput = 26;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
TaskHandle_t serverTask;

//Message Protocols
String message;
String sender = "server";
String receiver = "99"; //receiver must be a number which will be converted to an int on receiving device's end
uint8_t command;
uint8_t pattern;
uint8_t color;
bool sendMessage = false;

//DateWeatherManager
unsigned long startMillis;
unsigned long currentMillis;
#define DWM_MILLIS 300000
DateWeatherManager dateWeatherManager(city, state, APIKey, ssid, password, timezone);

void setup() {
  Serial.begin(115200); //For sending/receiving data to serial monitor
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // For sending data to another ESP32

  pinMode(ledOutput, OUTPUT);
  digitalWrite(ledOutput, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  xTaskCreatePinnedToCore(serverTaskCode, "serverTask", 10000, NULL, 1, &serverTask, 0);

  //Print IP address to OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(WiFi.localIP());
  display.display();

  dateWeatherManager.begin();
  Serial2.println(dateWeatherManager.toString(false));
  startMillis = millis();
}

void serverTaskCode( void * pvParameters)
{
  Serial.print("serverTask running on core: ");
  Serial.println(xPortGetCoreID());
  const TickType_t xDelay = 50;
  for (;;)
  {
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
      currentTime = millis();
      previousTime = currentTime;
      Serial.println("New Client.");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
        currentTime = millis();
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              //SET VALUES HERE!!
              //Set pattern/command
              if (header.indexOf("GET /modeSolid") >= 0)
              {
                pattern = 0;
              }
              else if (header.indexOf("GET /modeParty") >= 0)
              {
                pattern = 1;
              }
              else if (header.indexOf("GET /modeRainbow") >= 0)
              {
                pattern = 2;
              }
              else if (header.indexOf("GET /modeRainbow1") >= 0)
              {
                pattern = 3;
              }
              else if (header.indexOf("GET /modeConfetti") >= 0)
              {
                pattern = 4;
              }
              else if (header.indexOf("GET /modeLife") >= 0)
              {
                pattern = 5;
              }
              else if (header.indexOf("GET /modeAtmosphere") >= 0)
              {
                pattern = 6;
              }
              else if (header.indexOf("GET /modeClock") >= 0)
              {
                pattern = 7;
              }
              else if (header.indexOf("GET /modeNoise") >= 0)
              {
                pattern = 8;
              }

              //Set color
              if (header.indexOf("GET /colRed") >= 0)
                color = 0;
              else if (header.indexOf("GET /colOrange") >= 0)
                color = 1;
              else if (header.indexOf("GET /colYellowOrange") >= 0)
                color = 2;
              else if (header.indexOf("GET /colYellow") >= 0)
                color = 3;
              else if (header.indexOf("GET /colGreen") >= 0)
                color = 4;
              else if (header.indexOf("GET /colPaleGreen") >= 0)
                color = 5;
              else if (header.indexOf("GET /colTeal") >= 0)
                color = 6;
              else if (header.indexOf("GET /colLightBlue") >= 0)
                color = 7;
              else if (header.indexOf("GET /colBlue") >= 0)
                color = 8;
              else if (header.indexOf("GET /colIndigo") >= 0)
                color = 9;
              else if (header.indexOf("GET /colPurple") >= 0)
                color = 10;
              else if (header.indexOf("GET /colMagenta") >= 0)
                color = 11;
              else if (header.indexOf("GET /colWhite") >= 0)
                color = 12;
              else if (header.indexOf("GET /colRand") >= 0)
                color = 13;
              else if (header.indexOf("GET /colNA") >= 0)
                color = 14;

              //Set receiver
              if (header.indexOf("GET /action_page") >= 0)
              {
                String deviceHeader = header.substring(header.indexOf("?") + 1, header.indexOf(" HTTP"));
                String devicesInHeader = "";
                for (;;)
                {
                  int numberStart = deviceHeader.indexOf("=") + 1;
                  int deviceBreak = deviceHeader.indexOf("&");
                  if (deviceBreak > 0)
                  {
                    devicesInHeader += deviceHeader.substring(numberStart, deviceBreak) + ",";
                    deviceHeader = deviceHeader.substring(deviceBreak + 1);
                  }
                  else
                  {
                    devicesInHeader += deviceHeader.substring(numberStart);
                    break;
                  }
                }
                receiver = devicesInHeader;
                sendMessage = true;
              }

              //Set command
              if (header.indexOf("GET /deviceOn") >= 0)
                command = 1;
              if (header.indexOf("GET /deviceOff") >= 0)
                command = 0;

              //Brightness
              if (header.indexOf("GET /bUp") >= 0)
                command = 2;
              if (header.indexOf("GET /bDown") >= 0)
                command = 3;

              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");

              // Change background color
              client.println("<body style=\"background-color: black;\">");

              // CSS to style the buttons
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #555555; border: none; color: white; padding: 16px 24px;");
              client.println("text-decoration: none; font-size: 30px; margin: 0px; cursor: pointer; position: absolute; left: 130px; top: 88px;}");
              client.println(".button2 { background-color: #4CAF50; border: none; color: white; padding: 16px 33px;");
              client.println("text-decoration: none; font-size: 30px; margin: 1px; cursor: pointer; position: absolute; left: 8px; top: 87px;}");
              client.println(".colorButtons{position: absolute; left: 10px; border: none; width: 50px; height: 50px; padding: 1px 1px; cursor: pointer; color: white;}");
              client.println(".rButton {background-color: #FF0000; top: 167px;}");//R column start
              client.println(".oButton {background-color: #FF8000; top: 227px;}");
              client.println(".yoButton {background-color: #FFBF00; top: 287px;}");
              client.println(".yButton {background-color: #FFFF00; top: 347px;}");
              client.println(".wButton {background-color: #FFFFFF; top: 407px;}");//R column finish
              client.println(".gButton {background-color: #00FF00; left: 70px; top: 167px;}");//G column start
              client.println(".pgButton {background-color: #00FF80; top: 227px; left: 70px;}");
              client.println(".tButton {background-color: #00FFFF; top: 287px; left: 70px;}");
              client.println(".lbButton {background-color: #0080FF; top: 347px; left: 70px;}");
              client.println(".randButton {background-color: #555555; top: 407px; left: 70px;}");//G column finish
              client.println(".bButton {background-color: #0000FF; left: 130px; top: 167px;}");//B column start
              client.println(".iButton {background-color: #7401DF; top: 227px; left: 130px;}");
              client.println(".pButton {background-color: #FF00FF; top: 287px; left: 130px;}");
              client.println(".mButton {background-color: #DF0174; top: 347px; left: 130px;}");
              client.println(".naButton {background-color: #555555; top: 407px; left: 130px;}");//B column finish
              client.println(".brightUpButton {position: absolute; left: 188px; top: 167px; padding: 5px 5px; width: 50px; height: 110px;}");
              client.println(".brightDownButton {position: absolute; left: 188px; top: 287px; padding: 5px 5px; width: 50px; height: 110px;}");
              client.println(".modeButton {background-color: #555555; border: none; color: white; padding: 1px 1px; text-decoration: none; font-size: 16px; width: 100px; height: 60px;}");
              client.println("input[type=submit] {background-color: #555555; border: none; color: white; padding: 10px 40px; text-decoration: none; margin: 2px; cursor: pointer;}");
              client.println("</style></head>");

              // Web Page Heading
              client.println("<body><h1 style=\"color:white;\">LED Mesh Interface</h1>");

              //On and Off buttons
              client.println("<p><a href=\"/deviceOn\"><button class=\"button button2\">ON</button></a></p>");
              client.println("<p><a href=\"/deviceOff\"><button class=\"button\">OFF</button></a></p>");
              //Color buttons
              client.println("<div>");
              client.println("<p> <a href=\"/colRed\"><button class=\"colorButtons rButton\"></button></a>");//first row
              client.println("<a href=\"/colGreen\"><button class=\"colorButtons gButton\"></button></a>");
              client.println("<a href=\"/colBlue\"><button class=\"colorButtons bButton\"></button></a></p>");
              client.println("<p> <a href=\"/colOrange\"><button class=\"colorButtons oButton\"></button></a>");//second row
              client.println("<a href=\"/colPaleGreen\"><button class=\"colorButtons pgButton\"></button></a>");
              client.println("<a href=\"/colIndigo\"><button class=\"colorButtons iButton\"></button></a></p>");
              client.println("<p> <a href=\"/colYellowOrange\"><button class=\"colorButtons yoButton\"></button></a>");//third row
              client.println("<a href=\"/colTeal\"><button class=\"colorButtons tButton\"></button></a>");
              client.println("<a href=\"/colPurple\"><button class=\"colorButtons pButton\"></button></a></p>");
              client.println("<p> <a href=\"/colYellow\"><button class=\"colorButtons yButton\"></button></a>");//fourth row
              client.println("<a href=\"/colLightBlue\"><button class=\"colorButtons lbButton\"></button></a>");
              client.println("<a href=\"/colMagenta\"><button class=\"colorButtons mButton\"></button></a></p>");
              client.println("<p> <a href=\"/colWhite\"><button class=\"colorButtons wButton\"></button></a>");
              client.println("<a href=\"/colRand\"><button class=\"colorButtons randButton\">RAND</button></a>");
              client.println("<a href=\"/colNA\"><button class=\"colorButtons naButton\">N/A</button></a>");
              client.println("</div>");

              //Brightness buttons
              client.println("<div>");
              client.println("<p> <a href=\"/bUp\"><button class=\"button brightUpButton\">+</button></a>");
              client.println("<a href=\"/bDown\"><button class=\"button brightDownButton\">-</button></a></p>");
              client.println("</div>");

              //Mode buttons
              client.println("<div>");
              client.println("<p style = \"position: absolute; left: 10px; top: 475px;\"><a href=\"/modeSolid\"><button class=\"modeButton\">SOLID</button></a></p>");
              client.println("<p style = \"position: absolute; left: 115px; top: 475px;\"><a href=\"/modeParty\"><button class=\"modeButton\">PARTY</button></a></p>");
              client.println("<p style = \"position: absolute; left: 220px; top: 475px; \"><a href=\"/modeRainbow\"><button class=\"modeButton\">RAINBOW</button></a></p>");
              client.println("<p style = \"position: absolute; left: 10px; top: 540px;\"><a href=\"/modeRainbow1\"><button class=\"modeButton\">RAINBOW 2</button></a></p>");
              client.println("<p style = \"position: absolute; left: 115px; top: 540px;\"><a href=\"/modeConfetti\"><button class=\"modeButton\">CONFETTI</button></a></p>");
              client.println("<p style = \"position: absolute; left: 220px; top: 540px;\"><a href=\"/modeLife\"><button class=\"modeButton\">LIFE</button></a></p>");
              client.println("<p style = \"position: absolute; left: 10px; top: 605px;\"><a href=\"/modeAtmosphere\"><button class=\"modeButton\">ATMOS.</button></a></p>");
              client.println("<p style = \"position: absolute; left: 115px; top: 605px;\"><a href=\"/modeClock\"><button class=\"modeButton\">CLOCK</button></a></p>");
              client.println("<p style = \"position: absolute; left: 220px; top: 605px;\"><a href=\"/modeNoise\"><button class=\"modeButton\">NOISE</button></a></p>");
              client.println("</div>");

              //Device selection
              client.println("<div>");
              client.println("<form action=\"/action_page.php\">");
              int fromTop = 70;
              for (int i = 0; i < NUM_DEVICES; i++)
              {

                String pixelsFromTop = String(fromTop);
                String deviceNumber = String(i);
                client.println("<p style = \"position: absolute; left: 250px; top: " + pixelsFromTop + "px; color: white;\"><input type=\"checkbox\" id=\"device" + deviceNumber + "\" name=\"device" + deviceNumber + "\" value=\"" + deviceNumber + "\"> ");
                client.println("<label for=\"device" + deviceNumber + "\"> " + deviceNames[i] + "</label><br>");
                fromTop += 20;
                if (i == NUM_DEVICES - 1)
                {
                  pixelsFromTop = String(fromTop);
                  client.println("<p style = \"position: absolute; left: 250px; top: " + pixelsFromTop + "px; color: white;\"><input type=\"submit\" value=\"SUBMIT\">");
                }
              }

              client.println("</form>");
              client.println("</div>");

              client.println("</body></html>");

              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
    vTaskDelay( xDelay );
  }
}

void loop() {
  message = sender + ";" + receiver + ";" + command + ";" + pattern + ";" + color;
  if (sendMessage)
  {
    //Serial.println(message);
    Serial2.println(message);
    sendMessage = false;
  }
  currentMillis = millis();
  if (currentMillis - startMillis >= DWM_MILLIS)  //test whether the period has elapsed
  {
    //    Serial.println(dateWeatherManager.toString(false));
    //    Serial.println(dateWeatherManager.getWeatherCode());
    Serial2.println(dateWeatherManager.toString(false));
    startMillis = currentMillis;
  }
  delay(10);
}
