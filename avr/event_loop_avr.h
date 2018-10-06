#ifndef _AVR_EVENT_LOOP
#define _AVR_EVENT_LOOP

#include <stdint.h>

typedef struct
{

} EventLoop;

typedef void EventLoopUpdateFunc(uint32_t clock);

void event_loop_run(EventLoopUpdateFunc* update_func);

#endif
