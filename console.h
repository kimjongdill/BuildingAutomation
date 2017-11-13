#include <list>
#include <algorithm>

typedef struct console CONSOLE;
typedef struct node NODE;

extern void console_delete_id(CONSOLE* c, uint32_t id);
extern void console_push(CONSOLE* c, uint32_t id, int data, int type);
extern void console_free(CONSOLE* c);
extern CONSOLE* console_init();
extern int console_id_exist(CONSOLE* c, uint32_t id);
extern void console_update_node(CONSOLE* c, uint32_t id, int data, int type);
extern void console_update_node_list(CONSOLE* c, std::list<uint32_t> l);

extern void console_get_next_alarm(CONSOLE* c);

extern void console_get_next_console(CONSOLE* c, String *name, int *data);

extern void console_get_next_controller(CONSOLE* c);

extern void console_get_next_doorbell(CONSOLE* c);
