#include "xgamebase.h"
#include "xintern.h"

static struct {
 int mode;
 U32 timer;
 U32 msec;
 void (*func)(void);
} timer_func[MAX_TIMER_FUNC];;


bool _XTimerStarted = 0;

U32 _XReadTimer(void)
{
 return sys_XReadTimer();
}


U32 _XElapsedMSec(U32 *timer)
{
 return sys_XReadTimer()-(*timer);
}


void _XResetTimer(U32 *timer)
{
 *timer = sys_XReadTimer();
}

// Install system timer
int _XTimerStartup(void)
{
 if(_XTimerStarted) return 1;

 failed_int(!sys_XTimerStartup(), "ERROR : Could not create game timer!" );

 sys_XReadTimer();

 _XAddExitFunc(_XTimerShutdown);
 _XTimerStarted = 1;
 _XLog("Start game timer.");

 return 1;
}

void _XTimerShutdown(void)
{
 if(!_XTimerStarted)
  return;

 sys_XTimerShutdown();

 _XRemoveExitFunc(_XTimerShutdown);
 _XTimerStarted = 0;
          
 _XLog("========================================================================");
 _XLog("Release game timer");
}

int _XAddTimer(void (*func)(void), int msec, int mode)
{
 int i;

 _XRemoveTimer(func);

 for(i = 0; i < MAX_TIMER_FUNC; i++)
 {
  if(!timer_func[i].func)
  {
   _XResetTimer(&timer_func[i].timer);
   timer_func[i].msec = msec;
   timer_func[i].func = func;
   timer_func[i].mode = mode;
   return 1;
  }
 }

 return 0;
}

void _XRemoveTimer(void (*func)(void))
{
 int i;

 for(i = 0; i < MAX_TIMER_FUNC; i++)
 {
  if(timer_func[i].func == func)
  {
   _XResetTimer(&timer_func[i].timer);
   timer_func[i].msec = 0;
   timer_func[i].func = NULL;
  }
 }
}

void _XPollTimer(void)
{
 int i;
 U32 msec;

 for(i = 0; i < MAX_TIMER_FUNC; i++)
 {
  if(timer_func[i].func)
  {
   msec = _XElapsedMSec(&timer_func[i].timer);
   if(msec >= timer_func[i].msec)
   {
    _XResetTimer(&timer_func[i].timer);
    if(timer_func[i].mode == TIMER_LUNCHALL)
    {
     while(msec >= timer_func[i].msec)
     { 
      timer_func[i].func();
      msec -= timer_func[i].msec;
     }
    }
    else
    {
     timer_func[i].func();
    }
   } 
  }
 }
}

void _XRest(U32 msec)
{
 U32 timer;

 _XResetTimer(&timer);

 while(_XElapsedMSec(&timer) < msec)
  ;
}

void _XRest(U32 msecx, U32 msecy, void (*callback)(void))
{
 U32 x, cbrun = 0;
 U32 timer;

 _XResetTimer(&timer);

 while((x = _XElapsedMSec(&timer)) < msecx)
 {
  if(x >= (cbrun * msecy))
  {
   callback();
   cbrun++;
  }
 }
}

static U32 fps_timer = 0, framecounter = 0;
U32 _XGetFPS(void)
{
 int msec = 0;
 if(framecounter)
  msec = _XElapsedMSec(&fps_timer);
 else
  msec = 1;
 
 _XResetTimer(&fps_timer);
 framecounter++;

 if( msec == 0 )
	 return 0;
 else 
	return 1000/msec;
}

float _XGetFPSf(void)
{
 int msec = 0;
 if(framecounter)
  msec = _XElapsedMSec(&fps_timer);
 else
  msec = 1;

 _XResetTimer(&fps_timer);
 framecounter++;

 return ((float)1000/(float)msec);
}
