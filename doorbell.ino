#include "Adafruit_LiquidCrystal.h"
#include <ArduinoJson.h>
#include "painlessMesh.h"
#include "console.h"


const int dBut = 14, uBut = 0, sBut = 13;
const int rs = 12, en = 16, d4 = 15, d5 = 2, d6 = 5, d7 = 4;

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void sendMessage();

painlessMesh mesh;
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

CONSOLE* c = console_init();
int consoleNum = 0;
char consoleName[100];
int temp = 0;
uint32_t id = 0;

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
    Serial.println(mesh.subConnectionJson());
}

void changedConnectionCallback() {
    Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(dBut, INPUT);
  pinMode(uBut, INPUT);
  pinMode(sBut, INPUT);

  lcd.begin(16, 2);
  //lcd.print("hello");

  attachInterrupt(digitalPinToInterrupt(uBut), prevRemote, FALLING);
  attachInterrupt(digitalPinToInterrupt(dBut), nextRemote, FALLING);
  attachInterrupt(digitalPinToInterrupt(sBut), sendMessage, FALLING);

  mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  //mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  mesh.scheduler.addTask( taskSendMessage );
  taskSendMessage.enable() ;
}

void loop() {
  // put your main code here, to run repeatedly:
  mesh.update();
  screen_update();
  
  //lcd.setCursor(0,0);
  //lcd.clear();
  //lcd.print("Test");
  //delay(200);
  
}

void nextRemote(){
  //Serial.println("UpButton");
  lcd.clear();
  //delay(1000);
  lcd.print("Up");
  static unsigned long intrpt = 0;
  if(millis() - intrpt < 200)
    return;
  intrpt = millis();
  consoleNum++;
  
  return;
}

void prevRemote(){
  //Serial.println("DownButton");
  lcd.clear();
  lcd.print("Down");
  static unsigned long intrpt = 0;
  if(millis() - intrpt < 200)
    return;
  intrpt = millis();
  consoleNum--;
  if(consoleNum<0){
    consoleNum = 0;
  }
  
  return;
}

void screen_update(){
  console_get_info(c, &consoleNum, consoleName, &temp);
  
  lcd.clear();
  lcd.print("Room: ");
  lcd.print(consoleName);
}

void sendMessage(){
  //Serial.println("SendButton");
  //lcd.clear();
  //lcd.print("Send");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& myRequest = jsonBuffer.createObject();
  console_get_info_id(c, &consoleNum, consoleName, &id);
  myRequest["data"] = id;
  myRequest["device_id"] = ESP.getChipId();
  myRequest["type"] = 1;
  myRequest["device_name"] = "Don Rohitleone";
  String request;
  myRequest.printTo(request);
  Serial.print("Sending: ");
  Serial.println(request);
  if(!mesh.sendBroadcast( request ))
    Serial.println("Message Failed");
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  
}


