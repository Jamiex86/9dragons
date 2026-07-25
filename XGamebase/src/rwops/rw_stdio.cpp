
#include "xgamebase.h"


static int stdio_seek(_XVFile *rwops, int offset)
{ 
 if(fseek(rwops->hidden.stdio.fp, offset, SEEK_CUR))
  return 0;

 return 1;
}


static int stdio_seek_set(_XVFile *rwops, int offset)
{ 
 if(fseek(rwops->hidden.stdio.fp, offset, SEEK_SET))
  return 0;

 return 1;
}


static int stdio_read(_XVFile *rwops, void *ptr, int size)
{
 return fread(ptr, 1, size, rwops->hidden.stdio.fp);
}


static int stdio_write(_XVFile *rwops, void *ptr, int size)
{
 return fwrite(ptr, 1, size, rwops->hidden.stdio.fp);
}


static int stdio_tell(_XVFile *rwops)
{
 return ftell(rwops->hidden.stdio.fp);
}


static int stdio_end(_XVFile *rwops)
{
 return feof(rwops->hidden.stdio.fp) ? 1 : 0;
}


static int stdio_close(_XVFile *rwops)
{
 if(rwops->hidden.stdio.close)
 {
  if(fclose(rwops->hidden.stdio.fp))
   return 0;
 }

 return 1;
}


_XVFile *_XVFOpenFile(TCHAR *filename, TCHAR *mode)
{
 FILE *fp;

 fp = fopen(filename, mode);
 if(!fp)
 {
  //_XLogError("_XVFOpenFile(%s) - Can't open file !", filename);
  _XMessage( "_XVFOpenFile(%s) - Can't open file !", filename );
  return NULL;
 }

 return _XVFOpenFP(fp, 1);
}


_XVFile *_XVFOpenFP(FILE *fp, int close)
{
 _XVFile *rwops = _XCreateVFile();
 failed_NULL(!rwops, "_XVFOpenFP() - Can't create file template !");

 rwops->seek = stdio_seek;
 rwops->seek_set = stdio_seek_set;
 rwops->read = stdio_read;
 rwops->write = stdio_write;
 rwops->tell = stdio_tell;
 rwops->end = stdio_end; 
 rwops->close = stdio_close;
 rwops->memory = false; 
 rwops->hidden.stdio.fp = fp;
 rwops->hidden.stdio.close = close;

 return rwops;
}
