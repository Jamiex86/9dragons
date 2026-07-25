#include "xgamebase.h"

/*
  Informations pour le stretching
*/
static struct
{
 int sx;
 int dx;
 int i1;
 int i2;
 int dd;
 int dw;  int sinc;
 U8 **sline;
 U8 **dline;
} stretch_info;

static int _stretch_color = 0;

/*

  Fonctions de stretching de lignes

*/
static void _stretch_line8(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U8 *s = sptr + stretch_info.sx;
 U8 *d = dptr + stretch_info.dx;
 U8 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  *d = *s;

  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_line16(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*)sptr + stretch_info.sx;
 U16 *d = (U16*)dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  *d = *s;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_line24(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U8 *s = sptr + stretch_info.sx;
 U8 *d = dptr + stretch_info.dx;
 U8 *dend = d + stretch_info.dw;

 for(; d < dend; d+=3, s += stretch_info.sinc)
 {
  _XWrite24(d, _XRead24(s));

  if(dd >= 0)
   s+=3, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_line32(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U32 *s = (U32*)sptr + stretch_info.sx;
 U32 *d = (U32*)dptr + stretch_info.dx;
 U32 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  *d = *s;

  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

/*

  Fonctions de stretching de lignes maskee

*/
static void _stretch_masked_line8(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U8 *s = sptr + stretch_info.sx;
 U8 *d = dptr + stretch_info.dx;
 U8 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color)
   *d = color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line12(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*)sptr + stretch_info.sx;
 U16 *d = (U16*)dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_12)
   *d = color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line15(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*)sptr + stretch_info.sx;
 U16 *d = (U16*)dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_15)
   *d = color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line16(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*)sptr + stretch_info.sx;
 U16 *d = (U16*)dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_16)
   *d = color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line24(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U8 *s = (U8*)sptr + stretch_info.sx;
 U8 *d = (U8*)dptr + stretch_info.dx;
 U8 *dend = d + stretch_info.dw;
 int color;

 for(; d < dend; d+=3, s += stretch_info.sinc)
 {
  color = _XRead24(s);

  if(color != MASK_COLOR_24)
   _XWrite24(d, color);

  if(dd >= 0)
   s+=3, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line32(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U32 *s = (U32*)sptr + stretch_info.sx;
 U32 *d = (U32*)dptr + stretch_info.dx;
 U32 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_32)
   *d = color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

/*

  Fonctions de stretching de lignes maskee qui dessine une couleur a la place
  de la source

*/
static void _stretch_masked_line_color8(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U8 *s = sptr + stretch_info.sx;
 U8 *d = dptr + stretch_info.dx;
 U8 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color)
   *d = _stretch_color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line_color12(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*) sptr + stretch_info.sx;
 U16 *d = (U16*) dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_12)
   *d = _stretch_color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line_color15(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*) sptr + stretch_info.sx;
 U16 *d = (U16*) dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_15)
   *d = _stretch_color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line_color16(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U16 *s = (U16*) sptr + stretch_info.sx;
 U16 *d = (U16*) dptr + stretch_info.dx;
 U16 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_16)
   *d = _stretch_color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line_color24(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U8 *s = (U8*)sptr + stretch_info.sx;
 U8 *d = (U8*)dptr + stretch_info.dx;
 U8 *dend = d + stretch_info.dw;

 for(; d < dend; d+=3, s += stretch_info.sinc)
 {
  if(_XRead24(s) != MASK_COLOR_24)
   _XWrite24(d, _stretch_color);

  if(dd >= 0)
   s += 3, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

static void _stretch_masked_line_color32(U8* dptr, U8* sptr)
{
 int dd = stretch_info.dd;
 U32 *s = (U32*)sptr + stretch_info.sx;
 U32 *d = (U32*)dptr + stretch_info.dx;
 U32 *dend = d + stretch_info.dw;

 for(; d < dend; d++, s += stretch_info.sinc)
 {
  int color = *s;

  if(color != MASK_COLOR_32)
   *d = _stretch_color;
  if(dd >= 0)
   s++, dd += stretch_info.i2;
  else
   dd += stretch_info.i1;
 }
}

/*

  Fonction de stretching global

*/
static void stretch_blit(_XBitmap* src, _XBitmap* dst,
						 int sx, int sy, int sw, int sh,
						 int dx, int dy, int _dw, int dh,
						 int masked, int color)
{
 int x, y;
 int i1, i2, dd;
 int xinc, yinc;
 int dxbeg, dxend;
 int dybeg, dyend;
 void (*stretch_line)(U8 *dptr, U8 *sptr);

 if((sw <= 0) || (sh <= 0) || (_dw <= 0) || (dh <= 0))
  return;

 dybeg = ((dy > dst->ct) ? dy : dst->ct);
 dyend = (((dy + dh) < dst->cb) ? (dy + dh) : dst->cb);

 if(dybeg >= dyend)
  return;

 dxbeg = ((dx > dst->cl) ? dx : dst->cl);
 dxend = (((dx + _dw) < dst->cr) ? (dx + _dw) : dst->cr);

 if(dxbeg >= dxend)
  return;

 sw--; sh--;
 _dw--; dh--;

 if(_dw == 0)
  xinc = 0;
 else
 {
  xinc = sw / _dw;
  sw %= _dw;
 }

 if(dh == 0)
  yinc = 0;
 else
 {
  yinc = sh / dh;
  sh %= dh;
 }

 i2 = (dd = (i1 = 2 * sw) - _dw) - _dw;
 for(x = dx, y = sx; x < dxbeg; x++, y += xinc)
 {
  if(dd >= 0)
   y++, dd += i2;
  else
   dd += i1;
 }

 stretch_info.dx = x;
 stretch_info.sx = y;
 stretch_info.i1 = i1;
 stretch_info.i2 = i2;
 stretch_info.dd = dd;
 stretch_info.dw = dxend - dxbeg;
 stretch_info.sinc = xinc;
 stretch_info.sline = src->line;
 stretch_info.dline = dst->line;

 if(color >= 0)
 {
  _stretch_color = color;
  if(dst->bpp == 8)
   stretch_line = _stretch_masked_line_color8;
  else if(dst->bpp == 12)
   stretch_line = _stretch_masked_line_color12;
  else if(dst->bpp == 15)
   stretch_line = _stretch_masked_line_color15;
  else if(dst->bpp == 16)
   stretch_line = _stretch_masked_line_color16;
  else if(dst->bpp == 24)
  {
   stretch_line = _stretch_masked_line_color24;
   stretch_info.sx *= 3;
   stretch_info.dx *= 3;
   stretch_info.dw *= 3;
   stretch_info.sinc *= 3;
  }
  else if(dst->bpp == 32)
   stretch_line = _stretch_masked_line_color32;
  else
   return;
 }
 else if(masked)
 {
  if(dst->bpp == 8)
   stretch_line = _stretch_masked_line8;
  else if(dst->bpp == 12)
   stretch_line = _stretch_masked_line12;
  else if(dst->bpp == 15)
   stretch_line = _stretch_masked_line15;
  else if(dst->bpp == 16)
   stretch_line = _stretch_masked_line16;
  else if(dst->bpp == 24)
  {
   stretch_line = _stretch_masked_line24;
   stretch_info.sx *= 3;
   stretch_info.dx *= 3;
   stretch_info.dw *= 3;
   stretch_info.sinc *= 3;
  }
  else if(dst->bpp == 32)
   stretch_line = _stretch_masked_line32;
  else
   return;
 }
 else
 {
  if(dst->bpp == 8)
   stretch_line = _stretch_line8;
  else if(dst->bpp == 16 || dst->bpp == 15 || dst->bpp == 12)
   stretch_line = _stretch_line16;
  else if(dst->bpp == 24)
  {
   stretch_line = _stretch_line24;
   stretch_info.sx *= 3;
   stretch_info.dx *= 3;
   stretch_info.dw *= 3;
   stretch_info.sinc *= 3;
  }
  else if(dst->bpp == 32)
   stretch_line = _stretch_line32;
  else
   return;
 }

 i2 = (dd = (i1 = 2 * sh) - dh) - dh;
 for(x = dy, y = sy; x < dybeg; x++, y += yinc)
 {
  if(dd >= 0)
   y++, dd += i2;
  else
   dd += i1;
 }

 for(; x < dyend; x++, y += yinc)
 {
  (*stretch_line)(stretch_info.dline[x], stretch_info.sline[y]);

  if(dd >= 0)
   y++, dd += i2;
  else
   dd += i1;
 }
}


/*

  Blitting entre 2 bitmaps de format different

*/
static void blit_conv(_XBitmap *src, _XBitmap *dst, int sx, int sy,
					  int dx, int dy, int w, int h)
{
 int x, y, c;
 U8 r, g, b, a;

 for(y = 0; y < h; y++)
 {
  for(x = 0; x < w; x++)
  {
   c = get_pixel(src, x+sx, y+sy);
   r = _XGetREx(c, src->bpp);
   g = _XGetGEx(c, src->bpp);
   b = _XGetBEx(c, src->bpp);
   a = _XGetAEx(c, src->bpp);
   put_pixel(dst, x+dx, y+dy, _XRGBAEx(r,g,b,a,dst->bpp));      
  }
 }
}

/*

  Blitting maske entre 2 bitmaps de format different

*/
static void blit_mask_conv(_XBitmap *src, _XBitmap *dst, int sx, int sy,
					  int dx, int dy, int w, int h)
{
 int x, y, c;
 U8 r, g, b, a;

 for(y = 0; y < h; y++)
 {
  for(x = 0; x < w; x++)
  {
   c = get_pixel(src, x+sx, y+sy);
   if(c != BMP_MASK(src))
   {
    r = _XGetREx(c, src->bpp);
	g = _XGetGEx(c, src->bpp);
	b = _XGetBEx(c, src->bpp);
	a = _XGetAEx(c, src->bpp);
	put_pixel(dst, x+dx, y+dy, _XRGBAEx(r,g,b,a,dst->bpp));
   }
  }
 }
}

/* Helper pour clipper un rectangle de _XBlitting */
#define BLIT_CLIP()                                                         \
{                                                                           \
 if((sx >= src->w) || (sy >= src->h) || (dx >= dst->cr) || (dy >= dst->cb)) \
  return;                                                                   \
																			\
 if(sx < 0)                                                                 \
 {                                                                          \
  w += sx;                                                                  \
  dx -= sx;                                                                 \
  sx = 0;                                                                   \
 }                                                                          \
																			\
 if(sy < 0)                                                                 \
 {                                                                          \
  h += sy;                                                                  \
  dy -= sy;                                                                 \
  sy = 0;                                                                   \
 }                                                                          \
																			\
 if(sx+w > src->w)                                                          \
  w = src->w - sx;                                                          \
																			\
 if(sy+h > src->h)                                                          \
  h = src->h - sy;                                                          \
																			\
 if(dx < dst->cl)                                                           \
 {                                                                          \
  dx -= dst->cl;                                                            \
  w += dx;                                                                  \
  sx -= dx;                                                                 \
  dx = dst->cl;                                                             \
 }                                                                          \
																			\
 if(dy < dst->ct)                                                           \
 {                                                                          \
  dy -= dst->ct;                                                            \
  h += dy;                                                                  \
  sy -= dy;                                                                 \
  dy = dst->ct;                                                             \
 }                                                                          \
																			\
 if(dx+w > dst->cr)                                                         \
  w = dst->cr - dx;                                                         \
																			\
 if(dy+h > dst->cb)                                                         \
  h = dst->cb - dy;                                                         \
																			\
 if((w <= 0) || (h <= 0))                                                   \
  return;                                                                   \
}


/*

  Blitting d'un bitmap a l'autre

*/
void _XBlit(_XBitmap *src, _XBitmap *dst, int sx, int sy, int dx, int dy,
		  int w, int h)
{
 BLIT_CLIP();

 if(src->bpp != dst->bpp)
 {
  blit_conv(src, dst, sx, sy, dx, dy, w, h);
 }
 else
 {
  dst->vtbl->blit(src, dst, sx, sy, dx, dy, w, h);
 }
}

/*

  Blitting maske d'un bitmap a l'autre

*/
void _XBlitMask(_XBitmap *src, _XBitmap *dst, int sx, int sy, int dx, int dy,
		  int w, int h)
{
 BLIT_CLIP();

 if(src->bpp != dst->bpp)
 {
  blit_mask_conv(src, dst, sx, sy, dx, dy, w, h);
 }
 /* Vers un bitmap MEMOIRE */
 else
 {
  dst->vtbl->blit_mask(src, dst, sx, sy, dx, dy, w, h);
 }
}

/*

  Blitting mask entre 2 bitmaps de format different en convertisant la
  couleur de MASK

*/
void _XBlitCMask(_XBitmap *src, _XBitmap *dst, int sx, int sy,
				int dx, int dy, int w, int h)
{
 int x, y, c;
 U8 r, g, b, a;
 BLIT_CLIP();

 for(y = 0; y < h; y++)
 {
  for(x = w - 1; x >= 0; x--)
  {
   c = get_pixel(src, x+sx, y+sy);
   if(c == BMP_MASK(src))
   {
	put_pixel(dst, x+dx, y+dy, BMP_MASK(dst));
   }
   else
   {
    r = _XGetREx(c, src->bpp);
	g = _XGetGEx(c, src->bpp);
	b = _XGetBEx(c, src->bpp);
	a = _XGetAEx(c, src->bpp);
	put_pixel(dst, x+dx, y+dy, _XRGBAEx(r,g,b,a,dst->bpp));
   }
  }
 }
}

/*

  Blitting avec stretching

*/
void _XBlit(_XBitmap *s, _XBitmap *d, int x_s, int y_s, int x_d, int y_d,
				 int w, int h, int w_d, int h_d)
{
 if(w == w_d && h == h_d)
  _XBlit(s, d, x_s, y_s, x_d, y_d, w, h);
 else
  stretch_blit(s, d, x_s, y_s, w, h, x_d, y_d, w_d, h_d, 0, -1);
}

/*

  Blitting avec stretching et passage de la couleur de mask

*/
void _XBlitMask(_XBitmap *s, _XBitmap *d, int x_s, int y_s, int x_d, int y_d,
					   int w, int h, int w_d, int h_d)
{
 if(w == w_d && h == h_d)
  _XBlitMask(s, d, x_s, y_s, x_d, y_d, w, h);
 else
  stretch_blit(s, d, x_s, y_s, w, h, x_d, y_d, w_d, h_d, 1, -1);
}
