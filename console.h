#include <Time.h>

typedef struct console CONSOLE;
typedef struct node NODE;

extern void console_delete_id(CONSOLE* c, int id);
extern void console_push(CONSOLE* c, int id, int data, int type);
extern void console_free(CONSOLE* c);
extern CONSOLE* console_init();
extern int console_id_exist(CONSOLE* c, int id);
extern void console_update_node(CONSOLE* c, int id, int data, int type);
