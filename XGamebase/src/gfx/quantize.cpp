#include <stdlib.h>
#include <string.h>

#include "xgamebase.h"

#define DEFAULT_PREC       5	// default: 4
#define DEFAULT_FRACTION   5	// default: 5
#define DEFAULT_MAXSWAPS   12	// default: 16
#define DEFAULT_MINDIFF    9	// default: 9


/* the number of lists, prime number */
#define HASHTABLESIZE      1031


typedef struct NODE {
	struct NODE *next;
	int color, count;
} NODE;


typedef struct {
	int color, key;
} ITEM;


static int distinct;
static NODE *hash_table;



static void delete_list(NODE *list)
{
	NODE *node, *next;
	
	for (node = list; node != NULL; node = next) {
		next = node->next;
		free (node);
	}
}



static void insert_node(int color)
{
	NODE *p;
	
	p = &hash_table[color % HASHTABLESIZE];
	
	for (;;) {
		if (p->color == color) {
			/* this node (e.g. the color was already filled) */
			p->count++;
			return;
		}
		if (p->next)
			p = p->next;
		else
			break;
	}
	
	/* new color */
	if (p->count) {
		p->next = (NODE*)malloc(sizeof(NODE));
		p = p->next;
	}
	if (p != NULL) {
		p->color = color;
		p->count = 1;
		p->next = NULL;
		distinct++;
	}
}



/* helper function for builtin qsort function */
static int qsort_helper_ITEM(const void *e1, const void *e2)
{
	return ((ITEM *)e2)->key - ((ITEM *)e1)->key;
}



/* compare two color values */
static inline int compare_cols(int col1, int col2)
{
	int b = ((col1 >> 16) & 0xFF) - ((col2 >> 16) & 0xFF);
	int g = ((col1 >> 8) & 0xFF) - ((col2 >> 8) & 0xFF);
	int r = ((col1 & 0xFF) - (col2 & 0xFF));
	return (((r < 0) ? -r : r) + ((g < 0) ? -g : g) + ((b < 0) ? -b : b));
}


static void optimize_colors(ITEM *array, int item, int palsize, int length, int mindiff)
{
	int i, j, best, curbest, bestpos, t;
	
	/* iterate through the array comparing any item behind 'item' */
	for (i=item; i<length; i++) {
		
		/* with all item in front of 'item' */
		for (j=0, curbest=1000; j<item; j++) {
			t = compare_cols(array[i].color, array[j].color);
			
			/* finding minimum difference (maximal to all used colors) */
			if (t < curbest) {
				curbest = t;
				if (t < mindiff)
					break;
			}
		}
		
		/* filling that minimum to 'key' field */
		array[i].key = curbest;
	}
	
	/* sort the array begind 'item' according to 'key' field */
	qsort(array + item, length - item, sizeof(ITEM), qsort_helper_ITEM);
	
	/* find the start of small values ('key') in array and safely reducing
    * the number of items we'll work with 
    */
	for (i = item; i < length; i++) {
		if (array[i].key < mindiff) {
			length = i;
			break;
		}
	}
	
	/* the most different color (from colors in [0,item)) */
	bestpos = item;
	best = array[item].key;
	
	/* swapping loop (the length goes from the size of palette) */
	for (i=item; i<palsize; i++) {
		/* the 'i'th best is already known */
		if (best < mindiff) {
			return;
		}
		else {
			int tmp;
			
			/* swap the focused color and the one with 'bestpos' (the most
			* different) index
			*/
			tmp = array[bestpos].color;
			array[bestpos] = array[i];
			array[i].color = tmp;
			
			/* fix the keys (can be only diminished with the last added color) */
			for (j=i+1, best=-1; j<length; j++) {
				t = compare_cols(array[i].color, array[j].color);
				if (t < array[j].key) {
					array[j].key = t;
				}
				/* find the maximum for swapping */
				if (array[j].key > best) {
					best = array[j].key;
					bestpos = j;
				}
			}
		}
	}
}



/* searches the array of length for the color */
static inline int no_such_color(ITEM *array, int length, int color, int mask)
{
	int i;
	
	for (i=0; i<length; i++)
		if ((array[i].color & mask) == color)
			return 0;
		
		return 1;
}



/* copy color to palette */
static inline void copy_color(_XRGB *rgb, int color)
{
	rgb->r = (color & 0xFF);
	rgb->g = (color >> 8) & 0xFF;
	rgb->b = (color >> 16) & 0xFF;
}



int _XGenerateOptimizedPalette(_XBitmap *image, _XPalette pal,
								  signed char *rsvdcols, int bitsperrgb,
								  int fraction, int maxswaps, int mindiff)
{
	int i, j, x, y, imgdepth, numcols, palsize, rsvdcnt=0, rsvduse=0;
	unsigned int prec_mask, prec_mask2, bitmask15, bitmask16, bitmask24;
	signed char tmprsvd[256];
	int rshift, gshift, bshift;
	ITEM *colors;
	
	switch (bitsperrgb) {
		
	case 4:
		prec_mask = 0x3C3C3C;
		prec_mask2 = 0;
		bitmask15 = 0x7BDE;    /* 0111 1011 1101 1110 */
		bitmask16 = 0xF79E;    /* 1111 0111 1001 1110 */
		bitmask24 = 0xF0F0F0;
		break;
		
	case 5:
		prec_mask = 0x3E3E3E;
		prec_mask2 = 0x3C3C3C;
		bitmask15 = 0x7FFF;    /* 0111 1111 1111 1111 */
		bitmask16 = 0xFFDF;    /* 1111 1111 1101 1111 */
		bitmask24 = 0xF8F8F8;
		break;
		
	default:
		return -1;
	}
	
	distinct = 0;
	
	imgdepth = image->bpp;
	if (imgdepth == 8)
		return 0;
	
	hash_table = (NODE*)malloc(HASHTABLESIZE * sizeof(NODE));
	if (hash_table == NULL)
		return 0;
	
	for (i = 0; i < HASHTABLESIZE; i++) {
		hash_table[i].next = NULL;
		hash_table[i].color = -1;
		hash_table[i].count = 0;
	}
	
	/* count the number of colors we shouldn't modify */
	if (rsvdcols) {
		for (i=0; i<256; i++) {
			if (rsvdcols[i]) {
				rsvdcnt++;
				if (rsvdcols[i] > 0)
					rsvduse++;
			}
		}
	}
	else {
		pal[0].r = 63;
		pal[0].g = 0;
		pal[0].b = 63;
		
		tmprsvd[0] = -1;
		rsvdcnt++;
		
		for (i=1; i<256; i++)
			tmprsvd[i] = 0;
		
		rsvdcols = tmprsvd;
	}
	
	/* fix palette */
	for (i = 0; i < 256; i++) {
		pal[i].r &= 63;
		pal[i].g &= 63;
		pal[i].b &= 63;
	}
	
	/* fill the 'hash_table' with 4bit per RGB color values */
	switch (imgdepth) {
		
	case 32:
		for (y=0; y<image->h; y++)
			for (x=0; x<image->w; x++)
				insert_node(_XRead32((unsigned long)image->line[y]+x*sizeof(long)) & bitmask24);
			break;
			
	case 24:
		for (y=0; y<image->h; y++)
			for (x=0; x<image->w; x++)
				insert_node(_XRead24((unsigned long)image->line[y]+x*3) & bitmask24);
			break;
			
	case 16:
		for (y=0; y<image->h; y++)
			for (x=0; x<image->w; x++)
				insert_node(_XRead16((unsigned long)image->line[y]+x*sizeof(short)) & bitmask16);
			break;
			
	case 15:
		for (y=0; y<image->h; y++)
			for (x=0; x<image->w; x++)
				insert_node(_XRead15((unsigned long)image->line[y]+x*sizeof(short)) & bitmask15);
			break;
			
	default:
		return -1;
	}
	
	/* convert the 'hash_table' to array 'colors' */
	colors = (ITEM*)malloc((rsvduse + distinct) * sizeof(ITEM));
	if (colors == NULL) {
		free(hash_table);
		return 0;
	}
	
	for (i = 0, j = rsvduse; i<HASHTABLESIZE; i++) {
		if (hash_table[i].count) {
			NODE *node = &hash_table[i];
			
			do {
				colors[j].color = node->color;
				colors[j++].key = node->count;
				node = node->next;
			} while (node != NULL);
			
			if (hash_table[i].next)
				delete_list(hash_table[i].next);
		}
	}
	
	free(hash_table);
	
	/* sort the list with biggest count first */
	qsort(colors + rsvduse, distinct, sizeof(ITEM), qsort_helper_ITEM);
	
	/* we don't want to deal anymore with colors that are seldomly(?) used */
	numcols = rsvduse + distinct;
	palsize = 256 - rsvdcnt + rsvduse;
	
	/* change the format of the color information to some faster one
    * (in fact to the 00BBBB?0 00GGGG?0 00RRRR?0).
    */
	
	switch (imgdepth) {
		
	case 32:
		rshift = _rgb_r_shift_32 + 3;
		gshift = _rgb_g_shift_32 + 3;
		bshift = _rgb_b_shift_32 + 3;
		break;
		
	case 24:
		rshift = _rgb_r_shift_24 + 3;
		gshift = _rgb_g_shift_24 + 3;
		bshift = _rgb_b_shift_24 + 3;
		break;
		
	case 16:
		rshift = _rgb_r_shift_16;
		gshift = _rgb_g_shift_16 + 1;
		bshift = _rgb_b_shift_16;
		break;
		
	case 15:
		rshift = _rgb_r_shift_15;
		gshift = _rgb_g_shift_15;
		bshift = _rgb_b_shift_15;
		break;
		
	default:
		return -1;
	}
	
	for (i = rsvduse; i < numcols; i++) {
		int r = (colors[i].color >> rshift) & 0x1F;
		int g = (colors[i].color >> gshift) & 0x1F;
		int b = (colors[i].color >> bshift) & 0x1F;
		colors[i].color = ((r << 1) | (g << 9) | (b << 17));
	}
	
	do {
		int start, k;
		
		/* there may be only small number of numcols colors, so we don't need
		* any optimization
		*/
		if (numcols <= palsize)
			break;
		
		if (rsvduse > 0) {
			/* copy 'rsvd' to the 'colors' */
			for (i = 0, j = 0; i < rsvduse; j++)
				if (rsvdcols[j] > 0)
					colors[i++].color = (pal[j].r | (pal[j].g << 8) | (pal[j].b << 16));
				
				/* reduce 'colors' skipping colors contained in 'rsvd' palette */
				for (i = rsvduse, j = i; i < numcols; i++)
					if (no_such_color(colors, rsvduse, colors[i].color, prec_mask))
						colors[j++].color = colors[i].color;
					
					/* now there are j colors in 'common'  */
					numcols = j;
					
					/* now there might be enough free cells in palette */
					if (numcols <= palsize)
						break;
		}
		
		/* from 'start' will start swapping colors */
		start = palsize - palsize / fraction;
		
		/* it may be slow, so don't let replace too many colors */
		if (start < (palsize - maxswaps))
			start = palsize - maxswaps;
		
		/* swap not less than 10 colors */
		if (start > (palsize - 10))
			start = rsvduse;
		
		/* don't swap reserved colors */
		if (start < rsvduse)
			start = rsvduse;
		
		if (bitsperrgb == 5) {
		/* do second pass on the colors we'll possibly use to replace (lower
		bits per pixel to 4) - this would effectively lower the maximum
			number of different colors to some 4000 (from 32000) */
			for (i = start, k = i; i < numcols; i++) {
				for (j = 0; j < k; j++) {
					if ((colors[j].color & prec_mask2) == (colors[i].color & prec_mask2)) {
						j = -1;
						break;
					}
				}
				/* add this color if there is not similar one */
				if (j != -1)
					colors[k++].color = colors[i].color;
			}
			
			/* now there are k colors in 'common' */
			numcols = k;
			
			/* now there might be enough free cells in palette */
			if (numcols <= palsize)
				break;
		}
		
		/* start finding the most different colors */
		optimize_colors(colors, start, palsize, numcols, mindiff);
		
		numcols = palsize;
	} while (0);
	
	/* copy used colors to 'pal', skipping 'rsvd' */
	for (i = rsvduse, j = 0; i < numcols; j++)
		if (!rsvdcols[j])
			copy_color(&pal[j], colors[i++].color);
		
	free(colors);

	for(i = 0; i < 256; i++)
	{
		pal[i].r = ((pal[i].r+1)<<2)-1;
		pal[i].g = ((pal[i].g+1)<<2)-1;
		pal[i].b = ((pal[i].b+1)<<2)-1;
	}
		
	return distinct;
}



int _XGenerateOptimizedPalette(_XBitmap *image, _XPalette pal,
							   signed char *rsvdcols)
{
	return _XGenerateOptimizedPalette(image, pal, rsvdcols, DEFAULT_PREC, DEFAULT_FRACTION, DEFAULT_MAXSWAPS, DEFAULT_MINDIFF);
}
