#include "xsystem.h"
#include "xgfx.h"
#include "xinline.h"

static _XBitmap *create_inferior_mipmap(_XBitmap *bmp, int destroy)
{
 _XBitmap *tmp;
 int x, y, r, g, b, a, c;

 tmp = _XCreateBitmap(bmp->w/2, bmp->h/2, bmp->bpp);
 if(!tmp)
 {
  _XLogError("create_inferior_mipmap() (D3D) - Unable to create the tmp bitmap");
  return NULL;
 }

 for(y = 0; y < bmp->h; y += 2)
 {
  for(x = 0; x < bmp->w; x += 2)
  {
   c = get_pixel(bmp, x, y);
   r = _XGetREx(c, bmp->bpp);
   g = _XGetGEx(c, bmp->bpp);
   b = _XGetBEx(c, bmp->bpp);
   a = _XGetAEx(c, bmp->bpp);

   c = get_pixel(bmp, x+1, y);
   r += _XGetREx(c, bmp->bpp);
   g += _XGetGEx(c, bmp->bpp);
   b += _XGetBEx(c, bmp->bpp);
   a += _XGetAEx(c, bmp->bpp);

   c = get_pixel(bmp, x, y+1);
   r += _XGetREx(c, bmp->bpp);
   g += _XGetGEx(c, bmp->bpp);
   b += _XGetBEx(c, bmp->bpp);
   a += _XGetAEx(c, bmp->bpp);

   c = get_pixel(bmp, x+1, y+1);
   r += _XGetREx(c, bmp->bpp);
   g += _XGetGEx(c, bmp->bpp);
   b += _XGetBEx(c, bmp->bpp);
   a += _XGetAEx(c, bmp->bpp);

   r >>= 2; g >>= 2; b >>= 2; a >>= 2;
   put_pixel(tmp, x>>1, y>>1, _XRGBAEx(r,g,b,a,bmp->bpp));
  }
 }

 if(destroy) _XDestroyBitmap(bmp);

 return tmp;
}

_XBitmap *_XConvertBitmapSize(_XBitmap *bmp, int w, int h, int destroy)
{
	_XBitmap *tmp;
	
	if(w == 0) w = bmp->w;
	if(h == 0) h = bmp->h;
	
	tmp = _XCreateBitmap(w, h, bmp->bpp);
	if(!tmp)
	{
		_XLogError("_XConvertBitmapSize() - Unable to create the tmp bitmap");
		return NULL;
	}
	
	_XBlit(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h, w, h);
	
	if(destroy) _XDestroyBitmap(bmp);
	
	return tmp;
}

_XBitmap *_XConvertTextureBitmapSize(_XBitmap *bmp, int w, int h, int destroy)
{
	if(((w == 0) && (h == 0)) ||
		((w >= 4) && (h >= 4) &&
		(w == bmp->w/2) && (h == bmp->h/2)))
	{
		return create_inferior_mipmap(bmp, destroy);
	}

	return NULL;
}

_XBitmap *_XConvertBitmapBpp(_XBitmap *bmp, _XPalette pal, int bpp, int destroy)
{
	_XBitmap *tmp;
	
	tmp = _XCreateBitmap(bmp->w, bmp->h, bpp);
	if(!tmp)
	{
		_XLogError("_XConvertBitmapBpp() - Unable to create the tmp bitmap");
		return NULL;
	}
	
	if(pal) _XSelectPalette(pal);
	_XBlit(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h);
	if(pal) _XUnselectPalette();
	
	if(destroy) _XDestroyBitmap(bmp);
	
	return tmp;
}

_XBitmap *_XConvertBitmapMaskBpp(_XBitmap *bmp, _XPalette pal, int bpp, int destroy)
{
	_XBitmap *tmp;
	
	if(bpp <= 0)
		bpp = bmp->bpp;
	
	tmp = _XCreateBitmap(bmp->w, bmp->h, bpp);
	if(!tmp)
	{
		_XLogError("convert_bitmap_cmask_bpp_ex() - Unable to create the tmp bitmap");
		return NULL;
	}
	
	if(pal) _XSelectPalette(pal);
	_XBlitCMask(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h);
	if(pal) _XUnselectPalette();
	
	if(destroy) _XDestroyBitmap(bmp);
	
	return tmp;
}

