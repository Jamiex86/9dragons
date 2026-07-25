
#include "xgamebase.h"
#include "xintern.h"

TCHAR *_XFixFilenameCase(TCHAR *filename)
{
 if(!_XUseLFN)
  strupr(filename);

 return filename;
}

TCHAR *_XFixFilenameSlashes(TCHAR *filename)
{
 int pos;

 for(pos = 0; filename[pos]; pos++)
  if(filename[pos] == '/')
   filename[pos] = OTHER_PATH_SEPARATOR;

 return filename;
}

TCHAR *_XFixFilenamePath(TCHAR *dest, TCHAR *path, int size)
{
 int ch;
 int saved_errno = errno;
 TCHAR buf1[256];
 TCHAR buf2[256];
 int i;

 buf1[0] = '\0';
 strncat(buf1, path, sizeof(buf1) - 1);

 if((buf1[0] == '\0') || (buf1[strlen(buf1)-1] == DEVICE_SEPARATOR))
  strncat(buf1, "./", sizeof(buf1) - strlen(buf1) - 1);

#ifdef DJGPP
 _fixpath(buf1, buf2);
#endif

 ch = buf1[strlen(buf1)-1];
 if ((ch == '/') || (ch == OTHER_PATH_SEPARATOR))
  strncat(buf2, "/", sizeof(buf2) - strlen(buf2) - 1);

 i = 0;

 while (buf2[i])
 {
  if (((buf2[i] == '/') || (buf2[i] == OTHER_PATH_SEPARATOR)) &&
      ((buf2[i+1] == '/') || (buf2[i+1] == OTHER_PATH_SEPARATOR)))
   memmove(buf2+i, buf2+i+1, strlen(buf2+i));
  else
   i++;
 }

 _XFixFilenameCase(buf2);
 _XFixFilenameSlashes(buf2);

 dest[0] = '\0';
 strncat(dest, buf2, size - 1);

 errno = saved_errno;

 return dest;
}

TCHAR *_XReplaceFilename(TCHAR *dest, TCHAR *path, TCHAR *filename, int size)
{
 int pos;
 TCHAR tmp[256];

 pos = strlen(path);

 while ((pos>0) && (path[pos-1] != '/')
        && (path[pos-1] != OTHER_PATH_SEPARATOR)
	    && (path[pos-1] != DEVICE_SEPARATOR))
  pos--;

 tmp[0] = '\0';
 strncat(tmp, path, MIN((int) (sizeof(tmp) - 1), pos));
 strncat(tmp, filename, sizeof(tmp) - strlen(tmp) - 1);

 dest[0] = '\0';
 strncat(dest, tmp, size - 1);

 return dest;
}

TCHAR *_XReplaceExtension(TCHAR *dest, TCHAR *filename, TCHAR *ext, int size)
{
 int pos, end;
 TCHAR tmp[256];

 pos = end = strlen(filename);

 while ((pos>0) && (filename[pos-1] != '.')  && (filename[pos-1] != '/')
	    && (filename[pos-1] != OTHER_PATH_SEPARATOR)
	    && (filename[pos-1] != DEVICE_SEPARATOR))
  pos--;

 if (filename[pos-1] == '.')
  end = pos-1;

 tmp[0] = '\0';
 strncat(tmp, filename, MIN((int) (sizeof(tmp) - 1), end));
 strncat(tmp, ".", sizeof(tmp) - strlen(tmp) - 1);
 strncat(tmp, ext, sizeof(tmp) - strlen(tmp) - 1);

 dest[0] = '\0';
 strncat(dest, tmp, size - 1);

 return dest;
}

TCHAR *_XAppendFilename(TCHAR *dest, TCHAR *path, TCHAR *filename, int size)
{
 int pos;
 TCHAR tmp[256];

 tmp[0] = '\0';
 strncat(tmp, path, sizeof(tmp) - 1);
 pos = strlen(tmp);

 if ((pos > 0) && (pos < ((int)sizeof(tmp) - 1))
     && (tmp[pos-1] != '/')
     && (tmp[pos-1] != OTHER_PATH_SEPARATOR)
     && (tmp[pos-1] != DEVICE_SEPARATOR))
 {
  tmp[pos] = OTHER_PATH_SEPARATOR;
  tmp[pos+1] = '\0';
 }

 strncat(tmp, filename, sizeof(tmp) - strlen(tmp) - 1);

 dest[0] = '\0';
 strncat(dest, tmp, size - 1);

 return dest;
}

TCHAR *_XGetFilename(TCHAR *path)
{
 int pos;

 if (((path[0] >= 65 && path[0] <= 90) || (path[0] >= 97 && path[0] <= 122))
     && (path[1] == ':'))
  path += 2;

 pos = strlen(path);

 while((pos>0) && (path[pos-1] != '/')
	   && (path[pos-1] != OTHER_PATH_SEPARATOR)
	   && (path[pos-1] != DEVICE_SEPARATOR))
  pos--;

 return path+pos;
}

TCHAR *_XPutExtension(TCHAR *filename)
{
 int pos, end;

 pos = end = strlen(filename);

 while ((pos>0) && (filename[pos-1] != '.') && (filename[pos-1] != '/')
	    && (filename[pos-1] != OTHER_PATH_SEPARATOR)
        && (filename[pos-1] != DEVICE_SEPARATOR))
  pos--;

 if (filename[pos-1] == '.')
  return filename+pos;

 return filename+end;
}

void _XPutBackslash(TCHAR *filename)
{
 int i = strlen(filename);

 if ((i<=0) || (filename[i-1] == '/')
     || (filename[i-1] == OTHER_PATH_SEPARATOR)
     || (filename[i-1] == DEVICE_SEPARATOR))
  return;

 filename[i++] = OTHER_PATH_SEPARATOR;
 filename[i] = 0;
}

int _XFileExists(TCHAR *filename, int attrib, int *aret)
{
 int a;

 if(!sys_XFileIsOK(filename))
  return 0;

 if(!sys_XFileExists(filename, attrib, &a))
  return false;

 if(aret)
  *aret = a;

 return true;
}

int _XExists(TCHAR *filename)
{
 return sys_XFileExists(filename, FA_ARCH | FA_RDONLY, NULL);
}

long _XFileSize(TCHAR *filename)
{
 if(!sys_XFileIsOK(filename))
  return 0;

 return sys_XFileSize(filename);
}

long _XFileTime(TCHAR *filename)
{
 if(!sys_XFileIsOK(filename))
  return 0;

 return sys_XFileTime(filename);
}

int _XCopyFile(TCHAR *srcfile, TCHAR *dstfile)
{
 FILE *src, *dst;
 int c;

 src = fopen(srcfile, "rb");
 if(!src)
 {
  _XLogError("_XCopyFile(%s, %s) - Can't open source file !", srcfile, dstfile);
  return 0;
 }

 dst = fopen(dstfile, "wb");
 if(!dst)
 {
  _XLogError("_XCopyFile(%s, %s) - Can't write destination file !", srcfile, dstfile);
  return 0;
 }

 while(!feof(src))
 {
  c = fgetc(src);
  if(fputc(c, dst) != c)
   break;
 }

 fclose(src);
 fclose(dst);

 return 1;
}

int _XDeleteFile(TCHAR *filename)
{
 if(!sys_XFileIsOK(filename))
  return 0;

 return sys_XDeleteFile(filename);
}

int _XCreateDir(TCHAR *dir)
{
 return sys_XCreateDir(dir);
}

int _XDeleteDir(TCHAR *dir)
{
 if(!sys_XFileIsOK(dir))
  return 0;

 return sys_XDeleteDir(dir);
}

int _XForEachFile(TCHAR *name, int attrib, int (*callback)(TCHAR*,int,int), int param)
{
 TCHAR dta_name[512], buf[512];
 void *dta;
 int dta_attrib;
 int c = 0;

 if(!callback)
  return 0;

 if(!sys_XFileIsOK(name))
  return 0;

 dta = sys_XFindFirst(name, attrib, dta_name, &dta_attrib);

 if(!dta)
  return 0;

 do
 {
  _XReplaceFilename(buf, name, dta_name, sizeof(buf));
  if(!(*callback)(buf, dta_attrib, param))
   break;
  c++;
 } while(sys_XFindNext(dta, dta_name, &dta_attrib));

 sys_XFindClose(dta);

 errno = 0;
 return c;
}
