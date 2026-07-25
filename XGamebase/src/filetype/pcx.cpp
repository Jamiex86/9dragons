#include "xgamebase.h"

bool _XVFIsPCX(_XVFile *fp)
{
 if(_XVFGet8(fp) != 10)
  return false;

 if(_XVFGet8(fp) != 5)
  return false;

 if(_XVFGet8(fp) != 1)
  return false;

 return true;
}

_XBitmap *_XVFReadPCX(_XVFile *rwops, _XRGB *pal)
{
 _XBitmap *b;
 int c;
 int width, height;
 int bpp, bytes_per_line;
 int xx, po;
 int x, y;
 char ch;

 if(!_XVFIsPCX(rwops))
 {
  _XLogError("_XVFReadPCX() - Invalid PCX file !");
 }

 if(_XVFGet8(rwops) != 8)
 {
  _XLogError("_XVFReadPCX() - Can load only 8bit pcx data !");
  return NULL;
 }

 width = -(_XVFGetLE16(rwops));
 height = -(_XVFGetLE16(rwops));
 width += _XVFGetLE16(rwops) + 1;
 height += _XVFGetLE16(rwops) + 1;

 _XVFGetLE32(rwops);

 for(c = 0; c < 16; c++)
 {
  if(pal)
  {
   pal[c].r = _XVFGet8(rwops);
   pal[c].g = _XVFGet8(rwops);
   pal[c].b = _XVFGet8(rwops);
  }
  else
  {
   _XVFGet8(rwops);
   _XVFGet8(rwops);
   _XVFGet8(rwops);
  }
 }

 _XVFGet8(rwops);

 bpp = _XVFGet8(rwops) * 8;
 if(bpp != 8 && bpp != 24)
 {
  _XLogError("_XVFReadPCX() - Can load only 8bit and 24bit pcx data !");
  return NULL;
 }

 bytes_per_line = _XVFGetLE16(rwops);

 for(c = 0; c < 60; c++)
  _XVFGet8(rwops);

 b = _XCreateBitmap(width, height, bpp);
 if(!b)
 {
  _XLogError("_XVFReadPCX() - Can't create bitmap !");
  return NULL;
 }

 for(y = 0; y < height; y++)
 {
  x = xx = 0;
  po = _rgb_r_shift_24/8;

  while(x < bytes_per_line*bpp/8)
  {
   ch = _XVFGet8(rwops);
   if((ch & 0xC0) == 0xC0)
   {
	c = (ch & 0x3F);
	ch = _XVFGet8(rwops);
   }
   else
   {
	c = 1;
   }

   if(bpp == 8)
   {
	while(c--)
	{
	 if(x < b->w)
	  b->line[y][x] = ch;
	 x++;
	}
   }
   else
   {
	while(c--)
	{
	 if(xx < b->w)
	  b->line[y][xx*3+po] = ch;
	 x++;

	 if(x == bytes_per_line)
	 {
	  xx = 0;
	  po = _rgb_g_shift_24/8;
	 }
	 else if(x == bytes_per_line*2)
	 {
	  xx = 0;
	  po = _rgb_b_shift_24/8;
	 }
	 else
	 {
	  xx++;
	 }
	}
   }
  }
 }

 if(pal)
 {
  if(bpp == 8)
  {
   do
   {
	if(!_XVFRead(rwops, &ch, 1))
	{
	 _XLogError("_XVFReadPCX() - Can't found palette !");
	 _XDestroy(Bitmap, b);
	 return NULL;
	}
   } while(ch != 12);

   for(c = 0; c < 256; c++)
   {
	pal[c].r = _XVFGet8(rwops);
	pal[c].g = _XVFGet8(rwops);
	pal[c].b = _XVFGet8(rwops);
   }
  }
  else
  {
   _XGenerate332Palette(pal);
  }
 }

 return b;
}

/*

  Ecrit des donnees au format pcx

*/
int _XVFWritePCX(_XVFile *rwops, _XBitmap *bmp, _XRGB *pal)
{
 int c;
 int x, y;
 int runcount;
 int depth, planes;
 char runchar;
 char ch;

 depth = bmp->bpp;

 if(depth == 8)
  planes = 1;
 else
  planes = 3;

 _XVFPut8(rwops, 10);
 _XVFPut8(rwops, 5);
 _XVFPut8(rwops, 1); 
 _XVFPut8(rwops, 8);
 _XVFPutLE16(rwops, 0);
 _XVFPutLE16(rwops, 0); 
 _XVFPutLE16(rwops, bmp->w-1);
 _XVFPutLE16(rwops, bmp->h-1);
 _XVFPutLE16(rwops, 320);
 _XVFPutLE16(rwops, 200);

 for(c = 0; c < 16; c++)
 {
  if(pal)
  {
   _XVFPut8(rwops, pal[c].r);
   _XVFPut8(rwops, pal[c].g);
   _XVFPut8(rwops, pal[c].b);
  }
  else
  {
   _XVFPut8(rwops, 0);
   _XVFPut8(rwops, 0);
   _XVFPut8(rwops, 0);
  }
 }

 _XVFPut8(rwops, 0);
 _XVFPut8(rwops, planes);
 _XVFPutLE16(rwops, bmp->w);
 _XVFPutLE16(rwops, 1);
 _XVFPutLE16(rwops, bmp->w);
 _XVFPutLE16(rwops, bmp->h);

 for(c = 0; c < 54; c++)
  _XVFPut8(rwops, 0);

 for(y = 0; y < bmp->h; y++)
 {
  runcount = 0;
  runchar = 0;

  for(x = 0; x < bmp->w*planes; x++)
  {
   if(depth == 8)
   {
	ch = get_pixel(bmp, x, y);
   }
   else
   {
	if(x < bmp->w)
	{
	 c = get_pixel(bmp, x, y);
	 ch = _XGetREx(c, depth);
	}
	else if(x < bmp->w*2)
	{
	 c = get_pixel(bmp, x-bmp->w, y);
	 ch = _XGetGEx(c, depth);
	}
	else
	{
	 c = get_pixel(bmp, x-bmp->w*2, y);
	 ch = _XGetBEx(c, depth);
	}
   }

   if(runcount == 0)
   {
	runcount = 1;
	runchar = ch;
   }
   else
   {
	if((ch != runchar) || (runcount >= 0x3f))
	{
	 if((runcount > 1) || ((runchar & 0xC0) == 0xC0))
	  _XVFPut8(rwops, 0xC0 | runcount);
	 _XVFPut8(rwops, runchar);
	 runcount = 1;
	 runchar = ch;
	}
	else
	 runcount++;
   }
  }

  if((runcount > 1) || ((runchar & 0xC0) == 0xC0))
   _XVFPut8(rwops, 0xC0 | runcount);
  _XVFPut8(rwops, runchar);
 }

 if(depth == 8)
 {
  _XVFPut8(rwops, 12);
  for(c = 0; c < 256; c++)
  {
   if(pal)
   {
	_XVFPut8(rwops, pal[c].r);
	_XVFPut8(rwops, pal[c].g);
	_XVFPut8(rwops, pal[c].b);
   }
   else
   {
	_XVFPut8(rwops, 0);
	_XVFPut8(rwops, 0);
	_XVFPut8(rwops, 0);
   }
  }
 }

 return 1;
}
