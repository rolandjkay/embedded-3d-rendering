#ifndef _SDL_EVENT_LOOP
#define _SDL_EVENT_LOOP

typedef struct
{

} EventLoop;

typedef void EventLoopUpdateFunc(uint32_t clock);

void event_loop_run(EventLoopUpdateFunc* update_func);

#endif
