#include <SDL2/SDL.h>
#include <sys/time.h>
#include "event_loop_sdl.h"

// Timer callback function which diverts event back to the main thread
/* with the same code as before: */
static Uint32 timer_callbackfunc(Uint32 interval, void *param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  /* In this example, our callback pushes a function
  into the queue, and causes our callback to be called again at the
  same interval: */

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return(interval);
}

void event_loop_run(EventLoopUpdateFunc* update_func)
{
  int quit = 0;
  uint32_t clock = 0; // We pass this timestamp to the update function.
  SDL_Event event;

  struct timeval start_tv, tv;
  int e = gettimeofday(&start_tv, NULL);

  if (e != 0)
  {
    fprintf(stderr, "Couldn't get time of day.");
    return;
  }


  // Start the refresh timer
  Uint32 delay = 100;  /* To round it down to the nearest 10 ms */
  SDL_TimerID refresh_timer_id = SDL_AddTimer(delay, timer_callbackfunc, NULL);

  while (!quit)
  {
    SDL_WaitEvent(&event);
    switch (event.type)
    {
     case SDL_QUIT:
       quit = 1;
       break;

     // Our redraw timer fired
     case SDL_USEREVENT:
       // Call update func
       if (update_func)
       {
         e = gettimeofday(&tv, NULL);

         if (e != 0)
         {
           fprintf(stderr, "Couldn't get time of day\n");
           clock += 100;
         }
         else
         {
           // Calculate clock in 1024th of a second.
           clock = (tv.tv_sec - start_tv.tv_sec) << 10;
           clock += (tv.tv_usec - start_tv.tv_usec) / 1024;
           printf("clock=%d\n", clock);
         }

         update_func(clock);
       }
       break;
    }
  }

  SDL_RemoveTimer(refresh_timer_id);
}
