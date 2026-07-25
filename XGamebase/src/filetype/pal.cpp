#include "xgamebase.h"

int _XVFReadPalette(_XVFile *f, _XPalette pal)
{
 int i;

 if(_XVFGet8(f) != 80/*'P'*/)
  return 0;

 if(_XVFGet8(f) != 65/*'A'*/)
  return 0;

 if(_XVFGet8(f) != 76/*'L'*/)
  return 0;

 for(i = 0; i < 256; i++)
 {
  pal[i].r = _XVFGet8(f);
  pal[i].g = _XVFGet8(f);
  pal[i].b = _XVFGet8(f);
 }

 return 1;
}

void _XVFWritePalette(_XVFile *f, _XPalette pal)
{
 int i;

 _XVFPut8(f, 80/*'P'*/);
 _XVFPut8(f, 65/*'A'*/);
 _XVFPut8(f, 76/*'L'*/);

 for(i = 0; i < 256; i++)
 {
  _XVFPut8(f, pal[i].r);
  _XVFPut8(f, pal[i].g);
  _XVFPut8(f, pal[i].b);
 }
}

int _XLoadPalette(char *filename, _XPalette pal)
{
 int ret = 1;

 _XVFile *f = _XVFOpenFile(filename, "rb");
 if(!f)
 {
  _XLogError("_XLoadPalette() - Unable to open file [%s] !", filename);
  return 0;
 }

 if(!_XVFReadPalette(f, pal))
 {
  _XLogError("_XLoadPalette() - Invalid palette file [%s] !", filename);
  ret = 0;
 }

 _XDestroyVFile(f);
 return ret;
}

int _XSavePalette(char *filename, _XPalette pal)
{
 _XVFile *f = _XVFOpenFile(filename, "wb");
 if(!f)
 {
  _XLogError("_XSavePalette() - Unable to open file [%s] !", filename);
  return 0;
 }

 _XVFWritePalette(f, pal);

 _XDestroyVFile(f);
 return 1;
}
