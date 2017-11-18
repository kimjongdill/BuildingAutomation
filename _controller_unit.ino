
#include <TimeLib.h>

#include <ArduinoJson.h>

//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time betweew 1 and 5 seconds
// 2. prints anything it recieves to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include "console.h"
#include "Adafruit_LiquidCrystal.h"



#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define   BTN             14
#define   LED_RECV        0
#define   LED_SEND        17
#define   BLR_RLY         16
#define   DES_TEMP        78

void sendMessage() ; // Prototype so PlatformIO doesn't complain

// Initialize data structure to hold remote thermometer data
CONSOLE* c = console_init();

// Start the mesh network
painlessMesh  mesh;
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

// Keep track of time so the boiler isn't always updated
time_t time_last = 0;

// Variables to display on screens
int consoleNum = 0;
char consoleName[100];
int tempF = 0;
int avg = 0;
int minimum = 0;

// Initialize variables controlling the state of the boiler
bool blrState = 0;

// Start the 2 x 16 display
const int rs = 12, en = 13, d4 = 15, d5 = 2, d6 = 5, d7 = 4;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void receivedCallback( uint32_t from, String &msg ) {
  digitalWrite(LED_RECV, HIGH);
  //msg = mesh.subConnectionJson();
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  console_update(c, msg);
  //console_print(c);
  digitalWrite(LED_RECV, LOW);
}

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
  pinMode(LED_RECV, OUTPUT);
  pinMode(LED_SEND, OUTPUT);
  pinMode(BLR_RLY, OUTPUT);
  pinMode(BTN, INPUT);
  lcd.begin(16, 2);

  attachInterrupt(BTN, nextRemote, RISING);
  
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

void nextRemote(){
  static unsigned long intrpt = 0;
  if(millis() - intrpt < 200)
    return;
  intrpt = millis();
  Serial.println("Interrupt");
  consoleNum++;
  return;
}

void state_update(){
  //Serial.print("In State Update");
  if((now() - time_last) < 60){
    return;
  }


  time_last = now(); 
  
  console_get_stats(c, &minimum, &avg);

  Serial.print("Average: ");
  Serial.print(avg);
  Serial.print(" Min: ");
  Serial.print(minimum);

  if(avg < DES_TEMP || minimum < 65){
    blrState = 1;
    digitalWrite(BLR_RLY, HIGH);
    Serial.println(" Boiler On");    
  }
  else{
    blrState = 0;
    digitalWrite(BLR_RLY, LOW);
    Serial.println(" Boiler Off");
  
  }
  
  
}

void screen_update(){

  console_get_info(c, &consoleNum, consoleName, &tempF);

  if(consoleNum == -1){
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Avg:    ");
    lcd.setCursor(5,0);
    lcd.print(avg);
    lcd.print(" Min: ");
    lcd.print(minimum);
    lcd.setCursor(0,1);
    lcd.print("State: ");
    if(blrState){
      lcd.print("On");
    }
    else{
      lcd.print("Off");
    }
    
  }
  else{
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Room:           ");
    lcd.setCursor(6,0);
    lcd.print(consoleName);
  
    lcd.setCursor(0,1);
    lcd.print("Temp:           ");
    lcd.setCursor(6,1);
    lcd.print(tempF);
  } 
  return;
}

void loop() {
  mesh.update();
  state_update();
  screen_update();

}

/*
int getTemp(){
  static int readVal;
  static int tempC;
  static int tempF;
  readVal = analogRead(0);
  tempC = readVal * 1000/1023 / 10 + 2;
  tempF = 9.0 / 5.0 * tempC + 32;
  return tempF;  
}
*/

void sendMessage() {
  digitalWrite(LED_SEND, HIGH);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& myRequest = jsonBuffer.createObject();
  myRequest["data"] = blrState;
  myRequest["device_id"] = ESP.getChipId();
  myRequest["type"] = 3;
  myRequest["device_name"] = "Room 420";
  String request;
  myRequest.printTo(request);
  Serial.print("Sending: ");
  Serial.println(request);
  if(!mesh.sendBroadcast( request ))
    Serial.println("Message Failed");
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  digitalWrite(LED_SEND, LOW);
}
