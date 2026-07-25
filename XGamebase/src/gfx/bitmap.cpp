
#include "xsystem.h"
#include "xgfx.h"
#include "xinline.h"

int _XIsSupportedBpp(int bpp)
{
 return
	 (bpp == 8) ||
	 (bpp == 12) ||
	 (bpp == 15) ||
	 (bpp == 16) ||
	 (bpp == 24) ||
	 (bpp == 32);
}

void _XSetBitmapAddress(_XBitmap *b, void *addr)
{
	int i;
	
	b->idat = (U8*)addr;
	b->line[0] = (U8*)addr;
	
	for(i = 1; i < b->h; i++)
		b->line[i] = b->line[i-1] + b->bpl;
}

_XBitmapVTable *_XGetBitmapVTable(int bpp)
{
	if(bpp == 8 ) return &bmpvtbl_8bit_linear;
	else if(bpp == 12) return &bmpvtbl_12bit_linear;
	else if(bpp == 15) return &bmpvtbl_15bit_linear;
	else if(bpp == 16) return &bmpvtbl_16bit_linear;
	else if(bpp == 24) return &bmpvtbl_24bit_linear;
	else if(bpp == 32) return &bmpvtbl_32bit_linear;
	
	_XTrace(_XGetTextf("WARNING: Empty BMPVTBL returned for bpp %d !\n", bpp));
	return &bmpvtbl_empty;
}

_XBitmap *_XCreateBitmap(int w, int h, int bpp, int bpl, void *addr, _XBitmapVTable *vtbl, int id)
{
	int size = sizeof(_XBitmap) + (sizeof(char*) * h);

	_XBitmap *b = (_XBitmap *)malloc(size);	
	if(!b)
	{
		_XLogError("_XCreateBitmap() - Unable to alloc bitmap memory !");
		return NULL;
	}
	
	b->w = w;
	b->h = h;
	b->bpp = bpp;
	b->bpl = (bpl) ? (bpl) : (w*BYTES_PER_PIXEL(bpp));
	b->size = b->h * b->bpl;
	b->id = id;
	
	b->x_ofs = 0;
	b->y_ofs = 0;
	b->parent = NULL;
	
	b->extra = NULL;
	b->user = NULL;
	
	if(vtbl)
		b->vtbl = vtbl;
	else 
		b->vtbl = _XGetBitmapVTable(bpp); 
	
	_XSetBitmapAddress(b, addr);
	
	_XSetClipState(b, 0, 0, w, h);
	
	return b;
}


void _XDestroyBitmap(_XBitmap *bitmap)
{
	if(!bitmap) return;

	if(!(bitmap->id & BMP_ID_OWNDAT))
		_XFree(bitmap->idat);
	
	if(bitmap->user)
		free(bitmap->user);
	
	_XFree(bitmap);
}

_XBitmap *_XCreateBitmap(int w, int h, int bpp)
{
	_XBitmap *b;
	void *dat = NULL;

	if(!_XIsSupportedBpp(bpp))
	{
		_XLogError("_XCreateBitmap() - Unsupported bpp [%d]", bpp);
		return NULL;
	}

	dat = (U8*)malloc((w*BYTES_PER_PIXEL(bpp)) * h);
	if(!dat)
	{
		_XLogError("_XCreateBitmap(%d,%d,%d) - Unable to alloc system data !", w, h, bpp);
		return NULL;
	}

	b = _XCreateBitmap(w, h, bpp, 0, dat, NULL, 0);

	return b;
}

_XBitmap *_XCreateBitmap(_XBitmap *parent, int x, int y, int w, int h)
{
	int i;
	int size = sizeof(_XBitmap) + (sizeof(char*) * h);
	_XBitmap *b;
	
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x+w > parent->w) w = parent->w-x;
	if(y+h > parent->h) h = parent->h-y;
	
	b = (_XBitmap*)malloc(size);
	if(!b)
	{
		_XLogError("_XCreateBitmap() - Unable to alloc bitmap memory !");
		return NULL;
	}
	
	b->w = w;
	b->h = h;
	b->bpp = parent->bpp;
	b->bpl = parent->bpl;
	b->size = parent->size;
	b->id = parent->id | BMP_ID_OWNDAT | BMP_ID_SUBBMP;
	b->vtbl = parent->vtbl;
	b->extra = parent->extra;
	b->user = parent->user;
	b->idat = parent->idat;
	b->x_ofs = x;
	b->y_ofs = y;
	b->parent = parent;
	
	x *= BYTES_PER_PIXEL(parent->bpp);
	for(i = 0; i < h; i++)
		b->line[i] = parent->line[i+y] + x;
	
	_XSetClipState(b, 0, 0, w, h);
	
	return b;
}

