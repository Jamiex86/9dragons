#include "xgamebase.h"

static void rle_tga_read(_XVFile *rwops, unsigned char *b, int w)
{
 unsigned char value;
 int count;
 int c = 0;

 do
 {
  count = _XVFGet8(rwops);
  if(count & 0x80)
  {
   count = (count & 0x7F) + 1;
   c += count;
   value = _XVFGet8(rwops);
   while(count--)
	*(b++) = value;
  }
  else
  {
   count++;
   c += count;
   _XVFRead(rwops, b, count);
   b += count;
  }
 } while(c < w);
}

static void rle_tga_read24(_XVFile *rwops, unsigned char *b, int w)
{
 unsigned char value[4];
 int count;
 int c = 0;

 do
 {
  count = _XVFGet8(rwops);
  if(count & 0x80)
  {
   count = (count & 0x7F) + 1;
   c += count;
   _XVFRead(rwops, value, 3);
   while(count--)
   {
	b[_rgb_r_shift_24/8] = value[2];
	b[_rgb_g_shift_24/8] = value[1];
	b[_rgb_b_shift_24/8] = value[0];
	b += 3;
   }
  }
  else
  {
   count++;
   c += count;
   while(count--)
   {
	_XVFRead(rwops, value, 3);
	b[_rgb_r_shift_24/8] = value[2];
	b[_rgb_g_shift_24/8] = value[1];
	b[_rgb_b_shift_24/8] = value[0];
	b += 3;
   }
  }
 } while(c < w);
}

static void rle_tga_read32(_XVFile *rwops, unsigned char *b, int w)
{
 unsigned char value[4];
 int count;
 int c = 0;

 do
 {
  count = _XVFGet8(rwops);
  if(count & 0x80)
  {
   count = (count & 0x7F) + 1;
   c += count;
   _XVFRead(rwops, value, 4);
   while(count--)
   {
	b[_rgb_a_shift_32/8] = value[3];
	b[_rgb_r_shift_32/8] = value[2];
	b[_rgb_g_shift_32/8] = value[1];
	b[_rgb_b_shift_32/8] = value[0];	
	b += 4;
   }
  }
  else
  {
   count++;
   c += count;
   while(count--)
   {
	_XVFRead(rwops, value, 4);
	b[_rgb_a_shift_32/8] = value[3];
	b[_rgb_r_shift_24/8] = value[2];
	b[_rgb_g_shift_24/8] = value[1];
	b[_rgb_b_shift_24/8] = value[0];
	b += 4;
   }
  }
 } while(c < w);
}

static void rle_tga_read16(_XVFile *rwops, unsigned short *b, int w)
{
 unsigned int value;
 unsigned short color;
 int count;
 int c = 0;

 do
 {
  count = _XVFGet8(rwops);

  if(count & 0x80)
  {
   count = (count & 0x7F) + 1;
   c += count;
   value = _XVFGetLE16(rwops);
   color = ((((value >> 10) & 0x1F) << _rgb_r_shift_16) |
		   (((value >> 5) & 0x1F) << _rgb_g_shift_16) |
		   ((value & 0x1F) << _rgb_b_shift_16));
   while(count--)
	*(b++) = color;
  }
  else
  {
   count++;
   c += count;
   while(count--)
   {
	value = _XVFGetLE16(rwops);
	color = ((((value >> 10) & 0x1F) << _rgb_r_shift_16) |
			(((value >> 5) & 0x1F) << _rgb_g_shift_16) |
			((value & 0x1F) << _rgb_b_shift_16));
	*(b++) = color;
   }
  }
 } while(c < w);
}

bool _XVFIsTGA(_XVFile *rwops)
{
 int image_type = 0;

 _XVFGet8(rwops);
 _XVFGet8(rwops);
 image_type = _XVFGet8(rwops) & 7;

 if((image_type < 1) || (image_type > 3))
  return false;

 return true;
}

/*

  Cree un bitmap a partir de donnees au format TGA

*/
_XBitmap *_XVFReadTGA(_XVFile *rwops, _XRGB *pal)
{
 unsigned char image_id[256], image_palette[256][3], rgb[4];
 unsigned char id_length, palette_type, image_type;
 unsigned char bpp, descriptor_bits;
 short unsigned int image_width, image_height;
 unsigned int c, yc;
 int i, x, y;
 unsigned short *s;
 int compressed;
 U16 palette_colors;
 _XBitmap *bmp;

 id_length = _XVFGet8(rwops);
 palette_type = _XVFGet8(rwops);
 image_type = _XVFGet8(rwops);
 _XVFGetLE16(rwops);
 palette_colors = _XVFGetLE16(rwops);
 _XVFGet8(rwops);
 _XVFGetLE16(rwops);
 _XVFGetLE16(rwops);
 image_width = _XVFGetLE16(rwops);
 image_height = _XVFGetLE16(rwops);
 bpp = _XVFGet8(rwops);
 descriptor_bits = _XVFGet8(rwops);

 _XVFRead(rwops, &image_id, id_length);
 _XVFRead(rwops, &image_palette, palette_colors*3);

 compressed = (image_type & 8);
 image_type &= 7;

 if((image_type < 1) || (image_type > 3))
 {
  _XLogError("_XVFReadTGA() - Unknow TGA type !");
  return NULL;
 }

 switch(image_type)
 {
  case 1:
	   if((palette_type != 1) || (bpp != 8))
	   {
		_XLogError("_XVFReadTGA() - No palette or not 8 bpp in TGA type 1 !");
		return NULL;
	   }

	   if(pal)
	   {
		for(i = 0; i < palette_colors; i++)
		{
		 pal[i].r = image_palette[i][2];
		 pal[i].g = image_palette[i][1];
		 pal[i].b = image_palette[i][0];
		}
	   }
	   break;

  case 2:
	   if((palette_type == 0) && (bpp == 16))
	   {
		bpp = 1;
		if(pal)
		 _XGenerate332Palette(pal);
	   }
	   else if((palette_type == 0) && (bpp == 24 || bpp == 32))
	   {
		if(pal)
		 _XGenerate332Palette(pal);
	   }
	   else
	   {
		_XLogError("_XVFReadTGA() - Can't load TGA with this bpp [%d] !", bpp);
		return NULL;
	   }
	   break;

  case 3:
	   if((palette_type != 0) || (bpp != 8))
	   {
		_XLogError("_XVFReadTGA() - Palette or not 8bpp in TGA type 3 !");
		return NULL;
	   }

	   if(pal)
	   {
		for(i = 0; i < 256; i++)
		{
		 pal[i].r = i;
		 pal[i].g = i;
		 pal[i].b = i;
		}
	   }

	   break;

  default:
	   _XLogError("_XVFReadTGA() - Unknow TGA type [%d] !", image_type);
	   return NULL;
 }

 bmp = _XCreateBitmap(image_width, image_height, bpp);
 if(!bmp)
 {
  _XLogError("_XVFReadTGA() - Can't create bitmap !");
  return NULL;
 }

 for(y = image_height; y; y--)
 {
  yc = (descriptor_bits & 0x20) ? image_height-y : y-1;

  switch(image_type)
  {
   case 1:
   case 3:
		if(compressed)
		 rle_tga_read(rwops, bmp->line[yc], image_width);
		else
		 _XVFRead(rwops, bmp->line[yc], image_width);
		break;

   case 2:
		if(bpp == 32)
		{
		 if(compressed)
		 {
		  rle_tga_read32(rwops, bmp->line[yc], image_width);
		 }
		 else
		 {
		  for(x = 0; x < image_width; x++)
		  {
		   _XVFRead(rwops, rgb, 4);
		   bmp->line[yc][x*4+_rgb_a_shift_32/8] = rgb[3];
		   bmp->line[yc][x*4+_rgb_r_shift_32/8] = rgb[2];
		   bmp->line[yc][x*4+_rgb_g_shift_32/8] = rgb[1];
		   bmp->line[yc][x*4+_rgb_b_shift_32/8] = rgb[0];
		  }
		 }
		}
		else if(bpp == 24)
		{
		 if(compressed)
		 {
		  rle_tga_read24(rwops, bmp->line[yc], image_width);
		 }
		 else
		 {
		  for(x = 0; x < image_width; x++)
		  {
		   _XVFRead(rwops, rgb, 3);
		   bmp->line[yc][x*3+_rgb_r_shift_24/8] = rgb[2];
		   bmp->line[yc][x*3+_rgb_g_shift_24/8] = rgb[1];
		   bmp->line[yc][x*3+_rgb_b_shift_24/8] = rgb[0];
		  }
		 }
		}
		else
		{
		 if(compressed)
		 {
		  rle_tga_read16(rwops, (unsigned short *)bmp->line[yc], image_width);
		 }
		 else
		 {
		  s = (unsigned short *)bmp->line[yc];
		  for(x = 0; x < image_width; x++)
		  {
		   c = _XVFGetLE16(rwops);
		   s[x] = ((((c >> 10) & 0x1F) << _rgb_r_shift_16) |
				  (((c >> 5) & 0x1F) << _rgb_g_shift_16) |
				  ((c & 0x1F) << _rgb_b_shift_16));
		  }
		 }
		}
		break;
  }
 }

 return bmp;
}

int _XVFWriteTGA(_XVFile *rwops, _XBitmap *bmp, _XRGB *pal)
{
 unsigned char image_palette[256][3];
 int x, y, c, r, g, b;
 int depth;

 depth = bmp->bpp;
 if(depth == 32)
  depth = 24;

 _XVFPut8(rwops, 0);
 _XVFPut8(rwops, (depth == 8) ? 1 : 0);
 _XVFPut8(rwops, (depth == 8) ? 1 : 2);
 _XVFPutLE16(rwops, 0);
 _XVFPutLE16(rwops, (depth == 8) ? 256 : 0);
 _XVFPut8(rwops, (depth == 8) ? 24 : 0);
 _XVFPutLE16(rwops, 0);
 _XVFPutLE16(rwops, 0);
 _XVFPutLE16(rwops, bmp->w);
 _XVFPutLE16(rwops, bmp->h);
 _XVFPut8(rwops, depth);
 _XVFPut8(rwops, 0);

 if(depth == 8)
 {
  if(pal)
  {
   for(y = 0; y < 256; y++)
   {
	image_palette[y][2] = pal[y].r;
	image_palette[y][1] = pal[y].g;
	image_palette[y][0] = pal[y].b;
   }
  }
  else
  {
   for(y = 0; y < 256; y++)
   {
	image_palette[y][2] = 0;
	image_palette[y][1] = 0;
	image_palette[y][0] = 0;
   }
  }

  _XVFWrite(rwops, image_palette, 768);
 }

 switch(bmp->bpp)
 {
  case 8:
	   for(y = bmp->h; y; y--)
		for(x = 0; x < bmp->w; x++)
		 _XVFPut8(rwops, get_pixel(bmp, x, y-1));
	   break;

  case 16:
	   for(y = bmp->h; y; y--)
	   {
		for(x = 0; x < bmp->w; x++)
		{
		 c = get_pixel(bmp, x, y-1);
		 r = _XGetR16(c);
		 g = _XGetG16(c);
		 b = _XGetB16(c);
		 c = ((r<<7)&0x7C00) | ((g<<2)&0x3E0) | ((b>>3)&0x1F);
		 _XVFPutLE16(rwops, c);
		}
	   }
	   break;

  case 24:
	   for(y = bmp->h; y; y--)
	   {
		for(x = 0; x < bmp->w; x++)
		{
		 c = get_pixel(bmp, x, y-1);
		 _XVFPut8(rwops, _XGetB24(c));
		 _XVFPut8(rwops, _XGetG24(c));
		 _XVFPut8(rwops, _XGetR24(c));
		}
	   }
	   break;

  case 32:
	   for(y = bmp->h; y; y--)
	   {
		for(x = 0; x < bmp->w; x++)
		{
		 c = get_pixel(bmp, x, y-1);
		 _XVFPut8(rwops, _XGetB32(c));
		 _XVFPut8(rwops, _XGetG32(c));
		 _XVFPut8(rwops, _XGetR32(c));
		}
	   }
	   break;
 }

 return 1;
}
