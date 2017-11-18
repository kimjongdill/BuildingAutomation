/*
*  George Dill - November 2017
*  Library to store nodes representing the state
*  of sensors on a wifi mesh network
* 
*  Each sensor has a unique id, payload, type, and 
*  last reported time.
*
*  
*
*/

#include <Arduino.h>
#include "console.h"
#include <list>
#include <algorithm>
#include <ArduinoJson.h>

typedef struct node{

	uint32_t device_id;
	int device_data;
	char device_name[100];

}NODE;

typedef struct console{

	NODE** presentConsoles;
	int numConsoles;
	int maxConsoles;

}CONSOLE;


// Initialize an array of struct that hold information about
// the consoles on the network. Initialize to size three
extern CONSOLE* console_init(){

	CONSOLE* c = (CONSOLE*) malloc(sizeof(CONSOLE));
	c->presentConsoles =  (NODE**) malloc(sizeof(NODE*)*3);

	c->numConsoles = 0;
	c->maxConsoles = 3;	
	
	return c;
}

static NODE* find_ID(CONSOLE* c, uint32_t id){
	if(c->numConsoles == 0)
		return NULL;
	//Serial.println("Searching for node match");
	for(int i = 0; i < c->numConsoles; i++){
		//Serial.print("Searching For: ");
		//Serial.print(id);
		//Serial.print("In Node: ");
		//Serial.println(c->presentConsoles[i]->device_id);
		
		if(c->presentConsoles[i]->device_id == id){
			//Serial.println("ID Found");
			return c->presentConsoles[i];
		}
	}		
	//Serial.println("ID not Found");
	return NULL;
}

static void double_nodes(CONSOLE* c){

	int size = c->numConsoles;
	NODE** newList = (NODE**)malloc(sizeof(NODE*)*2*size);
	
	for(int i = 0; i < size; i++){
		newList[i] = c->presentConsoles[i];
	}
	
	free(c->presentConsoles);
	c->presentConsoles = newList;
	c->maxConsoles *= 2;
	return;

}

static NODE* new_node(CONSOLE* c){
	if(c->numConsoles == c->maxConsoles)
		double_nodes(c);
		
	NODE* n = (NODE*)malloc(sizeof(NODE));
	c->presentConsoles[c->numConsoles] = n;
	c->numConsoles++;
	//n->device_name = new String();
	
	return n;

}

extern void console_print(CONSOLE* c){
	Serial.println("Printing Console Info");
	for(int i = 0; i < c->numConsoles; i++){
		Serial.println(c->presentConsoles[i]->device_name);
		Serial.println(c->presentConsoles[i]->device_id);
		Serial.println(c->presentConsoles[i]->device_data);
		
	}
	return;
}

extern void console_get_stats(CONSOLE* c, int* minimum, int* avg){
	if(c->numConsoles == 0)
		return;
	
	int sum = 0;

	int currTemp = 0;
	*minimum = 1000;
		
	for(int i = 0; i < c->numConsoles; i++){
		currTemp = c->presentConsoles[i]->device_data;
		if(currTemp < *minimum)
			*minimum = currTemp;
		
		sum+= currTemp;
	}
	*avg = sum / c->numConsoles;
	return;
}

extern void console_get_info(CONSOLE* c, int* index, char name[100], int* temp){
	
	if(c == NULL || c->numConsoles == 0){
		return;
	}
	
	if(*index >= c->numConsoles || *index < 0){
		*index = -1;
		return;
	}
	
	strcpy(name, c->presentConsoles[*index]->device_name);
	*temp = c->presentConsoles[*index]->device_data;
	return;
}

// Read the JSON message and determine whether to update the console list
extern int console_update(CONSOLE* c, String msg){
	
	//Serial.println("In Console_update");
	NODE* n = NULL;
	
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& myRequest = jsonBuffer.parseObject(msg);
	if((int)myRequest["type"] != 2) 
		return 0;
	
	//Serial.println("This is a console unit");	
	// Check if unit ID is already on list
	
	n = find_ID(c, myRequest["device_id"]);
	
	// If its not, see if we can make a new node
	if(n == NULL){
		if(c->numConsoles < c->maxConsoles){
			n = new_node(c);
		}
		else {
			return 0;
		}
			
	}
	
	// I have a node, so populate it
	n->device_data = (int)myRequest["data"];
	n->device_id = (uint32_t)myRequest["device_id"];
	strcpy(n->device_name, myRequest["device_name"]);
	
	return 1;
	
}
