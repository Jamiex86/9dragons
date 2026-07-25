#include "StdAfx.h"
#include <string.h>

#include "XStringItem.h"

_XStringItem::_XStringItem()
{
    sString = NULL;
    Set_String(_T(" "));
    Set_Attribute(0,0);
	iAttribute3 = 0;
}

_XStringItem::_XStringItem( TCHAR *sDString ,int iAttr1, int iAttr2 )
{
    sString = NULL;
    Set_String(sDString);
    Set_Attribute(iAttr1, iAttr2);
	iAttribute3 = 0;
}

_XStringItem::~_XStringItem()
{ 
	if(sString != NULL)
	{
		delete[] sString; 
		sString = NULL;
	}
}
 

void _XStringItem::Set_String( TCHAR *sDString )
{
    if(sDString == NULL) return;
    iString_Size = strlen(sDString)+1;
	if( iString_Size > 512 ) iString_Size = 512;

	if(sString != NULL)
	{
	   delete[] sString;
	   sString = NULL;
	}

	if( iString_Size > 0 )
	{
		sString = new TCHAR [iString_Size];
		if( sString )
		{
		   memset(sString,0,iString_Size);
		   if( iString_Size > 1 )
		   {
			   strncpy( sString,sDString,iString_Size-1 );
		   }
		}
	}
}

void _XStringItem::Add( _XStringItem *DStr )
{
	int TStrlength = iString_Size;
	
	iString_Size += DStr->Get_Strlength();
	if( iString_Size > 512 ) return;
	
	TCHAR *TStr = NULL;
	
	if( TStrlength > 0 )
		TStr = new TCHAR[ TStrlength ];

	if( !TStr ) return;

	memset(TStr,0,TStrlength);
    strncpy( TStr,sString,TStrlength-1 );
	
	if(sString)
	{
		delete[] sString;
		sString = NULL;
	}

	if( iString_Size > 0 )
	{
		sString = new TCHAR [iString_Size];
		if( sString )
		{
			memset(sString,0,iString_Size);
			if( iString_Size > 1 )
			{
				strncpy( sString,TStr,TStrlength-1);
				strncpy( sString+TStrlength-1,DStr->Get_String(), DStr->Get_Strlength() );
			}
		}
	}
	
	delete TStr;
}


void _XStringItem::Add( TCHAR *DStr )
{
	int TStrlength = iString_Size;
	iString_Size += strlen(DStr);
	if( iString_Size > 512 ) return;

	TCHAR *TStr = NULL;
	if( iString_Size > 0 )
		TStr = new TCHAR[ iString_Size ];

	if( !TStr ) return;

	memset(TStr,0,TStrlength);
    strncpy( TStr,sString,TStrlength-1 );
	
	if(sString)
	{
		delete[] sString;
		sString = NULL;
	}
	if( iString_Size > 0 )
	{
		sString = new TCHAR [iString_Size];
		if( sString != NULL )
		{
			memset(sString,0,iString_Size);
			if( iString_Size > 1 )
			{
				strncpy( sString,TStr,TStrlength-1);
				strncpy( sString+TStrlength-1,DStr, strlen( DStr ) );
			}
		}
	}
	delete TStr;
}

void  _XStringItem::Add( BYTE Charactor )
{
	iString_Size++;
	if( iString_Size > 512 ) return;

	TCHAR *TStr = NULL;

	if( iString_Size-1 > 0 )
	{
		TStr = new TCHAR[ iString_Size-1 ];
	}

	if( !TStr ) return;

	memset(TStr,0,iString_Size-1 );
    strncpy( TStr,sString,iString_Size-2 );
	
	if(sString)
	{
		delete[] sString;
		sString = NULL;
	}
	if( iString_Size > 0 )
	{
		sString = new TCHAR [iString_Size];
		if( sString )
		{
			memset(sString,0,iString_Size);
			if( iString_Size > 1 )
			{
				strncpy( sString,TStr,iString_Size-2);
				sString[ iString_Size-2 ] = Charactor;
			}
		}	
	}
	delete TStr;
}

TCHAR*  _XStringItem::getFilename(void)
{
	int i= strlen( sString );  
	LPSTR Strptr = sString+i;
	do
	{
		 if( *Strptr == '\\' ) break;
	}while( Strptr-- != sString );
	return Strptr+1;

}


void _XStringItem::LShift( void )
{
	 TCHAR tChar = sString[0];

	 UINT i=1;for( ; i < strlen( sString ); i++ )
		  sString[i-1] = sString[i];
	 
	 sString[i-1] = tChar;

}

void _XStringItem::deleteExtention( void )
{	  
	  int strSize = strlen( sString );
	  		  
	  BOOL PeriodCheck = FALSE;
	  while( --strSize )
	  {
			 if( sString[strSize]== '.' )
			 {   
				 PeriodCheck = TRUE;
				 break;
			 }
			 //else sString[strSize]=0;
	  }
	  
	  if( PeriodCheck )
	  {	  
		  sString[strSize+1]=0;
		  
		  iString_Size = strlen(sString);

		  LPSTR tStr = new TCHAR[ iString_Size+1 ];
		  memset( tStr, 0, iString_Size+1 );
		  memcpy( tStr, sString, iString_Size );
		  
		  delete sString;
		  sString = tStr;
	  }	  
}

void _XStringItem::Add( int Char )
{
	TCHAR DStr[11];
	memset( DStr, 0, 11 );
	itoa( Char, DStr, 10 );	

	int TStrlength = iString_Size;
	iString_Size += strlen(DStr);
	if( iString_Size > 512 ) return;

	TCHAR *TStr = NULL;
	if( iString_Size > 0 )
	{
		TStr = new TCHAR[ iString_Size ];
	}
	if( !TStr ) return;

	memset(TStr,0,TStrlength);
    strncpy( TStr,sString,TStrlength-1 );
	
	if(sString)
	{
		delete[] sString;
		sString = NULL;
	}

	if( iString_Size > 0 )
	{
		sString = new TCHAR [iString_Size];
		if( sString != NULL )
		{
			memset(sString,0,iString_Size);
			if( iString_Size > 1 )
			{
				strncpy( sString,TStr,TStrlength-1);
				strncpy( sString+TStrlength-1,DStr, strlen( DStr ) );
			}
		}	
	}
	delete TStr;

}