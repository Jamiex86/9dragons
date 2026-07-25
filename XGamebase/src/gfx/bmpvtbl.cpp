#include "xsystem.h"
#include "xgfx.h"
#include "xinline.h"
#include "xintern.h"

int le_get_pixel(_XBitmap *p1, int p2, int p3)
{
	return 0;
}

void le_line(_XBitmap *p1, int p2, int p3, int p4, int p5)
{
}

void le_clear_to_color(_XBitmap *p1, int p2)
{
}

void le_put_pixel(_XBitmap *p1, int p2, int p3, int p4)
{
}

void le_blit(_XBitmap *p1, _XBitmap *p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
}

_XBitmapVTable bmpvtbl_empty =
{
 0, 0,

 le_put_pixel,
 le_get_pixel,
 le_line,
 le_line,
 le_clear_to_color,

 le_blit,
 le_blit,
};

_XBitmapVTable bmpvtbl_8bit_linear =
{
 8, MASK_COLOR_8,

 l8_put_pixel,
 l8_get_pixel,
 l8_vline,
 l8_hline,
 l8_clear_to_color,

 l8_blit,
 l8_masked_blit,
};

_XBitmapVTable bmpvtbl_12bit_linear =
{
 12, MASK_COLOR_12,

 l12_put_pixel,
 l12_get_pixel,
 l12_vline,
 l12_hline,
 l12_clear_to_color,

 l12_blit,
 l12_masked_blit,
};

_XBitmapVTable bmpvtbl_15bit_linear =
{
 15, MASK_COLOR_15,

 l15_put_pixel,
 l15_get_pixel,
 l15_vline,
 l15_hline,
 l15_clear_to_color,

 l15_blit,
 l15_masked_blit,
};

_XBitmapVTable bmpvtbl_16bit_linear =
{
 16, MASK_COLOR_16,

 l16_put_pixel,
 l16_get_pixel,
 l16_vline,
 l16_hline,
 l16_clear_to_color,

 l16_blit,
 l16_masked_blit,
};

_XBitmapVTable bmpvtbl_24bit_linear =
{
 24, MASK_COLOR_24,

 l24_put_pixel,
 l24_get_pixel,
 l24_vline,
 l24_hline,
 l24_clear_to_color,

 l24_blit,
 l24_masked_blit,
};

_XBitmapVTable bmpvtbl_32bit_linear =
{
 32, MASK_COLOR_32,

 l32_put_pixel,
 l32_get_pixel,
 l32_vline,
 l32_hline,
 l32_clear_to_color,

 l32_blit,
 l32_masked_blit,
};

