//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time betweew 1 and 5 seconds
// 2. prints anything it recieves to Serial.print
//
//
//************************************************************
#include <ArduinoJson.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define   BTN             5

bool currentlyAlarming;

void sendMessage() ; // Prototype so PlatformIO doesn't complain

painlessMesh  mesh;
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& myRequest = jsonBuffer.parseObject(msg);
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  pinMode(BTN, INPUT);
  currentlyAlarming = false;
  Serial.begin(115200);
  attachInterrupt(BTN, resetCurAlarm, RISING);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  mesh.scheduler.addTask( taskSendMessage );
  taskSendMessage.enable() ;
}

void loop() {
  mesh.update();
}

void resetCurAlarm(){
  currentlyAlarming = false;
}

int getAlarmBool(){
  int alarmThreshold = 335;
  int analogSensor = analogRead(A0);
  //Serial.print("Pin A0: ");
  //Serial.println(analogSensor);
  //Serial.println(currentlyAlarming);

  if(analogSensor > alarmThreshold)
    currentlyAlarming = true;

  return (analogSensor > alarmThreshold) || currentlyAlarming;
}

void sendMessage() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& myRequest = jsonBuffer.createObject();
  myRequest["data"] = getAlarmBool();
  myRequest["device_id"] = ESP.getChipId();
  myRequest["type"] = 1;
  myRequest["device_name"] = "Fire Alarm";
  String request;
  myRequest.printTo(request);
  Serial.print("Sending: ");
  Serial.println(request);
  mesh.sendBroadcast( request );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}
