#include "xgamebase.h"
#include "xintern.h"

void _XRect(_XBitmap *bmp, int x1, int y1, int x2, int y2, int color)
{
 if(x2 < x1)
 {
  x1 ^= x2;
  x2 ^= x1;
  x1 ^= x2;
 }

 if(y2 < y1)
 {
  y1 ^= y2;
  y2 ^= y1;
  y1 ^= y2;
 }

 hline(bmp, x1, y1, x2, color);

 if(y2 > y1)
  hline(bmp, x1, y2, x2, color);

 if(y2-1 >= y1+1)
 {
  vline(bmp, x1, y1+1, y2-1, color);
  if(x2 > x1)
   vline(bmp, x2, y1+1, y2-1, color);
 }
}

void _XFillRect(_XBitmap *bmp, int x1, int y1, int x2, int y2, int color)
{
 if(y1 > y2)
 {
  _XSwap(y1, y2);
 }

 if(x1 > x2)
 {
  _XSwap(x1, x2);
 }

 if(x1 < bmp->cl)
  x1 = bmp->cl;

 if(x2 >= bmp->cr)
  x2 = bmp->cr-1;

 if(x2 < x1)
  return;

 if(y1 < bmp->ct)
  y1 = bmp->ct;

 if(y2 >= bmp->cb)
  y2 = bmp->cb-1;

 if(y2 < y1)
  return;

 while(y1 <= y2)
 {
  hline(bmp, x1, y1, x2, color);
  y1++;
 }
}

void _XDottedRect(_XBitmap *bmp, int x1, int y1, int x2, int y2, int fg, int bg)
{
 int x = ((x1+y1) & 1) ? 1 : 0;
 int c;

 for(c=x1; c<=x2; c++)
 {
  put_pixel(bmp, c, y1, (((c+y1) & 1) == x) ? fg : bg);
  put_pixel(bmp, c, y2, (((c+y2) & 1) == x) ? fg : bg);
 }

 for(c=y1+1; c<y2; c++)
 {
  put_pixel(bmp, x1, c, (((c+x1) & 1) == x) ? fg : bg);
  put_pixel(bmp, x2, c, (((c+x2) & 1) == x) ? fg : bg);
 }
}

void _XDoCircle(_XBitmap *bmp, int x, int y, int radius, int d,
			   void (*proc)(_XBitmap*, int, int, int))
{
 int cx = 0;
 int cy = radius;
 int df = 1 - radius;
 int d_e = 3;
 int d_se = -2 * radius + 5;

 do
 {
  proc(bmp, x+cx, y+cy, d);

  if(cx) proc(bmp, x-cx, y+cy, d);
  if(cy) proc(bmp, x+cx, y-cy, d);
  if((cx) && (cy))
   proc(bmp, x-cx, y-cy, d);

  if(cx != cy)
  {
   proc(bmp, x+cy, y+cx, d);
   if(cx)  proc(bmp, x+cy, y-cx, d);
   if(cy)  proc(bmp, x-cy, y+cx, d);
   if(cx && cy)
	proc(bmp, x-cy, y-cx, d);
  }

  if(df < 0)
  {
   df += d_e;
   d_e += 2;
   d_se += 2;
  }
  else
  {
   df += d_se;
   d_e += 2;
   d_se += 4;
   cy--;
  }

  cx++;
 } while(cx <= cy);
}

void _XCircle(_XBitmap *bmp, int x, int y, int radius, int color)
{
 int sx, sy, dx, dy;

 sx = x-radius-1;
 sy = y-radius-1;
 dx = x+radius+1;
 dy = y+radius+1;

 if((sx >= bmp->cr) || (sy >= bmp->cb) || (dx < bmp->cl) || (dy < bmp->ct))
  return;

 _XDoCircle(bmp, x, y, radius, color, bmp->vtbl->put_pixel);
}

void _XFillCircle(_XBitmap *bmp, int x, int y, int radius, int color)
{
 int cx = 0;
 int cy = radius;
 int df = 1 - radius;
 int d_e = 3;
 int d_se = -2 * radius + 5;
 int sx, sy, dx, dy;

 sx = x-radius-1;
 sy = y-radius-1;
 dx = x+radius+1;
 dy = y+radius+1;

 if((sx >= bmp->cr) || (sy >= bmp->cb) || (dx < bmp->cl) || (dy < bmp->ct))
  return;

 do
 {
  hline(bmp, x-cy, y-cx, x+cy, color);

  if(cx) hline(bmp, x-cy, y+cx, x+cy, color);
  if(df < 0)
  {
   df += d_e;
   d_e += 2;
   d_se += 2;
  }
  else
  {
   if(cx != cy)
   {
	hline(bmp, x-cx, y-cy, x+cx, color);
	if(cy) hline(bmp, x-cx, y+cy, x+cx, color);
   }

   df += d_se;
   d_e += 2;
   d_se += 4;
   cy--;
  }

  cx++;

 } while(cx <= cy);
}

void _XDoLine(_XBitmap *bmp, int x1, int y1, int x2, int y2, int d,
			 void (*proc)(_XBitmap*, int, int, int))
{
 int dx = x2-x1;
 int dy = y2-y1;
 int i1, i2;
 int x, y;
 int dd;

 #define DO_LINE(pri_sign, pri_c, pri_cond, sec_sign, sec_c, sec_cond) \
 {                                                                  \
  if(d##pri_c == 0) {                                               \
   proc(bmp, x1, y1, d);                                            \
   return;                                                          \
  }                                                                 \
  i1 = 2 * d##sec_c;                                                \
  dd = i1 - (sec_sign (pri_sign d##pri_c));                         \
  i2 = dd - (sec_sign (pri_sign d##pri_c));                         \
  x = x1;                                                           \
  y = y1;                                                           \
  while(pri_c pri_cond pri_c##2) {                                  \
   proc(bmp, x, y, d);                                              \
   if(dd sec_cond 0) {                                              \
	sec_c sec_sign##= 1;                                            \
	dd += i2;                                                       \
   }                                                                \
   else                                                             \
	dd += i1;                                                       \
																	\
   pri_c pri_sign##= 1;                                             \
  }                                                                 \
 }

 if(dx >= 0)
 {
  if(dy >= 0)
  {
   if(dx >= dy)
   {
	DO_LINE(+, x, <=, +, y, >=);
   }
   else
   {
	DO_LINE(+, y, <=, +, x, >=);
   }
  }
  else
  {
   if(dx >= -dy)
   {
	DO_LINE(+, x, <=, -, y, <=);
   }
   else
   {
	DO_LINE(-, y, >=, +, x, >=);
   }
  }
 }
 else
 {
  if(dy >= 0)
  {
   if(-dx >= dy)
   {
	DO_LINE(-, x, >=, +, y, >=);
   }
   else
   {
	DO_LINE(+, y, <=, -, x, <=);
   }
  }
  else
  {
   if(-dx >= -dy)
   {
	DO_LINE(-, x, >=, -, y, <=);
   }
   else
   {
	DO_LINE(-, y, >=, -, x, <=);
   }
  }
 }
}

void _XLine(_XBitmap *bmp, int x1, int y1, int x2, int y2, int color)
{
 int sx, sy, dx, dy;

 if(x1 == x2)
 {
  vline(bmp, x1, y1, y2, color);
  return;
 }

 if(y1 == y2)
 {
  hline(bmp, x1, y1, x2, color);
  return;
 }

 sx = x1;
 sy = y1;
 dx = x2;
 dy = y2;

 if(sx > dx)
 {
  _XSwap(sx, dx);
 }

 if(sy > dy)
 {
  _XSwap(sy, dy);
 }

 if((sx >= bmp->cr) || (sy >= bmp->cb) || (dx < bmp->cl) || (dy < bmp->ct))
  return;

 _XDoLine(bmp, x1, y1, x2, y2, color, bmp->vtbl->put_pixel);
}
