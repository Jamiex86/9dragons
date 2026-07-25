
#include "xgamebase.h"

#include <string.h>

#ifdef _X_NO_MEMCMP
int memcmp(const void *s1, const void *s2, size_t num)
{
 long i;

 for(i = 0; i < num; i++)
  if(((unsigned char*) s1)[i] != ((unsigned char*) s2)[i])
   return ((((unsigned char*) s1)[i] < ((unsigned char*) s2)[i]) ? -1 : 1);

 return 0;
}
#endif

#ifdef _X_NO_STRICMP
int stricmp(const TCHAR *s1, const TCHAR *s2)
{
 return strcasecmp(s1, s2);
}
#endif

#ifdef _X_NO_STRLWR
TCHAR *strlwr(TCHAR *string)
{
 TCHAR *p;

 for(p = string; *p != 0; p++)
  *p = utolower(*p);

 return string;
}
#endif

#ifdef _X_NO_STRUPR
TCHAR *strupr(TCHAR *string)
{
 TCHAR *p;

 for(p = string; *p != 0; p++)
  *p = utoupper(*p);

 return string;
}
#endif

#ifdef _X_NO_FINDFIRST

#ifdef HAVE_DIRENT_H
 #include <sys/types.h>
 #include <dirent.h>
 #define NAMLEN(dirent) (strlen((dirent)->d_name))
#else
 #define dirent direct
 #define NAMLEN(dirent) ((dirent)->d_namlen)
 #ifdef HAVE_SYS_NDIR_H
  #include <sys/ndir.h>
 #endif
 #ifdef HAVE_SYS_DIR_H
  #include <sys/dir.h>
 #endif
 #ifdef HAVE_NDIR_H
  #include <ndir.h>
 #endif
#endif

#ifdef TIME_WITH_SYS_TIME
 #include <sys/time.h>
 #include <time.h>
#else
 #ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
 #else
  #include <time.h>
 #endif
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static TCHAR *ff__XGetFilename(char *path)
{
 TCHAR *p = path + strlen(path);

 while((p > path) && (*(p - 1) != '/'))
  p--;

 return p;
}

static void ff__XPutBackslash(TCHAR *filename, int size)
{
 int len = strlen(filename);

 if((len > 0) && (len < (size - 1)) && (filename[len - 1] != '/'))
 {
  filename[len] = '/';
  filename[len + 1] = 0;
 }
}

#define FF_MAXPATHLEN 1024

struct ff_info
{
 int attrib;
 TCHAR dirname[FF_MAXPATHLEN];
 TCHAR pattern[FF_MAXPATHLEN];
 TCHAR tempname[FF_MAXPATHLEN];
 TCHAR filename[FF_MAXPATHLEN];
 DIR *dir;
 struct dirent *entry;
 struct stat stat;
};

static int ff_match(TCHAR *s1, TCHAR *s2);

int findfirst(TCHAR *pattern, struct ffblk *ffblk, int attrib)
{
 struct ff_info *ff_info;
 int ret;

 ff_info = malloc(sizeof(struct ff_info));
 if(!ff_info)
  return (errno = ENOMEM);

 ffblk->ff_info = ff_info;

 ff_info->attrib = attrib;

 ff_info->dirname[0] = 0;
 strncat(ff_info->dirname, pattern, sizeof(ff_info->dirname) - 1);
 *ff__XGetFilename(ff_info->dirname) = 0;
 if(ff_info->dirname[0] == 0)
  strcpy(ff_info->dirname, "./");

 ff_info->pattern[0] = 0;
 strncat(ff_info->pattern, ff__XGetFilename(pattern), sizeof(ff_info->pattern) - 1);

 if(strcmp(ff_info->pattern, "*.*") == 0)
  strcpy(ff_info->pattern, "*");

 ff_info->dir = opendir(ff_info->dirname);

 if(ff_info->dir == 0)
 {
  free(ff_info);
  return ((errno == 0) ? ENOENT : errno);
 }

 ret = findnext(ffblk);

 if(ret)
 {
  closedir(ff_info->dir);
  free(ff_info);
 }

 return ret;
}

int findnext(struct ffblk *ffblk)
{
 struct ff_info *ff_info = ffblk->ff_info;
 struct tm *mtime;

 while(1)
 {
  ff_info->entry = readdir(ff_info->dir);
  if(ff_info->entry == 0)
   return ((errno == 0) ? ENOENT : errno);

  ff_info->tempname[0] = 0;
  if(NAMLEN(ff_info->entry) >= sizeof(ff_info->tempname))
   strncat(ff_info->tempname, ff_info->entry->d_name, sizeof(ff_info->tempname) - 1);
  else
   strncat(ff_info->tempname, ff_info->entry->d_name, NAMLEN(ff_info->entry));
  if(!ff_match(ff_info->tempname, ff_info->pattern))
   continue;

  strcpy(ff_info->filename, ff_info->dirname);
  ff__XPutBackslash(ff_info->filename, sizeof(ff_info->filename));
  strncat(ff_info->filename, ff_info->tempname,
  sizeof(ff_info->filename) - strlen(ff_info->filename) - 1);

  if(stat(ff_info->filename, &(ff_info->stat)))
   continue;

  ffblk->ff_attrib = 0;
  if((ff_info->stat.st_mode & S_IRUSR) == 0)
   ffblk->ff_attrib |= FA_RDONLY;
  if(S_ISDIR(ff_info->stat.st_mode))
   ffblk->ff_attrib |= FA_DIREC;
  if((ff_info->tempname[0] == '.') && ((ff_info->tempname[1] != '.')
	 || (ff_info->tempname[2] != 0)))
   ffblk->ff_attrib |= FA_HIDDEN;

  if((ffblk->ff_attrib & ~ff_info->attrib) != 0)
   continue;

  mtime = gmtime(&(ff_info->stat.st_mtime));
  ffblk->ff_ftime = ((mtime->tm_hour << 11) | (mtime->tm_min << 5)
					 | (mtime->tm_sec >> 1));
  ffblk->ff_fdate = (((mtime->tm_year - 1980) << 9) | (mtime->tm_mon << 5)
					 | (mtime->tm_mday));

  ffblk->ff_fsize = ff_info->stat.st_size;

  ffblk->ff_name[0] = 0;
  strncat(ffblk->ff_name, ff_info->tempname, sizeof(ffblk->ff_name) - 1);

  return 0;
 }
}

void findclose(struct ffblk *ffblk)
{
 struct ff_info *ff_info = ffblk->ff_info;

 if(ff_info)
 {
  if(ff_info->dir != 0)
   closedir(ff_info->dir);

  free(ff_info);
 }
}

#define FF_MATCH_TRY 0
#define FF_MATCH_ONE 1
#define FF_MATCH_ANY 2

struct ff_match_data
{
 int type;
 TCHAR *s1;
 TCHAR *s2;
};

static int ff_match(TCHAR *s1, TCHAR *s2)
{
 static int size = 0;
 static struct ff_match_data *data = 0;
 TCHAR *s1end = s1 + strlen(s1);
 int index, c1, c2;

 if((data != 0) && (size < strlen(s2)))
 {
  free(data);
  data = 0;
 }

 if(data == 0)
 {
  size = strlen(s2);
  data = malloc(sizeof(struct ff_match_data) * size * 2 + 1);
  if(data == 0)
   return 0;
 }

 index = 0;
 data[0].s1 = s1;
 data[0].s2 = s2;
 data[0].type = FF_MATCH_TRY;

 while(index >= 0)
 {
  s1 = data[index].s1;
  s2 = data[index].s2;
  c1 = *s1;
  c2 = *s2;

  switch(data[index].type)
  {
   case FF_MATCH_TRY:
		if(c2 == 0)
		{
		 if(c1 == 0)
		  return 1;
		 else
		  index--;
		}
		else if(c1 == 0)
		{
		 while(*s2 == '*')
		  s2++;
		 if(*s2 == 0)
		  return 1;
		 else
		  index--;
		}
		else if(c2 == '*')
		{
		 data[index++].type = FF_MATCH_ANY;
		 data[index].s1 = s1end;
		 data[index].s2 = s2 + 1;
		 data[index].type = FF_MATCH_TRY;
		}
		else if((c2 == '?') || (c1 == c2))
		{
		 data[index++].type = FF_MATCH_ONE;
		 data[index].s1 = s1 + 1;
		 data[index].s2 = s2 + 1;
		 data[index].type = FF_MATCH_TRY;
		}
		else
		 index--;
		break;

   case FF_MATCH_ONE:
		index--;
		break;

   case FF_MATCH_ANY:
		if(--data[index + 1].s1 >= s1)
		{
		 data[index + 1].type = FF_MATCH_TRY;
		 index++;
		}
		else
		 index--;
		break;

   default:
		return 0;
  }
 }

 return 0;
}

#endif
