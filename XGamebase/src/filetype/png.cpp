#include "xgamebase.h"

#include "png.h"

static void png_read_data(png_structp ctx, png_bytep area, png_size_t size);

static void png_read_data(png_structp ctx, png_bytep area, png_size_t size)
{
 _XVFile *fp = (_XVFile*)png_get_io_ptr(ctx);
 _XVFRead(fp, area, size);
}

bool _XVFIsPNG(_XVFile *fp)
{
 char buf[8];

 if(_XVFRead(fp, buf, 8) != 8)
  return false;

 /* Compare the 8 bytes of the signature. */
 return !png_sig_cmp((U8*)buf, (png_size_t)0, 8);
}

_XBitmap *_XVFReadPNG(_XVFile *fp, _XPalette pal)
{
 png_structp png_ptr = NULL;
 png_infop   info_ptr = NULL; 
 _XBitmap *bmp = NULL;
 png_uint_32 width, height; 
 int bit_depth, color_type, interlace_type;
 int i;
 U8  buf[4];

 /* Create the PNG loading context structure */
 png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
 if(!png_ptr)
 {
  _XLogError("_XVFReadPNG() - Unable to create png reading structure !");
  goto done;
 }

 /* Allocate/initialize the memory for image information. REQUIRED. */
 info_ptr = png_create_info_struct(png_ptr);
 if(!info_ptr)
 {
  _XLogError("_XVFReadPNG() - Unable to create the info structure !");
  goto done;
 }

 /* Set error handling if you are using setjmp/longjmp method (this is
  * the normal method of doing things with libpng).  REQUIRED unless you
  * set up your own error handlers in png_create_read_struct() earlier.
  */
 if(setjmp(png_ptr->jmpbuf))
 {
  png_destroy_read_struct(&png_ptr,
                          info_ptr ? &info_ptr : (png_infopp)0,
                          (png_infopp)0); 	
  _XLogError("_XVFReadPNG() - Error reading the PNG file !");
  return NULL;
 }

 /* Set up the input control */
 png_set_read_fn(png_ptr, fp, png_read_data);

 /* Read PNG header info */
 png_read_info(png_ptr, info_ptr);
 png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
			  &color_type, &interlace_type, NULL, NULL);

 /* tell libpng to strip 16 bit/color files down to 8 bits/color */
 png_set_strip_16(png_ptr) ;

 /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
  * byte into separate bytes (useful for paletted and grayscale images).
  */
 png_set_packing(png_ptr);

 /* Expand images to the full 8 bits from 1, 2, or 4 bits/pixel */
 if(bit_depth < 8)
  png_set_expand(png_ptr);

 /* Invert the alpha values of the image. */
 /*	png_set_invert_alpha(png_ptr); */

 /* Change the order of packed pixels to least significant bit first
  * (not useful if you are using png_set_packing).
  */
 /* png_set_packswap(png_ptr); */
	
 /* Expand paletted or RGB images with transparency to alpha channels
  * so the data will be available as RGBA quartets.
  */
 if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
  png_set_expand(png_ptr);

 if((color_type == PNG_COLOR_TYPE_GRAY) ||
	(color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
  png_set_gray_to_rgb(png_ptr);

 png_read_update_info(png_ptr, info_ptr);

 png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
			  &color_type, &interlace_type, NULL, NULL);

 /* Create the bitmap to hold the image. */
 bmp = _XCreateBitmap(width, height, bit_depth*info_ptr->channels);
 if(!bmp)
 {
  _XLogError("_XVFReadPNG() - Unable to create bitmap !");
  goto done;
 }

 /* Read the entire image in one go */
 png_read_image(png_ptr, bmp->line);

 // If it's necessary convert the swap the rgb values.
 switch(bmp->bpp)
 {  
  case 24:
      {
       if(_rgb_r_shift_24 == 16)
       {
        U8 *src = (U8*)bmp->idat;
        for(i = 0; i < bmp->size; i+=3)
        {
         memcpy(buf, (void*)(src+i), 3);
         src[i+0] = buf[2]; // 버그가 있을것 같은데...
         src[i+1] = buf[1];
         src[i+2] = buf[0];
        }
       }
       break;
      }

  case 32:
      {
       if(_rgb_r_shift_32 == 16)
       {
        U8 *src = (U8*)bmp->idat;
        for(i = 0; i < bmp->size; i+=4)
        {
         memcpy(buf, (void*)(src+i), 4);
         //src[i+0] = buf[3]; // D3D BGRA...
         //src[i+1] = buf[2];
         //src[i+2] = buf[1];
         //src[i+3] = buf[0];

		 src[i+0] = buf[0]; // D3D RGBA...
         src[i+1] = buf[1];
         src[i+2] = buf[2];
         src[i+3] = buf[3];
        }
       }
       break;
      }
  
  default: break;
 }

 /* read _XRest of file, get additional chunks in info_ptr - REQUIRED */
 png_read_end(png_ptr, info_ptr);

 /* Load the palette, if any */
 if(pal)
 {
  for(i = 0; i < info_ptr->num_palette; i++)
  {
   pal[i].b =(U8)info_ptr->palette[i].blue;
   pal[i].g =(U8)info_ptr->palette[i].green;
   pal[i].r =(U8)info_ptr->palette[i].red;
  }
 }

done:
 /* Clean up and return. */
 png_destroy_read_struct(&png_ptr,
                         info_ptr ? &info_ptr : (png_infopp)0,
                         (png_infopp)0);

 return bmp; 
}

