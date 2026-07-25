
#include "xgamebase.h"
#include "xintern.h"
#include <time.h>

static int clock_mode = 0;
static U64 rate = 0;

U32 sys_XReadTimer(void)
{
 U64 tmp;

 if(!clock_mode)
  QueryPerformanceCounter((LARGE_INTEGER*)&tmp);
 else
  tmp = clock();

 return CAST(U32,(U64)tmp/((U64)rate/(U64)1000));
}

int sys_XTimerStartup(void)
{
 if(!QueryPerformanceFrequency((LARGE_INTEGER*)&rate))
 {
  clock_mode = 1;
  rate = 1000;
 }
 else
 {
  clock_mode = 0;
 }

 return 1;
}

void sys_XTimerShutdown(void)
{
 clock_mode = 0;
}
