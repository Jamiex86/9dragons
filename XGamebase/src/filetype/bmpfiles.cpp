#include "xgamebase.h"

#define BMP_EXT_SIZE       5
#define MAX_BITMAP_TYPES   16

static struct
{
 char ext[5];
 bool   (*is)(_XVFile *rwops);
 _XBitmap *(*read)(_XVFile *rwops, _XRGB *pal);
 int (*write)(_XVFile *rwops, _XBitmap *bmp, _XRGB *pal);
} bitmap_types[MAX_BITMAP_TYPES];

static int _XGetBitmapConvMode_needed_bpp(void);
static int _XGetBitmapConvMode_needed_bpp(void);

static int bmpconv_mode = BMPCONV_NONE,
		   pushed_bmpconv_mode = BMPCONV_NONE;

static int first = 1;

static int _XGetBitmapConvMode_needed_bpp(void)
{
 if(bmpconv_mode & BMPCONV_TO8BPP)
  return 8;

 if(bmpconv_mode & BMPCONV_TO15BPP)
  return 15;

 if(bmpconv_mode & BMPCONV_TO16BPP)
  return 16;

 if(bmpconv_mode & BMPCONV_TO24BPP)
  return 24;

 if(bmpconv_mode & BMPCONV_TO32BPP)
  return 32;

 return 0;
}

static void init_bitmap_filetypes(void)
{
 int i = 0;

 strncpy(bitmap_types[i].ext, "jpg", 5);
 bitmap_types[i].is = _XVFIsJPG;
 bitmap_types[i].read = _XVFReadJPG;
 bitmap_types[i].write = NULL;
 i++;

 strncpy(bitmap_types[i].ext, "jpeg", 5);
 bitmap_types[i].is = _XVFIsJPG;
 bitmap_types[i].read = _XVFReadJPG;
 bitmap_types[i].write = NULL;
 i++;

 strncpy(bitmap_types[i].ext, "png", 5);
 bitmap_types[i].is = _XVFIsPNG;
 bitmap_types[i].read = _XVFReadPNG;
 bitmap_types[i].write = NULL;
 i++;

 strncpy(bitmap_types[i].ext, "bmp", 5);
 bitmap_types[i].is = _XVFIsBMP;
 bitmap_types[i].read = _XVFReadBMP;
 bitmap_types[i].write = _XVFWriteBMP;
 i++;

 strncpy(bitmap_types[i].ext, "pcx", 5);
 bitmap_types[i].is = _XVFIsPCX;
 bitmap_types[i].read = _XVFReadPCX;
 bitmap_types[i].write = _XVFWritePCX;
 i++;

 strncpy(bitmap_types[i].ext, "gif", 5);
 bitmap_types[i].is = _XVFIsGIF;
 bitmap_types[i].read = _XVFReadGIF;
 bitmap_types[i].write = _XVFWriteGIF;
 i++;

 strncpy(bitmap_types[i].ext, "tga", 5);
 bitmap_types[i].is = _XVFIsTGA;
 bitmap_types[i].read = _XVFReadTGA;
 bitmap_types[i].write = _XVFWriteTGA;
 i++;

 for(; i < MAX_BITMAP_TYPES; i++)
 {
  bitmap_types[i].is = NULL;
  bitmap_types[i].read = NULL;
  bitmap_types[i].write = NULL;
 }
}

void _XSetBitmapConvMode(int mode)
{
 bmpconv_mode = mode;
}

int _XGetBitmapConvMode(void)
{
 return bmpconv_mode;
}

void _XPushBitmapConvMode(void)
{
 pushed_bmpconv_mode = bmpconv_mode;
}

void _XPopBitmapConvMode(void)
{
 bmpconv_mode = pushed_bmpconv_mode;
}

void _XRegisterBitmapFileType(char *ext,
                              bool (*is)(_XVFile *rwops),
						      _XBitmap *(*read)(_XVFile *rwops, _XRGB *pal),
						      int (*write)(_XVFile *rwops, _XBitmap *bmp, _XRGB *pal))
{
 int i;

 for(i = 0; i < MAX_BITMAP_TYPES; i++)
 {
  if((!bitmap_types[i].is) || (stricmp(bitmap_types[i].ext,ext)==0))
  {
   strncpy(bitmap_types[i].ext, ext, 5);
   bitmap_types[i].is = is;
   bitmap_types[i].read = read;
   bitmap_types[i].write = write;
   return;
  }
 }
}

bool _XVFIsBitmap(_XVFile *fp)
{
 U32 i = 0, start;

 if(first)
 {
  init_bitmap_filetypes();
  first = 0;
 }

 start = _XVFTell(fp);
 while(bitmap_types[i].is)
 {
  _XVFSeekSet(fp, start);
  if(bitmap_types[i].is(fp))
  {
   if(bitmap_types[i].read)
   {
	return true;
   }
   else
   {
	return false;
   }
  }
  i++;
 }

 return false;
}

/*
_XBitmap *XVFLoadBitmap( FILE *fileptr, _XPalette pal)
{
	 _XBitmap *bmp = NULL;
	 int i = 0, bpp, start;

	 if(first)
	 {
		 init_bitmap_filetypes();
		 first = 0;
	 }

	 start = ftell(fileptr);
	 while(bitmap_types[i].is)
	 {
		  fseek( fileptr, start, SEEK_SET );
		  if(bitmap_types[i].is(fileptr))
		  {
			   if(bitmap_types[i].read)
			   {
					fseek(fileptr, start);
					bmp = bitmap_types[i].read(fileptr, pal);
					if(!bmp)
						return NULL;
					else
						goto terminate; 
			   }
			   else
			   {
					_XLogError("XLoadBitmap() - Not a loadable bitmap file type [%s] !", bitmap_types[i].ext);
					return NULL;
			   }
		  }
		  i++;
	 }
	 _XLogError("_XVFLoadBitmap() - Unknow bitmap file type !");
	 return NULL;

terminate:;
	 bpp = _XGetBitmapConvMode_needed_bpp();
	 if((bpp != bmp->bpp) && (bpp != 0))
	 {
		 if(bmpconv_mode & BMPCONV_MASK)
			bmp = _XConvertBitmapMaskBpp(bmp, pal, bpp, 1);
		 else
			bmp = _XConvertBitmapBpp(bmp, pal, bpp, 1);
	 }
	 return bmp;
}
*/

_XBitmap *_XVFLoadBitmap(_XVFile *fp, _XRGB *pal)
{
 _XBitmap *bmp = NULL;
 int i = 0, bpp, start;

 if(first)
 {
  init_bitmap_filetypes();
  first = 0;
 }

 start = _XVFTell(fp);
 while(bitmap_types[i].is)
 {
  _XVFSeekSet(fp, start);
  if(bitmap_types[i].is(fp))
  {
   if(bitmap_types[i].read)
   {
    _XVFSeekSet(fp, start);
	bmp = bitmap_types[i].read(fp, pal);
	if(!bmp)
	 return NULL;
	else
	 goto terminate; 
   }
   else
   {
	_XLogError("_XVFLoadBitmap() - Not a loadable bitmap file type [%s] !", bitmap_types[i].ext);
	return NULL;
   }
  }
  i++;
 }
 _XLogError("_XVFLoadBitmap() - Unknow bitmap file type !");
 return NULL;

terminate:;
 bpp = _XGetBitmapConvMode_needed_bpp();
 if((bpp != bmp->bpp) && (bpp != 0))
 {
  if(bmpconv_mode & BMPCONV_MASK)
   bmp = _XConvertBitmapMaskBpp(bmp, pal, bpp, 1);
  else
   bmp = _XConvertBitmapBpp(bmp, pal, bpp, 1);
 }

 return bmp;
}

bool _XIsBitmapFile(char *filename)
{
 bool ret;

 _XVFile *fp = _XVFOpenFile(filename, "rb");
 failed_BOOL(!fp, _XGetTextf("_XIsBitmapFile() - Unable to open file [%s] !", filename));

 ret = _XVFIsBitmap(fp);

 _XDestroyVFile(fp);

 return ret;
}

_XBitmap *_XLoadBitmap(char *filename, _XRGB *pal)
{
 _XBitmap *bmp = NULL;
 _XVFile *fp = _XVFOpenFile(filename, "rb");
 failed_NULL(!fp, _XGetTextf("_XLoadBitmap() - Unable to open file [%s] !", filename));

 bmp = _XVFLoadBitmap(fp, pal);

 _XDestroyVFile(fp);

 return bmp;
}

int _XSaveBitmap(char *filename, _XBitmap *bmp, _XRGB *pal)
{
 int i;
 _XVFile *rwops;

 if(first)
 {
  init_bitmap_filetypes();
  first = 0;
 }

 for(i = 0; i < MAX_BITMAP_TYPES; i++)
 {
  if((bitmap_types[i].ext[0]) && (stricmp(bitmap_types[i].ext, _XPutExtension(filename)) == 0))
  {
   if(bitmap_types[i].write)
   {
	rwops = _XVFOpenFile(filename, "wb");
	if(!rwops)
	{
	 _XLogError("_XSaveBitmap() - Unable to create file [%s] !", filename);
	 return 0;
	}

	i = bitmap_types[i].write(rwops, bmp, pal);

	_XDestroy(VFile, rwops);
	return i;
   }
   else
   {
	_XLogError("_XSaveBitmap(%s) - Not a savable bitmap file type !", filename);
	return 0;
   }
  }
 }

 _XLogError("_XSaveBitmap(%s) - Unknow bitmap file type !", filename);
 return 0;
}

