
#include "xgamebase.h"
#include "xintern.h"

TCHAR gstrErrorMsg[MAX_CHAR_STRING];

void (*gpLogErrorCB)(TCHAR *str) = NULL;

void _XLogError(TCHAR *msg, ...)
{
 TCHAR buf[MAX_CHAR_STRING];
 va_list ap;
 
 if(!msg) return;
 
 va_start(ap, msg);
 vsprintf((TCHAR*)buf, (TCHAR*)msg, ap);
 va_end(ap);

 strcpy(gstrErrorMsg, buf);
 
 _XLog("ERROR: %s\n", gstrErrorMsg);
 
 if(gpLogErrorCB)
  gpLogErrorCB(gstrErrorMsg);
}
