#include "xgamebase.h"

char *uascii(char *s, ...)
{
	static char strTemp[1024];
	DOUVSPRINTF(s);
	unchar *up = ustrVBuffer;
	char *p = strTemp;
	while(*up)
	{
		unchar c = *(up++);
		*(p++) = (char)(c)&0xFF;
	}

	return strTemp;
}

int uinsert(unchar *s, int index, int c)
{
	s += index*sizeof(unchar);
	memmove(s+sizeof(unchar), s, ustrsizez(s));
	*s = (unchar)c;
	
	return sizeof(unchar);
}

int uremove(unchar *s, int index)
{
	s += index*sizeof(unchar);
	memmove(s, s+sizeof(unchar), ustrsizez(s+sizeof(unchar)));
	
	return -(int)sizeof(unchar);
}

int ustrsize(unchar *s)
{	
	int size = 0;
	while(*(s++))
		size += sizeof(unchar);	
	
	return size;
}

int ustrsizez(unchar *s)
{
	return ustrsize(s)+sizeof(unchar);
}

double uatof(unchar *s)
{
	unsigned char *buf = new unsigned char[ustrsizez(s)];
	unsigned char *tmp = buf;

	while(*s)
		*(tmp++) = (unsigned char)*(s++);
	*tmp = 0;

	double ret = atof((const char*)tmp);

	delete[] buf;

	return ret;
}

