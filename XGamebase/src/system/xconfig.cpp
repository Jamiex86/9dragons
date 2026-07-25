#include "xgamebase.h"

#define COMMENT_CHAR '#'

_XConfig *_XCreateConfig(int num_entry)
{
 _XConfig *cfg = _XMalloc(_XConfig, 1);
 failed_NULL(!cfg, "_XCreateConfig() - Can't alloc cfg !");

 cfg->num_entry = num_entry;
 cfg->entry = _XMalloc(_XConfigItem, num_entry);
 failed_NULL(!cfg->entry, "_XCreateConfig() - Can't alloc cfg->entry !");

 return cfg;
}

void _XDestroyConfig(_XConfig *cfg)
{
 if(cfg)
 {
  _XFree(cfg->entry);
  free(cfg);
  cfg = NULL; 
 }
}

_XConfig *_XLoadConfig(TCHAR *filename)
{
 _XVFile *rwops = NULL;
 _XConfig *cfg;

 rwops = _XVFOpenFile(filename, "rwt");
 if(!rwops)
 {
  _XLogError("read_config_file(%s) - Can't open file !", filename);
  return NULL;
 }

 cfg = _XVFLoadConfig(rwops);
 if(!cfg)
 {
  _XLogError("read_config_file(%s) - Can't read config !", filename);
  return NULL; 
 }

 return cfg;
}

_XConfig *_XVFLoadConfig(_XVFile *rwops)
{
 _XConfig *cfg;
 TCHAR buf[MAX_CHAR_STRING];
 int i = 0, j = 0;
 int num_entry = 0;

 while(_XVFGetStr(rwops, buf, MAX_CHAR_STRING))
 {
  if((buf[0]) && (buf[0] != COMMENT_CHAR) && (buf[0] != ' '))
  {
   num_entry++;
  }
 }

 cfg = _XCreateConfig(num_entry);
 failed_NULL(!cfg, "read_config_rwops() - Can't alloc config !");

 for(i = 0; i < cfg->num_entry; i++)
 {
  memset(cfg->entry[i].name, '\0', MAX_CHAR_STRING);
  memset(cfg->entry[i].val, '\0', MAX_CHAR_STRING); 
 }

 num_entry = 0;
 _XVFSeekSet(rwops, 0); 
 while(_XVFGetStr(rwops, buf, MAX_CHAR_STRING))
 {
  if((buf[0]) && (buf[0] != COMMENT_CHAR) && (buf[0] != ' '))
  {
   i = j = 0;

   while((buf[i]) && (buf[i] != ' '))
   {
	cfg->entry[num_entry].name[i] = buf[i++];
   }

   while((buf[i]) && (buf[i++] != '"')) ;

   while((buf[i]) && (buf[i]) != '"')
   {
	cfg->entry[num_entry].val[j++] = buf[i++];
   }

   num_entry++; 
  }
 } 

 return cfg;
}

TCHAR *_XGetConfigStr(_XConfig *cfg, char *name, char *def)
{
 int i;

 if(!cfg)
  return def;

 for(i = 0; i < cfg->num_entry; i++)
 {
  if(strcmp(cfg->entry[i].name, name) == 0)
   return cfg->entry[i].val;
 }

 return def;
}


int _XGetConfigInt(_XConfig *config, TCHAR *name, int def)
{
 TCHAR *s;

 s = _XGetConfigStr(config, name, NULL);
 if((s) && (*s))
  return strtol(s, NULL, 0);

 return def;
}


int _XGetConfigHex(_XConfig *config, TCHAR *name, int def)
{
 TCHAR *s = _XGetConfigStr(config, name, NULL);
 int i;

 if((s) && (*s))
 {
  i = strtol(s, NULL, 16);
  if((i == 0x7FFFFFFF) && (stricmp(s,"7FFFFFFF") != 0))
   i = -1;
  return i;
 }

 return def;
}

float _XGetConfigFloat(_XConfig *config, TCHAR *name, float def)
{
 TCHAR *s = _XGetConfigStr(config, name, NULL);

 if((s) && (*s))
  return atof(s);

 return def;
}


bool _XGetConfigBool(_XConfig *config, TCHAR *name, bool def)
{
 TCHAR *s = _XGetConfigStr(config, name, NULL);

 if((s) && (*s))
 {
  if(strnicmp(s, "true", 4) == 0)
	  return true;
  if(strnicmp(s, "false", 4) == 0)
	  return false;
 }

 return def;
}

int _XGetConfigID(_XConfig *config, TCHAR *name, int def)
{
 TCHAR *s = _XGetConfigStr(config, name, NULL);
 TCHAR tmp[4];
 TCHAR *endp;
 int val, i;

 if((s) && (*s))
 {
  val = strtol((TCHAR*)s, (TCHAR**)&endp, 0);
  if(!(*endp))
   return val;

  tmp[0] = tmp[1] = tmp[2] = tmp[3] = ' ';

  for(i = 0; i < 4; i++)
  {
   if(s[i])
	tmp[i] = toupper(s[i]);
   else
	break;
  }

  return _XID(tmp[0], tmp[1], tmp[2], tmp[3]);
 }

 return def;
}

void _XWriteConfigStr(_XVFile *rwops, TCHAR *name, TCHAR *val)
{
 _XVFPutStr(rwops, name);
 _XVFPut8(rwops, ' ');
 _XVFPut8(rwops, '"');
 _XVFPutStr(rwops, val);
 _XVFPut8(rwops, '"');
 _XVFPut8(rwops, '\n');
}

void _XWriteConfigInt(_XVFile *rwops, TCHAR *name, int val)
{
 TCHAR buf[32];
 sprintf(buf, "%d", val);
 _XWriteConfigStr(rwops, name, buf);
}

void _XWriteConfigHex(_XVFile *rwops, TCHAR *name, int val)
{
 TCHAR buf[32];
 if(val >= 0)
 {
  sprintf(buf, "0x%X", val);
  _XWriteConfigStr(rwops, name, buf);
 }
 else
 {
  _XWriteConfigStr(rwops, name, "-1");
 }
}

void _XWriteConfigFloat(_XVFile *rwops, TCHAR *name, float val)
{
 TCHAR buf[32];
 sprintf(buf, "%f", val);
 _XWriteConfigStr(rwops, name, buf);
}

void _XWriteConfigID(_XVFile *rwops, TCHAR *name, int val)
{
 TCHAR buf[32];
 int v[4];
 int pos = 0;
 int i;

 if(val < 256)
 {
  sprintf(buf, "%d", val);
 }
 else
 {
  v[3] = (val>>24)&0xFF;
  v[2] = (val>>16)&0xFF;
  v[1] = (val>>8)&0xFF;
  v[0] = val&0xFF;

  for(i = 0; (i < 4) && (v[i]) && (v[i] != ' '); i++)
  {
   buf[pos++] = v[i];
  }

  buf[pos] = 0;
 }

 _XWriteConfigStr(rwops, name, buf);
}

void _XWriteConfigBool(_XVFile *rwops, TCHAR *name, bool val)
{
 if(val == true)
	_XWriteConfigStr(rwops, name, "true");
 else
	_XWriteConfigStr(rwops, name, "false");
}
