
#include "xgamebase.h"


static int mem_seek(_XVFile *rwops, int offset)
{
 U8 *newpos;

 newpos = rwops->hidden.mem.here+offset;

 if(newpos < rwops->hidden.mem.base)
  newpos = rwops->hidden.mem.base;

 if(newpos > rwops->hidden.mem.stop)
  newpos = rwops->hidden.mem.stop;

 rwops->hidden.mem.here = newpos;

 return 1;
}


static int mem_seek_set(_XVFile *rwops, int offset)
{
 U8 *newpos;

 newpos = rwops->hidden.mem.base+offset;

 if(newpos < rwops->hidden.mem.base)
  newpos = rwops->hidden.mem.base;

 if(newpos > rwops->hidden.mem.stop)
  newpos = rwops->hidden.mem.stop;

 rwops->hidden.mem.here = newpos;

 return 1;
}


static int mem_read(_XVFile *rwops, void *ptr, int size)
{
 if((rwops->hidden.mem.here + size) > rwops->hidden.mem.stop)
 {
  size -= (rwops->hidden.mem.here + size)-size-rwops->hidden.mem.stop;
 }

 memcpy(ptr, rwops->hidden.mem.here, size);
 rwops->hidden.mem.here += size;

 return size;
}


static int mem_write(_XVFile *rwops, void *ptr, int size)
{
 if((rwops->hidden.mem.here + size) > rwops->hidden.mem.stop)
 {
  size -= (rwops->hidden.mem.here + size)-size-rwops->hidden.mem.stop;
 }

 memcpy(rwops->hidden.mem.here, ptr, size);
 rwops->hidden.mem.here += size;

 return size;
}


static int mem_tell(_XVFile *rwops)
{
 return (rwops->hidden.mem.here - rwops->hidden.mem.base);
}


static int mem_end(_XVFile *rwops)
{
 return (rwops->hidden.mem.here == rwops->hidden.mem.stop);
}


static int mem_close(_XVFile *rwops)
{
 if(rwops->hidden.mem.free)
 {
  _XFree(rwops->hidden.mem.base);
 }

 return 1;
}


_XVFile *_XVFOpenMem(void *mem, int size)
{
 _XVFile *rwops = _XCreateVFile();
 failed_NULL(!rwops, "_XVFOpenMem() - Can't create rwops !");

 rwops->seek = mem_seek;
 rwops->seek_set = mem_seek_set;
 rwops->read = mem_read;
 rwops->write = mem_write;
 rwops->tell = mem_tell;
 rwops->end = mem_end;
 rwops->close = mem_close; 

 rwops->hidden.mem.free = mem ? false : true;

 if(mem == NULL)
 {
  mem = (void*)malloc(size);
  if(!mem)
  {
   _XLogError("_XVFOpenMem() - Unable to alloc mem !");
   free(rwops);
   return NULL;
  }
 }

 rwops->hidden.mem.base = (U8*)mem;
 rwops->hidden.mem.here = rwops->hidden.mem.base;
 rwops->hidden.mem.stop = rwops->hidden.mem.base+size;

 return rwops;
}
