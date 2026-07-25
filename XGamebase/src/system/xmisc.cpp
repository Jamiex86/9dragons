
#include "xgamebase.h"
#include "xintern.h"
#include <math.h>

int		gnOSType = OSTYPE_UNKNOWN;

U8 gstrTextfBuff[8000];

U8 *_XGetTextf(U8 *buf, U8 *s, ...)
{
 va_list ap;

 if(!s)
  return NULL;

 if(!buf)
  buf = _XMalloc(U8,8000);

 va_start(ap, s);
 vsprintf((TCHAR*)buf, (TCHAR*)s, ap);
 va_end(ap);

 return buf;
}

U8 *_XGetTextf(U8 *s, ...)
{
 va_list ap; 

 if(!s)
  return NULL;

 va_start(ap, s);
 vsprintf((TCHAR*)gstrTextfBuff, (TCHAR*)s, ap);
 va_end(ap);

 return gstrTextfBuff;
}
