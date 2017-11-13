# BuildingAutomation
Arduino ADT to store wifi mesh node messages and access data

Works with blackedder's <a href = "https://gitlab.com/BlackEdder/painlessMesh ">painlessMesh</a> to track the names and statuses of nodes on a wifi mesh network. 

The ADT is a singly linked list of data nodes. The nodes store:
  * int device_id = The unique id of the sending ESP8266 Module
  * int device_type = The type of device sending the message. Console, Controller, Doorbell, Alarm.
  * time_t time = The timestamp of the last message
  * String device_name = A descriptive name of the device eg. "George's Apartment"
 
The Linked List is stored in a public data node CONSOLE which stores:
  * The number of nodes on the list
  * A pointer to the front of the list
  * A pointer to the rear of the list

Supported Operations (So Far):
  extern void console_delete_id(CONSOLE* c, int id)
    Delete a node from the linked list by its device_id
  extern void console_push(CONSOLE* c, int id, int data, int type)
    Add a new device to the list of known nodes
  extern void console_free(CONSOLE* c)
    Free the entire list from memory
  extern CONSOLE* console_init()
    Initialize a new instance
  extern int console_id_exist(CONSOLE* c, int id)
    Returns 1 if the id is on the list of consoles, 0 if it is not
  extern void console_update_node(CONSOLE* c, int id, int data, int type)
    Update the data in the node of the device specified by the id
