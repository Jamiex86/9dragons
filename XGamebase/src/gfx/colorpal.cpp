#include "xgamebase.h"
#include <math.h>

static _XRGBMap rgb_tbl;
_XRGBMap *rgb_map = NULL;

/* Formats RGB(A) */
int _rgb_a_shift_12 = 12;
int _rgb_r_shift_12 = 8;
int _rgb_g_shift_12 = 4;
int _rgb_b_shift_12 = 0;
int _rgb_r_shift_15 = 10;
int _rgb_g_shift_15 = 5;
int _rgb_b_shift_15 = 0;
int _rgb_r_shift_16 = 11;
int _rgb_g_shift_16 = 5;
int _rgb_b_shift_16 = 0;
int _rgb_r_shift_24 = 16;
int _rgb_g_shift_24 = 8;
int _rgb_b_shift_24 = 0;
int _rgb_a_shift_32 = 24;
int _rgb_r_shift_32 = 16;
int _rgb_g_shift_32 = 8;
int _rgb_b_shift_32 = 0;

int _rgb_scale_5[32] =
{
 0, 8, 16, 24, 32, 41, 49, 57,
 65, 74, 82, 90, 98, 106, 115, 123,
 131, 139, 148, 156, 164, 172, 180, 189,
 197, 205, 213, 222, 230, 238, 246, 255
};

int _rgb_scale_6[64] =
{
 0, 4, 8, 12, 16, 20, 24, 28,
 32, 36, 40, 44, 48, 52, 56, 60,
 64, 68, 72, 76, 80, 85, 89, 93,
 97, 101, 105, 109, 113, 117, 121, 125,
 129, 133, 137, 141, 145, 149, 153, 157,
 161, 165, 170, 174, 178, 182, 186, 190,
 194, 198, 202, 206, 210, 214, 218, 222,
 226, 230, 234, 238, 242, 246, 250, 255
};

_XPalette sys_palette;

_XPalette default_palette =
{
 {   0,   0,   0 }, {   0,   0, 171 }, {   0, 171,   0 }, {   0, 171, 171 },
 { 171,   0,   0 }, { 171,   0, 171 }, { 171,  87,   0 }, { 171, 171, 171 },
 {  87,  87,  87 }, {  87,  87, 255 }, {  87, 255,  87 }, {  87, 255, 255 },
 { 255,  87,  87 }, { 255,  87, 255 }, { 255, 255,  87 }, { 255, 255, 255 },
 {   0,   0,   0 }, {  23,  23,  23 }, {  35,  35,  35 }, {  47,  47,  47 },
 {  59,  59,  59 }, {  71,  71,  71 }, {  83,  83,  83 }, {  99,  99,  99 },
 { 115, 115, 115 }, { 131, 131, 131 }, { 147, 147, 147 }, { 163, 163, 163 },
 { 183, 183, 183 }, { 203, 203, 203 }, { 227, 227, 227 }, { 255, 255, 255 },
 {   0,   0, 255 }, {  67,   0, 255 }, { 127,   0, 255 }, { 191,   0, 255 },
 { 255,   0, 255 }, { 255,   0, 191 }, { 255,   0, 127 }, { 255,   0,  67 },
 { 255,   0,   0 }, { 255,  67,   0 }, { 255, 127,   0 }, { 255, 191,   0 },
 { 255, 255,   0 }, { 191, 255,   0 }, { 127, 255,   0 }, {  67, 255,   0 },
 {   0, 255,   0 }, {   0, 255,  67 }, {   0, 255, 127 }, {   0, 255, 191 },
 {   0, 255, 255 }, {   0, 191, 255 }, {   0, 127, 255 }, {   0,  67, 255 },
 { 127, 127, 255 }, { 159, 127, 255 }, { 191, 127, 255 }, { 223, 127, 255 },
 { 255, 127, 255 }, { 255, 127, 223 }, { 255, 127, 191 }, { 255, 127, 159 },
 { 255, 127, 127 }, { 255, 159, 127 }, { 255, 191, 127 }, { 255, 223, 127 },
 { 255, 255, 127 }, { 223, 255, 127 }, { 191, 255, 127 }, { 159, 255, 127 },
 { 127, 255, 127 }, { 127, 255, 159 }, { 127, 255, 191 }, { 127, 255, 223 },
 { 127, 255, 255 }, { 127, 223, 255 }, { 127, 191, 255 }, { 127, 159, 255 },
 { 183, 183, 255 }, { 199, 183, 255 }, { 219, 183, 255 }, { 235, 183, 255 },
 { 255, 183, 255 }, { 255, 183, 235 }, { 255, 183, 219 }, { 255, 183, 199 },
 { 255, 183, 183 }, { 255, 199, 183 }, { 255, 219, 183 }, { 255, 235, 183 },
 { 255, 255, 183 }, { 235, 255, 183 }, { 219, 255, 183 }, { 199, 255, 183 },
 { 183, 255, 183 }, { 183, 255, 199 }, { 183, 255, 219 }, { 183, 255, 235 },
 { 183, 255, 255 }, { 183, 235, 255 }, { 183, 219, 255 }, { 183, 199, 255 },
 {   0,   0, 115 }, {  31,   0, 115 }, {  59,   0, 115 }, {  87,   0, 115 },
 { 115,   0, 115 }, { 115,   0,  87 }, { 115,   0,  59 }, { 115,   0,  31 },
 { 115,   0,   0 }, { 115,  31,   0 }, { 115,  59,   0 }, { 115,  87,   0 },
 { 115, 115,   0 }, {  87, 115,   0 }, {  59, 115,   0 }, {  31, 115,   0 },
 {   0, 115,   0 }, {   0, 115,  31 }, {   0, 115,  59 }, {   0, 115,  87 },
 {   0, 115, 115 }, {   0,  87, 115 }, {   0,  59, 115 }, {   0,  31, 115 },
 {  59,  59, 115 }, {  71,  59, 115 }, {  87,  59, 115 }, {  99,  59, 115 },
 { 115,  59, 115 }, { 115,  59,  99 }, { 115,  59,  87 }, { 115,  59,  71 },
 { 115,  59,  59 }, { 115,  71,  59 }, { 115,  87,  59 }, { 115,  99,  59 },
 { 115, 115,  59 }, {  99, 115,  59 }, {  87, 115,  59 }, {  71, 115,  59 },
 {  59, 115,  59 }, {  59, 115,  71 }, {  59, 115,  87 }, {  59, 115,  99 },
 {  59, 115, 115 }, {  59,  99, 115 }, {  59,  87, 115 }, {  59,  71, 115 },
 {  83,  83, 115 }, {  91,  83, 115 }, {  99,  83, 115 }, { 107,  83, 115 },
 { 115,  83, 115 }, { 115,  83, 107 }, { 115,  83,  99 }, { 115,  83,  91 },
 { 115,  83,  83 }, { 115,  91,  83 }, { 115,  99,  83 }, { 115, 107,  83 },
 { 115, 115,  83 }, { 107, 115,  83 }, {  99, 115,  83 }, {  91, 115,  83 },
 {  83, 115,  83 }, {  83, 115,  91 }, {  83, 115,  99 }, {  83, 115, 107 },
 {  83, 115, 115 }, {  83, 107, 115 }, {  83,  99, 115 }, {  83,  91, 115 },
 {   0,   0,  67 }, {  19,   0,  67 }, {  35,   0,  67 }, {  51,   0,  67 },
 {  67,   0,  67 }, {  67,   0,  51 }, {  67,   0,  35 }, {  67,   0,  19 },
 {  67,   0,   0 }, {  67,  19,   0 }, {  67,  35,   0 }, {  67,  51,   0 },
 {  67,  67,   0 }, {  51,  67,   0 }, {  35,  67,   0 }, {  19,  67,   0 },
 {   0,  67,   0 }, {   0,  67,  19 }, {   0,  67,  35 }, {   0,  67,  51 },
 {   0,  67,  67 }, {   0,  51,  67 }, {   0,  35,  67 }, {   0,  19,  67 },
 {  35,  35,  67 }, {  43,  35,  67 }, {  51,  35,  67 }, {  59,  35,  67 },
 {  67,  35,  67 }, {  67,  35,  59 }, {  67,  35,  51 }, {  67,  35,  43 },
 {  67,  35,  35 }, {  67,  43,  35 }, {  67,  51,  35 }, {  67,  59,  35 },
 {  67,  67,  35 }, {  59,  67,  35 }, {  51,  67,  35 }, {  43,  67,  35 },
 {  35,  67,  35 }, {  35,  67,  43 }, {  35,  67,  51 }, {  35,  67,  59 },
 {  35,  67,  67 }, {  35,  59,  67 }, {  35,  51,  67 }, {  35,  43,  67 },
 {  47,  47,  67 }, {  51,  47,  67 }, {  55,  47,  67 }, {  63,  47,  67 },
 {  67,  47,  67 }, {  67,  47,  63 }, {  67,  47,  55 }, {  67,  47,  51 },
 {  67,  47,  47 }, {  67,  51,  47 }, {  67,  55,  47 }, {  67,  63,  47 },
 {  67,  67,  47 }, {  63,  67,  47 }, {  55,  67,  47 }, {  51,  67,  47 },
 {  47,  67,  47 }, {  47,  67,  51 }, {  47,  67,  55 }, {  47,  67,  63 },
 {  47,  67,  67 }, {  47,  63,  67 }, {  47,  55,  67 }, {  47,  51,  67 },
 {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
 {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, { 255, 255, 255 },
};

int _XRGBEx(int r, int g, int b, int color_depth)
{
 switch(color_depth)
 {
  case 8:  return _XRGB8(r, g, b);
  case 12: return _XRGB12(r, g, b);
  case 15: return _XRGB15(r, g, b);
  case 16: return _XRGB16(r, g, b);
  case 24: return _XRGB24(r, g, b);
  case 32: return _XRGB32(r, g, b);
 }

 return 0;
}

int _XRGBAEx(int r, int g, int b, int a, int color_depth)
{
 switch(color_depth)
 {
  case 8:  return _XRGBA8(r, g, b, a);
  case 12: return _XRGBA12(r, g, b, a);
  case 15: return _XRGBA15(r, g, b, a);
  case 16: return _XRGBA16(r, g, b, a);
  case 24: return _XRGBA24(r, g, b, a);
  case 32: return _XRGBA32(r, g, b, a);
 }

 return 0;
}

int _XGetREx(int c, int color_depth)
{
 switch (color_depth)
 {
  case 8:  return _XGetR8(c);
  case 12: return _XGetR12(c);
  case 15: return _XGetR15(c);
  case 16: return _XGetR16(c);
  case 24: return _XGetR24(c);
  case 32: return _XGetR32(c);
 }

 return 0;
}

int _XGetGEx(int c, int color_depth)
{
 switch (color_depth)
 {
  case 8:  return _XGetG8(c);
  case 12: return _XGetG12(c);
  case 15: return _XGetG15(c);
  case 16: return _XGetG16(c);
  case 24: return _XGetG24(c);
  case 32: return _XGetG32(c);
 }

 return 0;
}

int _XGetBEx(int c, int color_depth)
{
 switch (color_depth)
 {
  case 8:  return _XGetB8(c);
  case 12: return _XGetB12(c);
  case 15: return _XGetB15(c);
  case 16: return _XGetB16(c);
  case 24: return _XGetB24(c);
  case 32: return _XGetB32(c);
 }

 return 0;
}

int _XGetAEx(int c, int color_depth)
{
 switch(color_depth)
 {
  case 8:  return _XGetA8(c);
  case 12: return _XGetA12(c);
  case 15: return _XGetA15(c);
  case 16: return _XGetA16(c);
  case 24: return _XGetA24(c);
  case 32: return _XGetA32(c);
 }

 return 0;
}

static unsigned col_diff[3*128];

static void bestfit_init(void)
{
 int i;

 for(i = 1; i < 64; i++)
 {
  int k = i * i;
  col_diff[0  +i] = col_diff[0  +128-i] = k * (59 * 59);
  col_diff[128+i] = col_diff[128+128-i] = k * (30 * 30);
  col_diff[256+i] = col_diff[256+128-i] = k * (11 * 11);
 }
}

int _XBestfitColor(_XPalette pal, int r, int g, int b)
{
 int i, coldiff, lowest;
 U32 bestfit;

 if(col_diff[1] == 0)
  bestfit_init();

 bestfit = 0;
 lowest = 2147483647;

 if((r == 255) && (g == 0) && (b == 255))
  i = 0;
 else
  i = 1;

 r >>= 2; g >>= 2; b >>= 2;

 while(i < _X_PAL_SIZE)
 {
  _XRGB *rgb = &pal[i];

  coldiff = (col_diff + 0)[((rgb->g>>2) - g) & 0x7F];

  if(coldiff < lowest)
  {
   coldiff += (col_diff + 128)[((rgb->r>>2) - r) & 0x7F];
   if(coldiff < lowest)
   {
    coldiff += (col_diff + 256)[((rgb->b>>2) - b) & 0x7F];
    if(coldiff < lowest)
    {
     bestfit = rgb - pal;
     if(coldiff == 0)
      return bestfit;
     lowest = coldiff;
    }
   }
  }
  i++;
 }

 return bestfit;
}

void _XSetColor(int index, _XRGB *p)
{
 sys_palette[index] = *p;
 _XSetPalette(sys_palette, index, index, 0);
}

void _XGetColor(int index, _XRGB *p)
{
 *p = sys_palette[index];
}

void _XSetPalette(_XPalette p, int from, int to, int vbl)
{
 int c, r, g, b;

 for(c = from; c <= to; c++)
 {
  r = p[c].r; g = p[c].g; b = p[c].b;
  sys_palette[c] = p[c];
 }
}

void _XSetPaletteMap(_XPalette p, int updmap)
{
 _XSetPalette(p, 0, _X_PAL_SIZE-1, 0);

 if(updmap)
 {
  _XCreateRGBTable(&rgb_tbl, p, NULL);
  rgb_map = &rgb_tbl;
 }
}


void _XGetPalette(_XPalette p, int from, int to)
{
 int c;

 for(c = from; c <= to; c++)
  _XGetColor(c,&p[c]);
}

void _XGetPaletteFromBuffer(U8 *src, _XPalette dst)
{
 int i;

 for(i=0; i < 768; i+=3)
 {
  dst[i/3].r = src[i];
  dst[i/3].g = src[i+1];
  dst[i/3].b = src[i+2];
 }
}


void _XCopyPalette(_XPalette src, _XPalette dst)
{
 int i;

 for(i=0; i < _X_PAL_SIZE; i++)
 {
  dst[i].r = src[i].r;
  dst[i].g = src[i].g;
  dst[i].b = src[i].b;
 }
}

static _XPalette prev_sys_palette;

void _XSelectPalette(_XPalette p)
{
 int c;
 
 for(c = 0; c < 256; c++)
 {

	 prev_sys_palette[c] = sys_palette[c];
  sys_palette[c] = p[c];
 }
}

void _XUnselectPalette()
{
 int c;
 for(c = 0; c < 256; c++)
 {
  sys_palette[c] = prev_sys_palette[c];
 }
}

void _XGenerate332Palette(_XPalette pal)
{
 int c;

 for(c = 0; c < 256; c++)
 {
  pal[c].r = ((c>>5)&7) * 255/7;
  pal[c].g = ((c>>2)&7) * 255/7;
  pal[c].b = (c&3) * 255/3;
 }

 pal[0].r = 255;
 pal[0].g = 0;
 pal[0].b = 255;

 pal[254].r = pal[254].g = pal[254].b = 0;
}

void _XCreateRGBTable(_XRGBMap *table, _XPalette p, void (*callback)(int pos))
{
 #define UNUSED 65535
 #define LAST 65532

 #define add(i)    (next[(i)] == UNUSED ? (next[(i)] = LAST, \
                   (first != LAST ? (next[last] = (i)) : (first = (i))), \
                   (last = (i))) : 0)

 #define add1(i)   (next[(i)] == UNUSED ? (next[(i)] = LAST, \
                   next[last] = (i), \
                   (last = (i))) : 0)

 #define dist(a1, a2, a3, b1, b2, b3) \
         (col_diff[ ((a2) - (b2)) & 0x7F] + \
         (col_diff + 128)[((a1) - (b1)) & 0x7F] + \
         (col_diff + 256)[((a3) - (b3)) & 0x7F])

 #define pos(r, g, b) (((r) / 2) * 32 * 32 + ((g) / 2) * 32 + ((b) / 2))

 #define depos(pal, r, g, b) ((b) = ((pal) & 31) * 2, \
                              (g) = (((pal) >> 5) & 31) * 2, \
                              (r) = (((pal) >> 10) & 31) * 2)

 #define better(r1, g1, b1, pal1) \
           (((int)dist((r1), (g1), (b1), \
             (pal1).r, (pal1).g, (pal1).b)) > (int)dist2)

 #define dopos(rp, gp, bp, ts) \
          if ((rp > -1 || r > 0) && (rp < 1 || r < 61) && \
          (gp > -1 || g > 0) && (gp < 1 || g < 61) && \
          (bp > -1 || b > 0) && (bp < 1 || b < 61)) \
          { \
           i = first + rp * 32 * 32 + gp * 32 + bp; \
           if (ts ? data[i] != val : !data[i]) \
           { \
            dist2 = (rp ? (col_diff+128)[(r+2*rp-pal[val].r) & 0x7F] : r2) + \
                    (gp ? (col_diff)[(g+2*gp-pal[val].g) & 0x7F] : g2) + \
                    (bp ? (col_diff+256)[(b+2*bp-pal[val].b) & 0x7F] : b2); \
            if (better((r+2*rp), (g+2*gp), (b+2*bp), pal[data[i]])) \
            { \
             data[i] = val; \
             add1 (i); \
            } \
           } \
          }

 int i, curr, r, g, b, val, r2, g2, b2, dist2;
 unsigned short next[32*32*32];
 unsigned char *data;
 int first = LAST;
 int last = LAST;
 int count = 0;
 int cbcount = 0;
 _XPalette pal;

 #define AVERAGE_COUNT   18000

 if(col_diff[1] == 0)
  bestfit_init();

 for(i = 0; i < _X_PAL_SIZE; i++)
 {
  pal[i].r = p[i].r>>2;
  pal[i].g = p[i].g>>2;
  pal[i].b = p[i].b>>2;
 }

 memset(next, 255, sizeof(next));
 memset(table->data, 0, sizeof(char)*32*32*32);

 data = (unsigned char *)table->data;

 for(i=1; i<256; i++)
 {
  curr = pos(pal[i].r, pal[i].g, pal[i].b);
  if(next[curr] == UNUSED)
  {
   data[curr] = i;
   add(curr);
  }
 }

 while(first != LAST)
 {
  depos(first, r, g, b);

  val = data[first];
  r2 = (col_diff+128)[((pal[val].r)-(r)) & 0x7F];
  g2 = (col_diff    )[((pal[val].g)-(g)) & 0x7F];
  b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];

  dopos( 0, 0, 1, 1);
  dopos( 0, 0,-1, 1);
  dopos( 1, 0, 0, 1);
  dopos(-1, 0, 0, 1);
  dopos( 0, 1, 0, 1);
  dopos( 0,-1, 0, 1);

  if((b > 0) && (data[first-1] == val))
  {
   b -= 2;
   first--;
   b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];

   dopos(-1, 0, 0, 0);
   dopos( 1, 0, 0, 0);
   dopos( 0,-1, 0, 0);
   dopos( 0, 1, 0, 0);

   first++;
  }

  i = first;
  first = next[first];
  next[i] = UNUSED;

  if(first != LAST)
  {
   depos(first, r, g, b);

   val = data[first];
   r2 = (col_diff+128)[((pal[val].r)-(r)) & 0x7F];
   g2 = (col_diff    )[((pal[val].g)-(g)) & 0x7F];
   b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];

   dopos( 0, 0, 1, 1);
   dopos( 0, 0,-1, 1);
   dopos( 1, 0, 0, 1);
   dopos(-1, 0, 0, 1);
   dopos( 0, 1, 0, 1);
   dopos( 0,-1, 0, 1);

   if((b < 61) && (data[first + 1] == val))
   {
    b += 2;
    first++;
    b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7f];

    dopos(-1, 0, 0, 0);
    dopos( 1, 0, 0, 0);
    dopos( 0,-1, 0, 0);
    dopos( 0, 1, 0, 0);

    first--;
   }

   i = first;
   first = next[first];
   next[i] = UNUSED;
  }

  count++;
  if(count == (cbcount+1)*AVERAGE_COUNT/256)
  {
   if(cbcount < 256)
   {
    if(callback) callback(cbcount);
    cbcount++;
   }
  }
 }

 if(callback)
  while(cbcount < 256)
   callback(cbcount++);
}

void _XDrawPalette(_XBitmap *bmp, int xd, int yd, int w, int h)
{
 int x = xd, y = yd,
     bw = w/32,
     bh = h/8,
     cl = 0, i;

 for(i = 0; i < 256; i++)
 {
  _XFillRect(bmp, x, y, x+bw, y+bh, _XRGBEx(sys_palette[i].r,sys_palette[i].g,sys_palette[i].b,bmp->bpp));
  x += bw;

  if( x > w)
  {
   y += bh;
   x = xd;
  }

  cl++;
 }
}

void _XHSVToRGB(float h, float s, float v, int *r, int *g, int *b)
{
 float f, x, y, z;
 int i;

 v *= 255.0;

 if(s == 0.0)
  *r = *g = *b = (int)v;
 else
 {
  while(h < 0)
   h += 360;

  h = fmod(h, 360) / 60.0;
  i = (int)h;
  f = h - i;
  x = v * (1.0 - s);
  y = v * (1.0 - (s * f));
  z = v * (1.0 - (s * (1.0 - f)));

  switch(i)
  {
   case 0: *r = (int)v; *g = (int)z; *b = (int)x; break;
   case 1: *r = (int)y; *g = (int)v; *b = (int)x; break;
   case 2: *r = (int)x; *g = (int)v; *b = (int)z; break;
   case 3: *r = (int)x; *g = (int)y; *b = (int)v; break;
   case 4: *r = (int)z; *g = (int)x; *b = (int)v; break;
   case 5: *r = (int)v; *g = (int)x; *b = (int)y; break;
  }
 }
}

void _XRGBToHSV(int r, int g, int b, float *h, float *s, float *v)
{
 float min, max, delta, rc, gc, bc;

 rc = (float)r / 255.0;
 gc = (float)g / 255.0;
 bc = (float)b / 255.0;
 max = MAX(rc, MAX(gc, bc));
 min = MIN(rc, MIN(gc, bc));
 delta = max - min;
 *v = max;

 if(max != 0.0)
  *s = delta / max;
 else
  *s = 0.0;

 if(*s == 0.0)
  *h = 0.0;
 else
 {
  if(rc == max)
   *h = (gc - bc) / delta;
  else if(gc == max)
   *h = 2 + (bc - rc) / delta;
  else if(bc == max)
   *h = 4 + (rc - gc) / delta;

  *h *= 60.0;
  if(*h < 0)
   *h += 360.0;
 }
}

void _XCreatePaletteShading(_XPalette pal,
                            int ib, int rb, int gb, int bb,
                            int ie, int re, int ge, int be)
{
 int i, n;
 float rr, gr, br;
 _XRGB c;

 if(ib > ie)
 {
  _XSwap(ib, ie);
  _XSwap(rb, re);
  _XSwap(gb, ge);
  _XSwap(bb, be);
 }

 n = ie-ib;

 c.r = rb;
 c.g = gb;
 c.b = bb;
 pal[ib] = c;

 c.r = re;
 c.g = ge;
 c.b = be;
 pal[ie] = c;

 rr = (re-rb)/(float)n;
 gr = (ge-gb)/(float)n;
 br = (be-bb)/(float)n;

 for(i = 1; i < n; i++)
 {
  c.r = rb+(rr*i);
  c.g = gb+(gr*i);
  c.b = bb+(br*i);
  pal[ib+i] = c;
 }
}

static int push_bpp_shift, push_r_shift, push_g_shift, push_b_shift, push_a_shift;

void _XPushRGBShift(int bpp)
{
 push_bpp_shift = bpp;

 switch(bpp)
 {
  case 12:
       push_r_shift = _rgb_r_shift_12;
       push_g_shift = _rgb_g_shift_12;
       push_b_shift = _rgb_b_shift_12;
       push_a_shift = _rgb_a_shift_12;
       break;
       
  case 15:
       push_r_shift = _rgb_r_shift_15;
       push_g_shift = _rgb_g_shift_15;
       push_b_shift = _rgb_b_shift_15;
       push_a_shift = 0;
       break;

  case 16:
       push_r_shift = _rgb_r_shift_16;
       push_g_shift = _rgb_g_shift_16;
       push_b_shift = _rgb_b_shift_16;
       push_a_shift = 0;
       break;

  case 24:
       push_r_shift = _rgb_r_shift_24;
       push_g_shift = _rgb_g_shift_24;
       push_b_shift = _rgb_b_shift_24;
       push_a_shift = 0;
       break;

  case 32:
       push_r_shift = _rgb_r_shift_32;
       push_g_shift = _rgb_g_shift_32;
       push_b_shift = _rgb_b_shift_32;
       push_a_shift = _rgb_a_shift_32;
       break;

  default:
       return;
 }
}

void _XPopRGBShift(void)
{
 switch(push_bpp_shift)
 {
  case 12:
       _rgb_r_shift_12 = push_r_shift;
       _rgb_g_shift_12 = push_g_shift;
       _rgb_b_shift_12 = push_b_shift;
	   _rgb_a_shift_12 = push_a_shift;
       break;

  case 15:
       _rgb_r_shift_15 = push_r_shift;
       _rgb_g_shift_15 = push_g_shift;
       _rgb_b_shift_15 = push_b_shift;
       break;

  case 16:
       _rgb_r_shift_16 = push_r_shift;
       _rgb_g_shift_16 = push_g_shift;
       _rgb_b_shift_16 = push_b_shift;
       break;

  case 24:
       _rgb_r_shift_24 = push_r_shift;
       _rgb_g_shift_24 = push_g_shift;
       _rgb_b_shift_24 = push_b_shift;
       break;

  case 32:
       _rgb_r_shift_32 = push_r_shift;
       _rgb_g_shift_32 = push_g_shift;
       _rgb_b_shift_32 = push_b_shift;
       _rgb_a_shift_32 = push_a_shift;
       break;

  default: 
       return;
 }
}

void _XSetRGBShift(int bpp, int rs, int gs, int bs, int as)
{
 switch(bpp)
 {
  case 12:
       _rgb_r_shift_12 = rs;
       _rgb_g_shift_12 = gs;
       _rgb_b_shift_12 = bs;
	   _rgb_a_shift_12 = as;
       break;

  case 15:
       _rgb_r_shift_15 = rs;
       _rgb_g_shift_15 = gs;
       _rgb_b_shift_15 = bs;
       break;

  case 16:
       _rgb_r_shift_16 = rs;
       _rgb_g_shift_16 = gs;
       _rgb_b_shift_16 = bs;
       break;

  case 24:
       _rgb_r_shift_24 = rs;
       _rgb_g_shift_24 = gs;
       _rgb_b_shift_24 = bs;
       break;

  case 32:
       _rgb_r_shift_32 = rs;
       _rgb_g_shift_32 = gs;
       _rgb_b_shift_32 = bs;
       _rgb_a_shift_32 = as;
       break;

  default:
       break;
 }
} 

void _XGetRGBShift(int bpp, int *rs, int *gs, int *bs, int *as)
{
 *rs = *gs = *bs = *as = 0;

 switch(bpp)
 {
  case 12:
       *rs = _rgb_r_shift_12;
       *gs = _rgb_g_shift_12;
       *bs = _rgb_b_shift_12;
	   *as = _rgb_a_shift_12;
       break;

  case 15:
       *rs = _rgb_r_shift_15;
       *gs = _rgb_g_shift_15;
       *bs = _rgb_b_shift_15;
       break;

  case 16:
       *rs = _rgb_r_shift_16;
       *gs = _rgb_g_shift_16;
       *bs = _rgb_b_shift_16;
       break;

  case 24:
       *rs = _rgb_r_shift_24;
       *gs = _rgb_g_shift_24;
       *bs = _rgb_b_shift_24;
       break;

  case 32:
       *rs = _rgb_r_shift_32;
       *gs = _rgb_g_shift_32;
       *bs = _rgb_b_shift_32;
       *as = _rgb_a_shift_32;
       break;

  default:
       break;
 }
} 
