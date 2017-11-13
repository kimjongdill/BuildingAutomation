# BuildingAutomation
Arduino ADT to store wifi mesh node messages and access data

Works with blackedder's <a href = "https://gitlab.com/BlackEdder/painlessMesh ">painlessMesh</a> to track the names and statuses of nodes on a wifi mesh network. 

<ul>The ADT is a singly linked list of data nodes. The nodes store:
  <li>int device_id = The unique id of the sending ESP8266 Module</li>
  <li>int device_type = The type of device sending the message. Console, Controller, Doorbell, Alarm.</li>
  <li>time_t time = The timestamp of the last message</li>
  <li>String device_name = A descriptive name of the device eg. "George's Apartment"</li>
</ul>
 
<ul>The Linked List is stored in a public data node CONSOLE which stores:
  <li>The number of nodes on the list</li>
  <li>A pointer to the front of the list</li>
  <li>A pointer to the rear of the list</li>
</ul>

<ul>Supported Operations (So Far):
  <li>extern void console_delete_id(CONSOLE* c, int id)</br>
    Delete a node from the linked list by its device_id</li>
  <li>extern void console_push(CONSOLE* c, int id, int data, int type)</br>
    Add a new device to the list of known nodes</li>
  <li>extern void console_free(CONSOLE* c)</br>
    Free the entire list from memory</li>
  <li>extern CONSOLE* console_init()</br>
    Initialize a new instance</li>
  <li>extern int console_id_exist(CONSOLE* c, int id)</br>
    Returns 1 if the id is on the list of consoles, 0 if it is not</li>
  <li>extern void console_update_node(CONSOLE* c, int id, int data, int type)</br>
    Update the data in the node of the device specified by the id</li>
</ul>
