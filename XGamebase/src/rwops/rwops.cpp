#include "xgamebase.h"


_XVFile *_XCreateVFile(void)
{
 _XVFile *rwops = _XMalloc(_XVFile,1);

 if(!rwops)
  return NULL;

 rwops->seek = NULL;
 rwops->seek_set = NULL;
 rwops->read = NULL;
 rwops->write = NULL;
 rwops->tell = NULL; 
 rwops->end = NULL;
 rwops->close = NULL;
 rwops->hidden.user.data = NULL;

 rwops->memory = false; 

 return rwops;
}


int _XDestroyVFile(_XVFile *rwops)
{
 int ret = 0;

 if(rwops)
 {
  ret = rwops->close(rwops);
  free(rwops);
  rwops = NULL;
 }

 return ret;
}


int _XVFSeek(_XVFile *rwops, int offset)
{
 if(!rwops)
  return 0;

 return rwops->seek(rwops, offset); 
}


int _XVFSeekSet(_XVFile *rwops, int offset)
{
 if(!rwops)
  return 0;

 return rwops->seek_set(rwops, offset); 
}


int _XVFRead(_XVFile *rwops, void *ptr, int size)
{
 if(!rwops) return -1;
 return rwops->read(rwops, ptr, size);
}


int _XVFWrite(_XVFile *rwops, void *ptr, int size)
{
 if(!rwops) return -1;
 return rwops->write(rwops, ptr, size);
}


int _XVFTell(_XVFile *rwops)
{
 if(!rwops)
  return -1;

 return rwops->tell(rwops);
}


int _XVFEnd(_XVFile *rwops)
{
 if(!rwops)
  return 1;

 return rwops->end(rwops);
}


float _XVFGetF32(_XVFile *rwops)
{
 float buf = 0;
 _XVFRead(rwops, (void*)&buf, 4);
 return buf;
}


double _XVFGetF64(_XVFile *rwops)
{
 double buf = 0;
 _XVFRead(rwops, (void*)&buf, 8);
 return buf;
}


U8 _XVFGet8(_XVFile *rwops)
{
 U8 buf = 0;
 _XVFRead(rwops, (void*)&buf, 1);
 return buf;
}


U16 _XVFGetLE16(_XVFile *rwops)
{
 U16 buf = 0;
 _XVFRead(rwops, (void*)&buf, 2);
 return SWAP_LE16(buf);
}


U32 _XVFGetLE32(_XVFile *rwops)
{
 U32 buf = 0;
 _XVFRead(rwops, (void*)&buf, 4);
 return SWAP_LE32(buf);
}


U64 _XVFGetLE64(_XVFile *rwops)
{
 U64 buf = 0;
 _XVFRead(rwops, (void*)&buf, 8);
 return SWAP_LE64(buf);
}


U16 _XVFGetBE16(_XVFile *rwops)
{
 U16 buf = 0;
 _XVFRead(rwops, (void*)&buf, 2);
 return SWAP_BE16(buf);
}


U32 _XVFGetBE32(_XVFile *rwops)
{
 U32 buf = 0;
 _XVFRead(rwops, (void*)&buf, 4);
 return SWAP_BE32(buf);
}


U64 _XVFGetBE64(_XVFile *rwops)
{
 U64 buf = 0;
 _XVFRead(rwops, (void*)&buf, 8);
 return SWAP_BE64(buf);
}


int _XVFGetStr(_XVFile *rwops, U8 *str, int maxsize)
{
 U8 buf[MAX_CHAR_STRING];
 int pos = 0;
 int c = 0;

 if(_XVFEnd(rwops))
 {
  str[0] = 0;
  return 0;
 }

 while(pos < MAX_CHAR_STRING)
 {
  c = _XVFGet8(rwops);
  if(c == '\n' || c == '\r' || c == '\0')
   break;

  buf[pos++] = c;
 }

 buf[pos] = 0;

 strncpy((TCHAR*)str, (TCHAR*)buf, maxsize);

 return 1;
}


int _XVFPutF32(_XVFile *rwops, float val)
{
 return _XVFWrite(rwops, &val, 4);
}


int _XVFPutF64(_XVFile *rwops, double val)
{
 return _XVFWrite(rwops, &val, 8);
}


int _XVFPut8(_XVFile *rwops, U8 val)
{
 return _XVFWrite(rwops, &val, 1);
}


int _XVFPutLE16(_XVFile *rwops, U16 val)
{
 val = SWAP_LE16(val);
 return _XVFWrite(rwops, &val, 2);
}


int _XVFPutLE32(_XVFile *rwops, U32 val)
{
 val = SWAP_LE32(val);
 return _XVFWrite(rwops, &val, 4);
}


int _XVFPutLE64(_XVFile *rwops, U64 val)
{
 val = SWAP_LE64(val);
 return _XVFWrite(rwops, &val, 8);
}


int _XVFPutBE16(_XVFile *rwops, U16 val)
{
 val = SWAP_BE16(val);
 return _XVFWrite(rwops, &val, 2);
}


int _XVFPutBE32(_XVFile *rwops, U32 val)
{
 val = SWAP_BE32(val);
 return _XVFWrite(rwops, &val, 4);
}


int _XVFPutBE64(_XVFile *rwops, U64 val)
{
 val = SWAP_BE64(val);
 return _XVFWrite(rwops, &val, 8);
}


int _XVFPutStr(_XVFile *rwops, U8 *str)
{
 int pos = 0;
 int len = strlen((TCHAR*)str);
 TCHAR buf[MAX_CHAR_STRING];
 memset( buf, 0, MAX_CHAR_STRING );

 strncpy((TCHAR*)buf, (TCHAR*)str, MAX_CHAR_STRING);

 while((pos < len) && (buf[pos]))
 {
  _XVFPut8(rwops, buf[pos++]);
 }

 return 1;
}
