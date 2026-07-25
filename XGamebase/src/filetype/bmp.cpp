
#include "xgamebase.h"

#define MY_BI_RGB          0
#define MY_BI_RLE8         1
#define MY_BI_RLE4         2
#define MY_BI_BITFIELDS    3

#define OS2INFOHEADERSIZE  12
#define WININFOHEADERSIZE  40

typedef struct BMPHeader
{
 unsigned long  bfType;
 unsigned long  bfSize;
 unsigned short bfReserved1;
 unsigned short bfReserved2;
 unsigned long  bfOffBits;
} BMPHeader;

typedef struct BMPInfoHeader
{
 unsigned long  biWidth;
 unsigned long  biHeight;
 unsigned short biBitCount;
 unsigned long  biCompression;
} BMPInfoHeader;

typedef struct WinBMPInfoHeader
{
 unsigned long  biWidth;
 unsigned long  biHeight;
 unsigned short biPlanes;
 unsigned short biBitCount;
 unsigned long  biCompression;
 unsigned long  biSizeImage;
 unsigned long  biXPelsPerMeter;
 unsigned long  biYPelsPerMeter;
 unsigned long  biClrUsed;
 unsigned long  biClrImportant;
} WinBMPInfoHeader;

typedef struct OS2BMPInfoHeader
{
 unsigned short biWidth;
 unsigned short biHeight;
 unsigned short biPlanes;
 unsigned short biBitCount;
} OS2BMPInfoHeader;

static int read_bmfileheader(_XVFile *rwops, BMPHeader *fileheader)
{
 fileheader->bfType = _XVFGetLE16(rwops);
 fileheader->bfSize = _XVFGetLE32(rwops);
 fileheader->bfReserved1 = _XVFGetLE16(rwops);
 fileheader->bfReserved2 = _XVFGetLE16(rwops);
 fileheader->bfOffBits = _XVFGetLE32(rwops);

 return 1;
}

static int read_win_bminfoheader(_XVFile *rwops, BMPInfoHeader *infoheader)
{
 WinBMPInfoHeader win_infoheader;

 win_infoheader.biWidth = _XVFGetLE32(rwops);
 win_infoheader.biHeight = _XVFGetLE32(rwops);
 win_infoheader.biPlanes = _XVFGetLE16(rwops);
 win_infoheader.biBitCount = _XVFGetLE16(rwops);
 win_infoheader.biCompression = _XVFGetLE32(rwops);
 win_infoheader.biSizeImage = _XVFGetLE32(rwops);
 win_infoheader.biXPelsPerMeter = _XVFGetLE32(rwops);
 win_infoheader.biYPelsPerMeter = _XVFGetLE32(rwops);
 win_infoheader.biClrUsed = _XVFGetLE32(rwops);
 win_infoheader.biClrImportant = _XVFGetLE32(rwops);
 infoheader->biWidth = win_infoheader.biWidth;
 infoheader->biHeight = win_infoheader.biHeight;
 infoheader->biBitCount = win_infoheader.biBitCount;
 infoheader->biCompression = win_infoheader.biCompression;

 return 1;
}

static int read_os2_bminfoheader(_XVFile *rwops, BMPInfoHeader *infoheader)
{
 OS2BMPInfoHeader os2_infoheader;

 os2_infoheader.biWidth = _XVFGetLE16(rwops);
 os2_infoheader.biHeight = _XVFGetLE16(rwops);
 os2_infoheader.biPlanes = _XVFGetLE16(rwops);
 os2_infoheader.biBitCount = _XVFGetLE16(rwops);

 infoheader->biWidth = os2_infoheader.biWidth;
 infoheader->biHeight = os2_infoheader.biHeight;
 infoheader->biBitCount = os2_infoheader.biBitCount;
 infoheader->biCompression = 0;

 return 1;
}

static void read_bmicolors(int ncols, _XRGB *pal, _XVFile *rwops, int win_flag)
{
 int i;

 for(i=0; i<ncols; i++)
 {
  if(pal)
  {
   pal[i].b = _XVFGet8(rwops);
   pal[i].g = _XVFGet8(rwops);
   pal[i].r = _XVFGet8(rwops);
  }
  else
  {
   _XVFGet8(rwops);
   _XVFGet8(rwops);
   _XVFGet8(rwops);
  }

  if(win_flag)
   _XVFGet8(rwops);
 }
}

static void read_8bit_line(int length, _XVFile *rwops, _XBitmap *bmp, int line)
{
 unsigned char b[4];
 unsigned long n;
 int i, j, k;
 int pix;

 for(i = 0; i < length; i++)
 {
  j = i % 4;

  if(j == 0)
  {
   n = _XVFGetLE32(rwops);

   for(k = 0; k < 4; k++)
   {
	b[k] = n & 255;
	n = n >> 8;
   }
  }

  pix = b[j];
  bmp->line[line][i] = pix;
 }
}


static void read_24bit_line(int length, _XVFile *rwops, _XBitmap *bmp, int line)
{
 int i, nbytes;
 int r, g, b;

 nbytes = 0;

 for(i = 0; i < length; i++)
 {
  b = _XVFGet8(rwops);
  g = _XVFGet8(rwops);
  r = _XVFGet8(rwops);
  bmp->line[line][(i*3)+(_rgb_r_shift_24>>3)] = r;
  bmp->line[line][(i*3)+(_rgb_g_shift_24>>3)] = g;
  bmp->line[line][(i*3)+(_rgb_b_shift_24>>3)] = b;
  nbytes += 3;
 }

 nbytes = nbytes % 4;

 if(nbytes != 0)
  for(i = nbytes; i < 4; i++)
   _XVFGet8(rwops);
}


static int read_image(_XVFile *f, _XBitmap *bmp, BMPInfoHeader *infoheader)
{
 int i;

 for(i = 0; i < (int)infoheader->biHeight; i++)
 {
  switch(infoheader->biBitCount)
  {
   case 8:  read_8bit_line(infoheader->biWidth, f, bmp, infoheader->biHeight-i-1); break;
   case 24: read_24bit_line(infoheader->biWidth, f, bmp, infoheader->biHeight-i-1); break;
   default: return 0;
  }
 }

 return 1;
}


static void read_RLE8_compressed_image(_XVFile *rwops, _XBitmap *bmp,
																		   BMPInfoHeader *infoheader)
{
 unsigned char count, val, val0;
 int j, pos, line;
 int eolflag, eopicflag;

 eopicflag = 0;
 line = infoheader->biHeight - 1;

 while(eopicflag == 0)
 {
  pos = 0;
  eolflag = 0;

  while((eolflag == 0) && (eopicflag == 0))
  {
   count = _XVFGet8(rwops);
   val = _XVFGet8(rwops);

   if(count > 0)
   {
	for(j = 0; j < count;j++)
	{
	 bmp->line[line][pos] = val;
	 pos++;
	}
   }
   else
   {
	switch(val)
	{
	 case 0:
		  eolflag = 1;
		  break;

	 case 1:
		  eopicflag = 1;
		  break;

	 case 2:
		  count = _XVFGet8(rwops);
		  val = _XVFGet8(rwops);
		  pos += count;
		  line -= val;
		  break;

	 default:
		  for(j = 0; j < val; j++)
		  {
		   val0 = _XVFGet8(rwops);
		   bmp->line[line][pos] = val0;
		   pos++;
		  }

		  if(j%2 == 1)
		   val0 = _XVFGet8(rwops);
		  break;
	}
   }

   if(pos > (int)infoheader->biWidth)
	eolflag = 1;
  }

  line--;
  if(line < 0)
   eopicflag = 1;
 }
}

bool _XVFIsBMP(_XVFile *rwops)
{
 bool is_bmp = false;
 char magic[2];

 if(_XVFRead(rwops, magic, 2))
 {
  if(strncmp(magic, "BM", 2) == 0)
  {
   is_bmp = true;
  }
 }

 return is_bmp;
}


_XBitmap *_XVFReadBMP(_XVFile *rwops, _XRGB *pal)
{
 BMPHeader fileheader;
 BMPInfoHeader infoheader;
 _XBitmap *bmp;
 int ncol;
 unsigned long biSize;
 int bpp;

 if(!read_bmfileheader(rwops, &fileheader))
 {
  _XLogError("_XVFReadBMP() - Not a valid bitmap file !");
  return NULL;
 }

 biSize = _XVFGetLE32(rwops);

 if(biSize == WININFOHEADERSIZE)
 {
  if(!read_win_bminfoheader(rwops, &infoheader))
  {
   _XLogError("_XVFReadBMP() - Can't read win header !");
   return NULL;
  }

  ncol = (fileheader.bfOffBits - 54) / 4;
  read_bmicolors(ncol, pal, rwops, 1);
 }
 else if(biSize == OS2INFOHEADERSIZE)
 {
  if(!read_os2_bminfoheader(rwops, &infoheader))
  {
   _XLogError("_XVFReadBMP() - Can't read OS2 header !");
   return NULL;
  }

  ncol = (fileheader.bfOffBits - 26) / 3;
  read_bmicolors(ncol, pal, rwops, 0);
 }
 else
 {
  _XLogError("_XVFReadBMP() - Unknow bitmap header !");
  return NULL;
 }

 if(infoheader.biBitCount == 24)
 {
  bpp = 24;
  if(pal)
   _XGenerate332Palette(pal);
 }
 else
 {
  bpp = 8;
 }

 bmp = _XCreateBitmap(infoheader.biWidth, infoheader.biHeight, bpp);
 if(!bmp)
 {
  _XLogError("_XVFReadBMP() - Can't create bitmap !");
  return NULL;
 }

 switch(infoheader.biCompression)
 {
  case MY_BI_RGB:
	   if(!read_image(rwops, bmp, &infoheader))
	   {
		_XDestroyBitmap(bmp);
		bmp = NULL;
	   }
	   break;

  case MY_BI_RLE8:
	   read_RLE8_compressed_image(rwops, bmp, &infoheader);
	   break;

  default:
	   _XDestroyBitmap(bmp);
	   bmp = NULL;
	   break;
 }

 return bmp;
}


int _XVFWriteBMP(_XVFile *rwops, _XBitmap *bmp, _XRGB *pal)
{
 int bfSize;
 int depth = bmp->bpp;
 int bpp = (depth == 8) ? 8 : 24;
 int filler = 3 - ((bmp->w*(bpp/8)-1) & 3);
 int c, i, j;

 if(bpp == 8)
 {
  bfSize = 54 + 256*4 + bmp->w*bmp->h;
 }
 else
 {
  bfSize = 54 + bmp->w*bmp->h*3;
 }

 _XVFPutLE16(rwops, 0x4D42);
 _XVFPutLE32(rwops, bfSize);
 _XVFPutLE16(rwops, 0);
 _XVFPutLE16(rwops, 0);

 if(bpp == 8)
  _XVFPutLE32(rwops, 54+256*4);
 else
  _XVFPutLE32(rwops, 54);

 bfSize = bmp->w * bmp->h * bpp/8;

 _XVFPutLE32(rwops, 40);
 _XVFPutLE32(rwops, bmp->w);
 _XVFPutLE32(rwops, bmp->h);
 _XVFPutLE16(rwops, 1);
 _XVFPutLE16(rwops, bpp);
 _XVFPutLE32(rwops, 0);
 _XVFPutLE32(rwops, bfSize);
 _XVFPutLE32(rwops, 0);
 _XVFPutLE32(rwops, 0);

 if(bpp == 8)
 {
  _XVFPutLE32(rwops, 256);
  _XVFPutLE32(rwops, 256);

  for(i = 0; i < 256; i++)
  {
   if(pal)
   {
	_XVFPut8(rwops, pal[i].b);
	_XVFPut8(rwops, pal[i].g);
	_XVFPut8(rwops, pal[i].r);
	_XVFPut8(rwops, 0);
   }
   else
   {
	_XVFPut8(rwops, 0);
	_XVFPut8(rwops, 0);
	_XVFPut8(rwops, 0);
	_XVFPut8(rwops, 0);
   }
  }
 }
 else
 {
  _XVFPutLE32(rwops, 0);
  _XVFPutLE32(rwops, 0);
 }

 for(i = bmp->h-1; i >= 0; i--)
 {
  for(j = 0; j < bmp->w; j++)
  {
   if(bpp == 8)
   {
	_XVFPut8(rwops, get_pixel(bmp, j, i));
   }
   else
   {
	c = get_pixel(bmp, j, i);
	_XVFPut8(rwops, _XGetBEx(c,depth));
	_XVFPut8(rwops, _XGetGEx(c,depth));
	_XVFPut8(rwops, _XGetREx(c,depth));
   }
  }

  for(j = 0; j < filler; j++)
   _XVFPut8(rwops, 0);
 }

 return 1;
}
