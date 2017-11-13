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
#include <Time.h>

typedef struct node{

	int device_id;
	int device_state;
	int device_type;
	time_t time;
	node* pNext;

}NODE;

typedef struct console{

	int size;
	NODE* front;
	NODE* back;
	
}CONSOLE;

extern int console_id_exist(CONSOLE* c, int id){
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

extern CONSOLE* console_init(){

	CONSOLE* c = (CONSOLE*) malloc(sizeof(CONSOLE));
	c->size = 0;
	c->front = NULL;
	c->back = NULL;

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

extern void console_push(CONSOLE* c, int id, int data, int type){
	
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

extern void console_delete_id(CONSOLE* c, int id){

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

extern void console_update_node(CONSOLE* c, int id, int data, int type){

	NODE* pCurr = NULL;
	
	if(c == NULL){
		return;
	}
	
	pCurr = c->front;
	while(pCurr != NULL){
		if(pCurr->device_id == id){
			pCurr->device_state = data;
			pCurr->device_type = type;
		}
	}
	
	return;	


}

