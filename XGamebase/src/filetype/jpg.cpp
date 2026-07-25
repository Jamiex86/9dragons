#include "xgamebase.h"

#include "jpeglib.h"
#include "setjmp.h"

typedef struct my_error_mgr *my_error_ptr;

static void init_source(j_decompress_ptr cinfo);
static int fill_input_buffer(j_decompress_ptr cinfo);
static void skip_input_data(j_decompress_ptr cinfo, long num_bytes);
static void term_source(j_decompress_ptr cinfo);
static void jpeg_src(j_decompress_ptr cinfo, _XVFile *ctx);

void my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	(*cinfo->err->output_message)(cinfo);
	if(*(myerr->bmp)) _XDestroyBitmap(*(myerr->bmp));
	if(*(myerr->buf)) free(*(myerr->buf));
	longjmp(myerr->setjmp_buffer, 1);
}

void my_output_message(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];
	
	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);
	
	/* Send it to stderr, adding a newline */
	_XLogError("JPEG: %s", buffer);
}

static void init_source(j_decompress_ptr cinfo)
{
	/* We don't actually need to do anything */
	return;
}

static int fill_input_buffer(j_decompress_ptr cinfo)
{
	my_source_mgr *src = (my_source_mgr*)cinfo->src;
	int nbytes;
	
	nbytes = _XVFRead(src->ctx, src->buffer, INPUT_BUFFER_SIZE);
	if(nbytes <= 0)
	{
		/* Insert a fake EOI marker */
		src->buffer[0] = (U8)0xFF;
		src->buffer[1] = (U8)JPEG_EOI;
		nbytes = 2;
	}
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	
	return TRUE;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	my_source_mgr *src = (my_source_mgr*)cinfo->src;
	
	/* Just a dumb implementation for now.	Could use fseek() except
	* it doesn't work on pipes.  Not clear that being smart is worth
	* any trouble anyway --- large skips are infrequent.
	*/
	if(num_bytes > 0)
	{
		while(num_bytes > (long) src->pub.bytes_in_buffer)
		{
			num_bytes -= (long)src->pub.bytes_in_buffer;
			src->pub.fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never
			* return FALSE, so suspension need not be handled.
			*/
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

static void term_source(j_decompress_ptr cinfo)
{
	/* We don't actually need to do anything */
	return;
}

static void jpeg_src(j_decompress_ptr cinfo, _XVFile *ctx)
{
	my_source_mgr *src;
	
	// The source object and input buffer are made permanent so that a series
	// of JPEG images can be read from the same file by calling jpeg_stdio_src
	// only before the first one.  (If we discarded the buffer at the end of
	// one image, we'd likely lose the start of the next one.)
	// This makes it unsafe to use this manager and a different source
	// manager serially with the same JPEG object.  Caveat programmer.
	if(cinfo->src == NULL)
	{
		// first time for this JPEG object?
		cinfo->src = (struct jpeg_source_mgr*)
			(*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_source_mgr));
		src = (my_source_mgr*)cinfo->src;
	}
	
	src = (my_source_mgr *)cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method
	src->pub.term_source = term_source;
	src->ctx = ctx;
	src->pub.bytes_in_buffer = 0;    // forces fill_input_buffer on first read
	src->pub.next_input_byte = NULL; // until buffer loaded
}

bool _XVFIsJPG(_XVFile *src)
{
	bool is_jpeg = false;
	U8 magic[4];
	
	if(_XVFRead(src, magic, 2) > 0)
	{
		if((magic[0] == 0xFF) && (magic[1] == 0xD8))
		{
			_XVFRead(src, magic, 4);
			_XVFRead(src, magic, 4);
			if(strncmp((char*)magic, "JFIF", 4) == 0)
			{
				is_jpeg = true;
			}
		}
	}
	
	return is_jpeg;
}

bool _XVFIsJPG(FILE* src)
{
	bool is_jpeg = false;
	U8 magic[4];
	
	if(fread(magic, 2, 1, src) > 0)
	{
		if((magic[0] == 0xFF) && (magic[1] == 0xD8))
		{
			fread( magic, 4, 1, src );
			fread( magic, 4, 1, src );
			if(strncmp((char*)magic, "JFIF", 4) == 0)
			{
				is_jpeg = true;
			}
		}
	}
	
	return is_jpeg;
}

_XBitmap *_XVFReadJPG(_XVFile *src, _XPalette pal)
{
	struct my_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	JSAMPROW rowptr[1];
	U8 *allocbuffer = NULL;
	_XBitmap *bmp = NULL;
	U8 *buffer = NULL; 
	int i;
	
	/* Create a decompression structure and load the JPEG header */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	jerr.pub.output_message = my_output_message;
	jerr.bmp = &bmp;
	jerr.buf = &allocbuffer;
	
	if(setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}
	
	jpeg_create_decompress(&cinfo);
	jpeg_src(&cinfo, src);
	jpeg_read_header(&cinfo, TRUE);
	
	/* Set 24-bit RGB output */
	cinfo.out_color_space = JCS_RGB;
	cinfo.quantize_colors = FALSE;
	jpeg_calc_output_dimensions(&cinfo);
	
	/* Allocate an output surface to hold the image */
	bmp = _XCreateBitmap(cinfo.output_width, cinfo.output_height, 24);
	if(!bmp)
	{
		_XLogError("_XVFReadJPG() - Unable to create bmp !");
		goto done;
	}
	
	/* Allocate a temp buffer to get the image data. */
	if(_rgb_r_shift_24 == 16) 
	{  
		allocbuffer = _XMalloc(U8,bmp->size);
		buffer = allocbuffer;
	}
	else
	{
		buffer = (U8*)bmp->idat;
	}
	if(!buffer)
	{
		_XLogError("_XVFReadJPG() - Unable to create temp buffer !");
		_XDestroyBitmap(bmp);  
		bmp = NULL;
		goto done;
	}
	
	/* Decompress the image */
	jpeg_start_decompress(&cinfo);
	while(cinfo.output_scanline < cinfo.output_height)
	{
		rowptr[0] = (JSAMPROW)(U8*)buffer + cinfo.output_scanline * bmp->bpl;
		jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION)1);
	}
	jpeg_finish_decompress(&cinfo);
	
	/* Copy the temp buffer in the bitmap. */
	if(_rgb_r_shift_24 == 16)
	{
		U8 *dst = (U8*)bmp->idat;
		for(i = 0; i < bmp->size; i+=3)
		{
			dst[i+0] = buffer[i+2];
			dst[i+1] = buffer[i+1];
			dst[i+2] = buffer[i+0];
		}
	}
	
done:
	jpeg_destroy_decompress(&cinfo);
	if(allocbuffer) free(allocbuffer); 
	return bmp;
}

