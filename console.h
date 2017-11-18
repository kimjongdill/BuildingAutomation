#include <list>
#include <algorithm>
#include <ArduinoJson.h>

typedef struct console CONSOLE;
typedef struct node NODE;


extern CONSOLE* console_init();
extern int console_update(CONSOLE* c, String msg);
extern void console_print(CONSOLE* c);
extern void console_get_stats(CONSOLE* c, int* minimum, int* avg);
extern void console_get_info(CONSOLE* c, int* index, char name[100], int* temp);
