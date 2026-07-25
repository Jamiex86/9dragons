
#include "xgamebase.h"

typedef struct LZW_STRING
{
 short base;
 char newc;
 short length;
} LZW_STRING;

static _XVFile *f;
static int empty_str, curr_bit_size, bit_overflow;
static int bit_pos, data_pos, data_len, entire, code;
static int cc, string_length, i, bit_size;
static unsigned char string[4096];
static LZW_STRING str[4096];
static _XBitmap *gif_tmp_bmp;
static int image_x, image_y, image_w, image_h, x, y;
static int interlace;

static void clear_table(void)
{
 empty_str = cc + 2;
 curr_bit_size = bit_size + 1;
 bit_overflow = 0;
}

static void get_code(void)
{
 if(bit_pos + curr_bit_size > 8)
 {
  if(data_pos >= data_len)
  {
   data_len = _XVFGet8(f);
   data_pos = 0;
  }
  entire = (_XVFGet8(f) << 8) + entire;
  data_pos ++;
 }

 if(bit_pos + curr_bit_size > 16)
 {
  if(data_pos >= data_len)
  {
   data_len = _XVFGet8(f);
   data_pos = 0;
  }
  entire = (_XVFGet8(f) << 16) + entire;
  data_pos ++;
 }
 code = (entire >> bit_pos) & ((1 << curr_bit_size) - 1);
 if(bit_pos + curr_bit_size > 8)
  entire >>= 8;
 if(bit_pos + curr_bit_size > 16)
  entire >>= 8;
 bit_pos = (bit_pos + curr_bit_size) % 8;
 if(bit_pos == 0)
 {
  if(data_pos >= data_len)
  {
   data_len = _XVFGet8(f);
   data_pos = 0;
  }
  entire = _XVFGet8(f);
  data_pos ++;
 }
}

static void get_string(int num)
{
 if(num < cc)
 {
  string_length = 1;
  string[0] = str[num].newc;
 }
 else
 {
  i = str[num].length;
  string_length = i;
  while(i > 0)
  {
   i--;
   string[i] = str[num].newc;
   num = str[num].base;
  }
 }
}

static void output_string(void)
{
 for(i = 0; i < string_length; i ++)
 {
  put_pixel(gif_tmp_bmp, x, y, string[i]);
  x++;
  if(x >= image_x + image_w)
  {
   x = image_x;
   y += interlace;
   if(interlace)
   {
	if(y >= image_y + image_h)
	{
	 if(interlace == 8 && (y - image_y) % 8 == 0)
	 {
	  interlace = 8;
	  y = image_y + 4;
	 }
	 else if(interlace == 8  && (y - image_y) % 8 == 4)
	 {
	  interlace = 4;
	  y = image_y + 2;
	 }
	 else if(interlace == 4)
	 {
	  interlace = 2;
	  y = image_y + 1;
	 }
	}
   }
  }
 }
}

bool _XVFIsGIF(_XVFile *fp)
{
 int i = _XVFGetBE16(fp) << 8;
 i += _XVFGet8(fp);

 if(i != 0x474946)
  return false;

 return true;
}

_XBitmap *_XVFReadGIF(_XVFile *file, _XRGB *pal)
{
 int width, height, depth;
 int old;
 _XBitmap *bmp2;

 f = file;

 i = _XVFGetBE16(f) << 8;
 i += _XVFGet8(f);
 if(i != 0x474946) /* is it really a GIF? */
 {
  _XLogError("_XVFReadGIF() - Invalid gif !");
  return NULL;
 }
 _XVFSeek(f, 3);

 width = _XVFGetLE16(f);
 height = _XVFGetLE16(f);

 gif_tmp_bmp = _XCreateBitmap(width, height, 8);
 if(gif_tmp_bmp == NULL)
 {
  _XLogError("_XVFReadGIF() - Can't create bitmap !");
  return NULL;
 }
 clear(gif_tmp_bmp);

 i = _XVFGet8(f);
 if(i & 128)
  depth = (i & 7) + 1;
 else
  depth = 0;

 _XVFSeek(f, 2);

 if(pal && depth)
 {
  for(i = 0; i < (1 << depth); i ++)
  {
   pal[i].r = _XVFGet8(f);
   pal[i].g = _XVFGet8(f);
   pal[i].b = _XVFGet8(f);
  }
 }
 else if(depth)
 {
  _XVFSeek(f, (1 << depth) * 3);
 }

 do
 {
  i = _XVFGet8(f);
  switch(i)
  {
   case 0x2C:
		image_x = _XVFGetLE16(f);
		image_y = _XVFGetLE16(f);
		image_w = _XVFGetLE16(f);
		image_h = _XVFGetLE16(f);

		i = _XVFGet8(f);
		if(i & 64)
		 interlace = 8;
		else
		 interlace = 1;

		if(i & 128)
		{
		 depth = (i & 7) + 1;
		 if(pal)
		 { 
		  for(i = 0; i < (1 << depth); i ++)
		  {
		   pal[i].r = _XVFGet8(f);
		   pal[i].g = _XVFGet8(f);
		   pal[i].b = _XVFGet8(f);
		  }
		 }
		 else
		 {
		  _XVFSeek(f, (1 << depth) * 3);
		 }
		}

		bit_size = _XVFGet8(f);
		cc = 1 << bit_size;

		for(i = 0; i < cc; i ++)
		{
		 str[i].base = -1;
		 str[i].newc = i;
		 str[i].length = 1;
		}

		bit_pos = 0;
		data_len = _XVFGet8(f);
		data_pos = 0;
		entire = _XVFGet8(f);
		data_pos ++;
		string_length = 0;
		x = image_x;
		y = image_y;

		clear_table();
		get_code();
		if(code == cc)
		 get_code();
		get_string(code);
		output_string();
		old = code;

		while(true)
		{
		 get_code();

		 if(code == cc)
		 {
		  clear_table();
		  get_code();
		  get_string(code);
		  output_string();
		  old = code;
		 }
		 else if(code == cc + 1)
		 {
		  break;
		 }
		 else if(code < empty_str)
		 {
		  get_string(code);
		  output_string();

		  if(bit_overflow == 0)
		  {
		   str[empty_str].base = old;
		   str[empty_str].newc = string[0];
		   str[empty_str].length = str[old].length + 1;
		   empty_str++;
		   if(empty_str == (1 << curr_bit_size))
			curr_bit_size++;
		   if(curr_bit_size == 13)
		   {
			curr_bit_size = 12;
			bit_overflow = 1;
		   }
		  }
		  old = code;
		 }
		 else
		 {
		  get_string(old);
		  string[str[old].length] = string[0];
		  string_length++;

		  if(bit_overflow == 0)
		  {
		   str[empty_str].base = old;
		   str[empty_str].newc = string[0];
		   str[empty_str].length = str[old].length + 1;
		   empty_str++;
		   if(empty_str == (1 << curr_bit_size))
			curr_bit_size++;
		   if(curr_bit_size == 13)
		   {
			curr_bit_size = 12;
			bit_overflow = 1;
		   }
		  }
		  output_string();
		  old = code;
		 }
		}
		break;

   case 0x21:
		i = _XVFGet8(f);
		if(i == 0xF9)
		{
		 _XVFSeek(f, 1);
		 i = _XVFGet8(f);
		 if(i & 1)
		 {
		  _XVFSeek(f, 2);
		  _XVFGet8(f);
		 }
		 else
		 {
		  _XVFSeek(f, 3);
		 }
		}
		i = _XVFGet8(f);

		while(i)
		{
		 _XVFSeek(f, i);
		 i = _XVFGet8(f);
		}
		break;

   case 0x3B:
  	    bmp2 = _XCreateBitmap(gif_tmp_bmp->w, gif_tmp_bmp->h, 24);
		if(!bmp2)
		{
		 _XDestroyBitmap(gif_tmp_bmp);
		 _XLogError("_XVFReadGIF() - Can't create bmp2 !");
		 return NULL;
		}

		_XSelectPalette(pal);
		_XBlit(gif_tmp_bmp, bmp2, 0, 0, 0, 0, gif_tmp_bmp->w, gif_tmp_bmp->h);
		_XUnselectPalette();

		_XDestroyBitmap(gif_tmp_bmp);
		gif_tmp_bmp = bmp2;
		return gif_tmp_bmp;
  }
 } while(true);

 return NULL;
}

typedef struct BUFFER
{
 int pos;
 int bit_pos;
 unsigned char data[255];
} BUFFER;

static void clear_speed_buffer(short *speed_buffer)
{
 memset(speed_buffer, -1, (256*4096/2)*4);
}

static void dump_buffer(BUFFER *b, _XVFile *f)
{
 int size;

 size = b->pos;
 if(b->bit_pos != 0)
  size ++;

 _XVFPut8(f, size);
 _XVFWrite(f, b->data, size);
}

static void output(BUFFER *b, int bit_size, int code, _XVFile *f)
{
 int shift;

 shift = b->bit_pos;
 do
 {
  if(shift >= 0)
  {
   if(b->bit_pos != 0)
	b->data[b->pos] = (unsigned char)((code << shift) | b->data[b->pos]);
   else
	b->data[b->pos] = (unsigned char)(code << shift);
  }
  else
  {
   if(b->bit_pos != 0)
	b->data[b->pos] = (unsigned char)((code >> -shift) | b->data[b->pos]);
   else
	b->data[b->pos] = (unsigned char)(code >> -shift);
  }

  if(bit_size + shift > 7)
  {
   b->bit_pos = 0;
   b->pos ++;
   shift -= 8;
   if(b->pos == 255)
   {
	dump_buffer(b, f);
	b->pos = 0;
	b->bit_pos = 0;
   }
   if(bit_size + shift <= 0)
	break;
  }
  else
  {
   b->bit_pos = bit_size + shift;
   break;
  }
 } while(true);
}


int _XVFWriteGIF(_XVFile *f, _XBitmap *bmp, _XRGB *pal)
{
 int i, bpp, bit_size;
 LZW_STRING string_table[4096];
 int prefix;
 int input_pos = 0;
 int c;
 int empty_str;
 BUFFER buffer;
 short *speed_buffer;

 if(pal)
  _XSelectPalette(pal);

 _XVFPutBE32(f, 0x47494638);
 _XVFPutBE16(f, 0x3761);
 _XVFPutLE16(f, bmp->w);
 _XVFPutLE16(f, bmp->h);
 _XVFPut8(f, 215);
 _XVFPut8(f, 0);
 _XVFPut8(f, 0);

 if(pal)
 {
  for(i = 0; i < 256; i ++)
  {
   _XVFPut8(f, pal[i].r);
   _XVFPut8(f, pal[i].g);
   _XVFPut8(f, pal[i].b);
  }
 }
 else
 {
  for(i = 0; i < 256; i ++)
  {
   _XVFPut8(f, 0);
   _XVFPut8(f, 0);
   _XVFPut8(f, 0);
  } 
 }

 _XVFPut8(f, 0x2c);
 _XVFPutLE16(f, 0);
 _XVFPutLE16(f, 0);
 _XVFPutLE16(f, bmp->w);
 _XVFPutLE16(f, bmp->h);
 _XVFPut8(f, 0);

 bpp = 8;
 _XVFPut8(f, bpp);

 for(i = 0; i < 1 << bpp; i ++)
 {
  string_table[i].base = -1;
  string_table[i].newc = i;
 }
 for(; i < (1 << bpp) + 2; i ++)
 {
  string_table[i].base = -1;
  string_table[i].newc = -1;
 }
 empty_str = (1 << bpp) + 2;

 prefix = -1;

 bit_size = bpp + 1;

 buffer.pos = 0;
 buffer.bit_pos = 0;

 output(&buffer, bit_size, 1 << bpp, f);

 speed_buffer = (short*)malloc(256 * 4096 * 2);
 clear_speed_buffer(speed_buffer);

 while(true)
 {
  c = get_pixel(bmp, input_pos % bmp->w, input_pos / bmp->w);

  if(c == EOF)
  {
   output(&buffer, bit_size, prefix, f);
   output(&buffer, bit_size, (1 << bpp) + 1, f);
   dump_buffer(&buffer, f);
   _XVFPut8(f, 0);
   break;
  }
  else if(bmp->bpp != 8)
  {
   int r, g, b;
   r = _XGetREx(c, bmp->bpp);
   g = _XGetGEx(c, bmp->bpp);
   b = _XGetBEx(c, bmp->bpp);
   c = _XRGB8(r, g, b);
  }

  input_pos ++;

  if(prefix == -1)
   i = c;
  else
   i = speed_buffer[prefix * 256 + c];

  if(i != -1)
  {
   prefix = i;
  }
  else
  {
   string_table[empty_str].base = prefix;
   string_table[empty_str].newc = c;
   speed_buffer[prefix * 256 + c] = empty_str;

   empty_str ++;

   output(&buffer, bit_size, prefix, f);

   if(empty_str == (1 << bit_size) + 1)
	bit_size ++;

   if(empty_str == 4095)
   {
	output(&buffer, bit_size, 1 << bpp, f);
	empty_str = (1 << bpp) + 2;
	bit_size = bpp + 1;
	clear_speed_buffer(speed_buffer);
   }

   prefix = c;
  }
 }

 free(speed_buffer);

 _XVFPut8(f, 0x3b);

 if(pal)
  _XUnselectPalette();

 return 1;
}
