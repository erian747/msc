#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


void gui_poll(void);
void gui_init(void);
void gui_graph_new_value(float temp);

#ifdef __cplusplus
}
#endif


#endif

