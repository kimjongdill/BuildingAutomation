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

typedef struct node{

	uint32_t device_id;
	int device_state;
	int device_type;
	String device_name;
	time_t time;
	node* pNext;

}NODE;

typedef struct list{

	NODE** map;
	int size;
	int capacity;
	int curr;

}LIST;

typedef struct console{

	int size;
	NODE* front;
	NODE* back;
	LIST* alarm;
	LIST* console;
	LIST* controller;
	LIST* doorbell;
	
}CONSOLE;

static void device_list_remove(CONSOLE* c, NODE* n);
static void device_list_add(CONSOLE* c, NODE* n);

extern int console_id_exist(CONSOLE* c, uint32_t id){
	NODE* pCurr = NULL;

	if(c == NULL){
		return 0;
	}
	
	pCurr = c->front;
	while(pCurr != NULL){
		if(pCurr->device_id == id){
			return 1;
		}
	}
	
	return 0;

}

// Initialize the ADT

static LIST* list_init(){
	
	LIST* newList = (LIST*) malloc(sizeof(LIST));
	newList->map = (NODE**) malloc(sizeof(NODE*)*10);
	for(int i = 0; i < 10; i++){
		newList->map[i] = NULL;
	}
	newList->capacity = 10;
	newList->curr = 0;
	newList->size = 0;
	
	return newList;
}

static void list_grow(LIST* l){
	
	NODE** newMap = (NODE**) malloc(sizeof(NODE*)*l->capacity*2);
	
	for(int i = 0; i < l->capacity; i++){
		newMap[i] = l->map[i];
	}
	for(int i = l->capacity; i < l->capacity * 2; i++){
		newMap[i] = NULL;
	}

	free(l->map);
	l->map = newMap;
	return;
}

extern CONSOLE* console_init(){

	CONSOLE* c = (CONSOLE*) malloc(sizeof(CONSOLE));
	c->size = 0;
	c->front = NULL;
	c->back = NULL;
	c->alarm = list_init();
	c->console = list_init();
	c->controller = list_init();
	c->doorbell = list_init();
	
}

extern void console_free(CONSOLE* c){
	
	NODE* p;
	NODE* q;
	
	if(c == NULL){
		return;
	}
	
	if(c->front == NULL){
		free(c);
		return;
	}
	
	p = c->front;
	while(p != NULL){
		q = p->pNext;
		free(p);
		p = q;
	}
	free(c);
	return;
	
}

extern void console_push(CONSOLE* c, uint32_t id, int data, int type){
	
	if(c == NULL)
		return;
	
	if(console_id_exist(c, id)){
		return;
	}
	NODE* n = (NODE*)malloc(sizeof(NODE));
	n->device_id = id;
	n->device_state = data;
	n->device_type = type;

	if(c->front == NULL){
		c->front = n;
		c->back = n;
		n->pNext = NULL;
		c->size = 1;
	}
	else{
		n->pNext = c->front;
		c->front = n;
		c->size++;
	}
	return;
}

extern void console_delete_id(CONSOLE* c, uint32_t id){

	NODE* pCurr = NULL;
	NODE* pBack = NULL;
	
	if(c == NULL){
		return;
	}
	
	pCurr = c->front;
	pBack = NULL;
	
	while(pCurr != NULL){
		if(pCurr->device_id == id){
			if(pBack == NULL){
				c->front = pCurr->pNext;
			}
			else{
				pBack->pNext = pCurr->pNext;
			}
			if(pCurr == c->back){
				c->back = pBack;
			}
			c->size--;
			break;
		}
		else{
			pBack = pCurr;
			pCurr = pCurr->pNext;
		}
	}

}



extern void console_update_node(CONSOLE* c, uint32_t id, int data, int type){

	NODE* pCurr = NULL;
	
	if(c == NULL){
		return;
	}
	
	pCurr = c->front;
	while(pCurr != NULL){
		if(pCurr->device_id == id){
			pCurr->device_state = data;
			if(pCurr->device_type != type){
				device_list_remove(c, pCurr);
				pCurr->device_type = type;
				device_list_add(c, pCurr);
			}

		}
	}
	
	return;	
}

static int list_contains(std::list<uint32_t> l, uint32_t id){

	auto it = std::find(l.begin(), l.end(), id);
	if(it == l.end()){
		return 0;
	}
	return 1;

}

static int node_contains(uint32_t id, NODE* head){

	while(head != NULL){
		if(head->device_id == id){
			return 1;
		}
	}
	return 0;
}

extern void console_update_node_list(CONSOLE* c, std::list<uint32_t> l){

	NODE* pCurr	= NULL;
	NODE* pLast = NULL;
	
	if(c == NULL){
		return;
	}
	
	pCurr = c->front;
	// Compare linked lists and delete nodes from c that don't appear in l
	while(pCurr != NULL){
		if(!list_contains(l, pCurr->device_id))
			if(pCurr == c->front){
				c->front = pCurr->pNext;
				free(pCurr);
				pCurr = c->front;
				c->size--;
			}
			else if(pCurr = c->back){
				c->back = pLast;
				free(pCurr);
				pLast->pNext = NULL;
				c->size--;
			}
			else{
				pLast->pNext = pCurr->pNext;
				free(pCurr);
				pCurr = pLast->pNext;
				c->size--;
			}
		else{
			pLast = pCurr;
			pCurr = pCurr->pNext;
		}
	}
	
	// Compare linked lists and add nodes to c from l
	for(uint32_t n : l){
		if(!node_contains(n, c->front))
			console_push(c, n, 0, 0); 
	}
	
	
}

static void list_append(LIST* l, NODE* n){

	// If list already contains the NODE, return
	for(int i = 0; i < l->size; i++){
		if(l->map[i] == n){
			return;
		}
	}
	
	// Add to the list
	l->size++;
	if(l->size == l->capacity){
		list_grow(l);
	}
	l->map[l->size] = n;
	
	return;

}

static void list_remove(LIST* l, NODE* n){

	int found = 0;
	// find the node
	for(int i = 0; i < l->size; i++){
		if(l->map[i] == n){
			found = 1;
			
		}
		if(found){
			if(i == l->size - 1){
				l->map[i] = NULL;
			}
			else{
				l->map[i] = l->map[i+1];
			}
		}
	}
	if(found)
		l->size--;
	
	return;
}

static void device_list_add(CONSOLE* c, NODE* n){
	
	
	switch(n->device_type){
		case 1: 
			list_append(c->alarm, n);
			break;
		case 2: 
			list_append(c->console, n);
			break;
		case 3: 
			list_append(c->controller, n);
			break;
		case 4:
			list_append(c->doorbell, n);
			break;
		default:
			break;
	}
	
	return;

}

static void device_list_remove(CONSOLE* c, NODE* n){

	switch(n->device_type){
		case 1: 
			list_append(c->alarm, n);
			break;
		case 2: 
			list_append(c->console, n);
			break;
		case 3: 
			list_append(c->controller, n);
			break;
		case 4:
			list_append(c->doorbell, n);
			break;
		default:
			break;
	}
	
	return;

}

extern void console_get_next_alarm(CONSOLE* c){
}

extern void console_get_next_console(CONSOLE* c, String* name, int* data){

	static int index = 0;
	*name = c->console->map[index]->device_name;
	*data = c->console->map[index]->device_state;
	
	index++;
	if(index >= c->console->size)
		index = 0;
	
	return;
	
}

extern void console_get_next_controller(CONSOLE* c){
}

extern void console_get_next_doorbell(CONSOLE* c){
}
