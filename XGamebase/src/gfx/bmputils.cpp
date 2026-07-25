#include "xsystem.h"
#include "xgfx.h"
#include "xinline.h"

void _XSetClipState(_XBitmap *bitmap, int x1, int y1, int x2, int y2)
{
	if (x2 < x1)
		_XSwap(x2, x1);
	
	if (y2 < y1)
		_XSwap(y2, y1);
	
	if(x2 == 0)
		x2 = bitmap->w;
	
	if(y2 == 0)
		y2 = bitmap->h;
	
	x2++;
	y2++;
	
	bitmap->cl = MID(0, x1, bitmap->w-1);
	bitmap->ct = MID(0, y1, bitmap->h-1);
	bitmap->cr = MID(0, x2, bitmap->w);
	bitmap->cb = MID(0, y2, bitmap->h);
}

void _XPushClipState(_XBitmap *bmp, _XClipState *pc)
{
	pc->cr = bmp->cr;
	pc->cl = bmp->cl;
	pc->ct = bmp->ct;
	pc->cb = bmp->cb;
}

void _XPopClipState(_XBitmap *bmp, _XClipState *pc)
{
	bmp->cr = pc->cr;
	bmp->cl = pc->cl;
	bmp->ct = pc->ct;
	bmp->cb = pc->cb;
}

_XBitmap *_XBlurBitmap(_XBitmap *bmp, int destroy)
{
	int x, y, r1, g1, b1, r2, g2, b2,
		r3, g3, b3, r4, g4, b4,
		r5, g5, b5, mr, mg, mb, ps;
	
#define GETCOLOR(num, x, y) \
	ps = get_pixel(bmp, x, y); \
	r##num = _XGetREx(ps, bmp->bpp); \
	g##num = _XGetGEx(ps, bmp->bpp); \
	b##num = _XGetBEx(ps, bmp->bpp)
	
#define PIX() put_pixel(tmp, x, y, _XRGBEx(mr, mg, mb, bmp->bpp))
	
	_XBitmap *tmp = _XCreateBitmap(bmp->w, bmp->h, bmp->bpp);
	if(!tmp)
		return NULL;
	
	x = 0;
	for(y = 1; y < bmp->h-1; y++)
	{
		GETCOLOR(1, x, y);
		GETCOLOR(2, x, y-1);
		GETCOLOR(3, x+1, y);
		GETCOLOR(4, x, y+1);
		mr = (r1+r2+r3+r4)>>2;
		mg = (g1+g2+g3+g4)>>2;
		mb = (b1+b2+b3+b4)>>2;
		PIX();
	}
	
	x = bmp->w-1;
	for(y = 1; y < bmp->h-1; y++)
	{
		GETCOLOR(1, x, y);
		GETCOLOR(2, x, y-1);
		GETCOLOR(3, x-1, y);
		GETCOLOR(4, x, y+1);
		mr = (r1+r2+r3+r4)>>2;
		mg = (g1+g2+g3+g4)>>2;
		mb = (b1+b2+b3+b4)>>2;
		PIX();
	}
	
	y = 0;
	for(x = 1; x < bmp->w-1; x++)
	{
		GETCOLOR(1, x, y);
		GETCOLOR(2, x-1, y);
		GETCOLOR(3, x+1, y);
		GETCOLOR(4, x, y+1);
		mr = (r1+r2+r3+r4)>>2;
		mg = (g1+g2+g3+g4)>>2;
		mb = (b1+b2+b3+b4)>>2;
		PIX();
	}
	
	y = bmp->h-1;
	for(x = 1; x < bmp->w-1; x++)
	{
		GETCOLOR(1, x, y);
		GETCOLOR(2, x-1, y);
		GETCOLOR(3, x+1, y);
		GETCOLOR(4, x, y-1);
		mr = (r1+r2+r3+r4)>>2;
		mg = (g1+g2+g3+g4)>>2;
		mb = (b1+b2+b3+b4)>>2;
		PIX();
	}
	
	{
		x = 0; y = 0;
		GETCOLOR(1, x, y);
		GETCOLOR(2, x+1, y);
		GETCOLOR(3, x, y+1);
		mr = (r1+r2+r3) / 3;
		mg = (g1+g2+g3) / 3;
		mb = (b1+b2+b3) / 3;
		PIX();
	}
	
	{
		x = bmp->w-1; y = 0;
		GETCOLOR(1, x, y);
		GETCOLOR(2, x-1, y);
		GETCOLOR(3, x, y+1);
		mr = (r1+r2+r3) / 3;
		mg = (g1+g2+g3) / 3;
		mb = (b1+b2+b3) / 3;
		PIX();
	}
	
	{
		x = 0; y = bmp->h-1;
		GETCOLOR(1, x, y);
		GETCOLOR(2, x+1, y);
		GETCOLOR(3, x, y-1);
		mr = (r1+r2+r3) / 3;
		mg = (g1+g2+g3) / 3;
		mb = (b1+b2+b3) / 3;
		PIX();
	}
	
	{
		x = bmp->w-1; y = bmp->h-1;
		GETCOLOR(1, x, y);
		GETCOLOR(2, x-1, y);
		GETCOLOR(3, x, y-1);
		mr = (r1+r2+r3) / 3;
		mg = (g1+g2+g3) / 3;
		mb = (b1+b2+b3) / 3;
		PIX();
	}
	
	for(y = 1; y < bmp->h-1; y++)
	{
		for(x = 1; x < bmp->w-1; x++)
		{
			GETCOLOR(1, x, y);
			
			GETCOLOR(2, x, y-1);
			GETCOLOR(3, x+1, y);
			GETCOLOR(4, x, y+1);
			GETCOLOR(5, x-1, y);
			
			mr = (r1+r2+r3+r4+r5) / 5;
			mg = (g1+g2+g3+g4+g5) / 5;
			mb = (b1+b2+b3+b4+b5) / 5;
			PIX();
		}
	}
	
#undef GETCOLOR
#undef PIX
	
	if(destroy)
		_XDestroyBitmap(bmp);
	
	return tmp;
}

