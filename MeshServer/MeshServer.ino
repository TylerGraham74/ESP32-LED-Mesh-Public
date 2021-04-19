/*
   MeshServer.ino - Reads data from serial connection and broadcasts using PainlessMesh.h for LED Mesh Network
   Tyler W. Graham, March 7, 2021
*/


//Everything you need to enter:
#define   MESH_PREFIX     "Prefix"      //It really doesn't matter what these are, as long as they are the same on each board
#define   MESH_PASSWORD   "Password"
#define   MESH_PORT       5555
//-----------------------------

#include "painlessMesh.h"

// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

void sendMessage();//fixes compile-time error

TaskHandle_t serialTask;

String message = "";
bool newMessage = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // For sending data to another ESP32

  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  xTaskCreatePinnedToCore(serialTaskCode, "serialTask", 10000, NULL, 1, &serialTask, 0);

}

void loop() {
  if (newMessage)
    sendMessage();

  mesh.update();
}

void serialTaskCode( void * pvParameters)
{
  Serial.print("serialTask running on core: ");
  Serial.println(xPortGetCoreID());
  const TickType_t xDelay = 50;
  for (;;)
  {
    if (Serial2.available())
    {
      message = Serial2.readString();
      Serial.print(message);
      newMessage = true;
    }
    vTaskDelay( xDelay );
  }
}

//Mesh Methods
void sendMessage() {
  String msg = message;
  mesh.sendBroadcast(msg);
  newMessage = false;
  //taskSendMessage.setInterval(TASK_SECOND * 1);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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
