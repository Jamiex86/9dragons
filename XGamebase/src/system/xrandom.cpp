
#include "xgamebase.h"

static unsigned long x = 1;
static unsigned long c = 0;
static unsigned long a = 2083801278UL;

// Initialize ramdom value...
void _XSRandom(U32 seed)
{
 x = 1;
 c = seed;
}

U32 _XRandom(void)
{
 U64 tmp;

 tmp = (U64) x * a + c;
 c = (tmp >> 32) & 0xFFFFFFFFUL;
 x = (tmp & 0xFFFFFFFFUL);

 return x & 0x7FFFFFFFUL;
}

I32 _XRand(I32 max)
{
 return _XRandom()%max;
}

double _XRand(double max)
{
 return ((double)_XRandom() / ((double)2147483647.0f*(max)));
}

I32 _XRand2(I32 min, I32 max)
{
 long nt;

 nt = _XRand(max - min);

 return (nt + min);
}

double _XRand2(double min, double max)
{
 double nt = 0;

 nt = _XRand(max - min);

 return (nt + min);
}
