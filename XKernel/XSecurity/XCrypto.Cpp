#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "XKernel.h"
#include "XCrypto.h"

#ifdef _XUSEENCRYPTPATTERN

BYTE g_EncyptPatternTable[256][32];
unsigned char g_SelectedEncryptSequance;

bool _XGetEncryptPatternData( int patternindex, unsigned char sequancepos )
{
	if( patternindex < 0 || patternindex > 255 )
	{
		_XFatalError( "_XCR : Invalid index [%d]", patternindex );
		return false;
	}

	DWORD bytepos = sequancepos / 8;
	DWORD bitpos  = sequancepos % 8;

	return ( (g_EncyptPatternTable[patternindex][bytepos]^0x3E) & (1<<bitpos) ) ? true : false;
}

#endif

#ifdef _XNEWCRYPTO

bool _j_Crypto::Init(FILE* pKeyMapFile)
{
	if( !pKeyMapFile ) return false;

#ifdef _XUSEENCRYPTPATTERN
	long filefirstpos = ftell( pKeyMapFile );
#endif
	
	// 키 파일 읽기
	fread(m_cKeyBox, sizeof(m_cKeyBox), 1, pKeyMapFile) ;

#ifdef _XUSEENCRYPTPATTERN	

	memset( g_EncyptPatternTable, 0, sizeof( 256 * 32 ) );

	fseek( pKeyMapFile, filefirstpos, SEEK_SET );
	fseek( pKeyMapFile, 8192, SEEK_CUR );

	if( fread( g_EncyptPatternTable, 256*32, 1, pKeyMapFile ) < 1 )
	{
		return false;
	}

#endif

	return true ;
}

#else

#ifndef API_CRYPTO_FUNC 

_j_Crypto::_j_Crypto()
{				
	memset(m_cKeyBox,0,sizeof(m_cKeyBox));
	memset(m_buf, 0, sizeof(m_buf)) ;
}

_j_Crypto::~_j_Crypto()
{
	memset(m_cKeyBox,0,sizeof(m_cKeyBox));		
}

// 초기화 (키값 생성)
// pathName : 키 맵으로 사용할 파일 이름..
// 클래스를 생성할깨 초기 좌표값 (x,y) 를 넣어서 초기화해 주세요.
bool _j_Crypto::Init(const char x, const char y, FILE* pKeyMapFile)
{
	if(!GetKey(abs(x)%KEY_X_RANGE, abs(y)%KEY_Y_RANGE, pKeyMapFile)) return false;	//키값 생성(키값 범위를 맞추기 위해 조작)
	return true;
}



//암호화//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 이 함수는 데이타를 암호화 한다.   									//
// 함수 : Encryption(char * pData, USHORT usDataLen)					//
// 설명 : 키와 비트 연산을 하고 대각선형 전치 조합으로 데이타를 혼돈시켰음//
//////////////////////////////////////////////////////////////////////////
bool _j_Crypto::Encryption(char * pData, USHORT usDataLen)
{
			try {		
			
			//memset(m_buf, 0, sizeof(m_buf)) ;					// 임시 버퍼 초기화
			UINT * puiData = (UINT *) pData ;						// 'int *' type data 로 변환		

			j = 0 ;							// line step
			k = usDataLen / KEYVALLENTH ;	// line count
			l = usDataLen % KEYVALLENTH ;	// char count (잔텡이 데이타)
			m = k * 4 ;
		
			// line data block Encrypto
			for( i = 0 ; i < k ; i++, j += 16 ) {	// Key 와 pData 를 전치한다.
				puiData[j+0] = m_uiValKey[0] ^ puiData[j+0] ;
				puiData[j+1] = ~(m_uiValKey[2] ^ puiData[j+1]) ;
				puiData[j+2] = m_uiValKey[1] ^ puiData[j+2] ;
				puiData[j+3] = ~(m_uiValKey[3] ^ puiData[j+3]) ;
			}
			
			// char data Encrypto (잔텡이 데이타)
			//j += KEYVALLENTH ;
			for( h = 0 ; h < l ; h++ ) {
				pData[j+h] = ~(m_cValKey[h] ^ pData[j+h]) ;
			}

			// 대각선 전치 알고리즘..		
			x = y = 0 ;	// x, y 좌표
			for ( z = 0; z < m ; z++ ) 
			{
				
				if ( (x == 0) && (x == k-1) ) {
					m_buf[0] = puiData[0] ; 
					m_buf[1] = puiData[1] ;
					m_buf[2] = puiData[3] ;
					m_buf[3] = puiData[2] ;
					break ;
				}
				else if ( x == 0 ) {			
					if ( (y % 2) == 1 ) {
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
						y--;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						y++;
					}
				}
				else if ( (x == k-1) && (x % 2 == 1) ) {
					if ( y == 3 ) {
						m_buf[z] = puiData[(x*4)+y] ;
					}
					else if ( (y % 2) == 1 ) {
						m_buf[z] = puiData[(x*4)+y] ;
						x--;
						y++;
					}
					else {
						m_buf[z] = puiData[(x*4)+y] ;
						y++;
					}
				}
				else if ( (x == k-1) && (x % 2 == 0) ) {
					if ( y == 3 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
					}
					else if ( (y % 2) == 1 ){				
						m_buf[z] = puiData[(x*4)+y] ;
						y++;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						x--;
						y++;
					}
				}
				else if ( (x % 2) == 1 ) {
					if ( y == 0 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
					}
					else if ( y == 3 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
					}
					else if ( (y % 2) == 1 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x--;
						y++;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
						y--;
					}
				}			
				else {
					if ( (y % 2) == 1 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++ ;
						y-- ;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						x-- ;
						y++ ;
					}
				}
			}
			memcpy( puiData, m_buf, k * KEYVALLENTH ) ;
			
		} // end try
		catch (...) {
			return false ;
		}	
		return true ;	
}



//복호화//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 이 함수는 데이타를 복호화 한다.   									//
// 함수 : Decryption(char * pData, USHORT usDataLen)					//
// 설명 : 키와 비트 연산을 하고 대각선형 전치 조합으로 데이타를 혼돈시켰음//
//////////////////////////////////////////////////////////////////////////
bool _j_Crypto::Decryption(char * pData, USHORT usDataLen)
{
	try {			
			//memset(m_DecBuf, 0, sizeof(m_DecBuf)) ;						// 임시 버퍼 초기화
			UINT * puiData = (UINT *) pData ;						// 'int *' type data 로 변환		

			j = 0 ;							// line step
			k = usDataLen / KEYVALLENTH ;	// line count
			l = usDataLen % KEYVALLENTH ;	// char count (잔텡이 데이타)
			m = k * 4 ;

			// 대각선 전치 알고리즘..		
			x = y = 0 ;	// x, y 좌표
			for ( z = 0; z < m ; z++ ) 
			{
				
				if ( (x == 0) && (x == k-1) ) {
					m_buf[0] = puiData[0] ; 
					m_buf[1] = puiData[1] ;
					m_buf[2] = puiData[3] ;
					m_buf[3] = puiData[2] ;
					break ;
				}
				else if ( x == 0 ) {			
					if ( (y % 2) == 1 ) {
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
						y--;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						y++;
					}
				}
				else if ( (x == k-1) && (x % 2 == 1) ) {
					if ( y == 3 ) {
						m_buf[z] = puiData[(x*4)+y] ;
					}
					else if ( (y % 2) == 1 ) {
						m_buf[z] = puiData[(x*4)+y] ;
						x--;
						y++;
					}
					else {
						m_buf[z] = puiData[(x*4)+y] ;
						y++;
					}
				}
				else if ( (x == k-1) && (x % 2 == 0) ) {
					if ( y == 3 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
					}
					else if ( (y % 2) == 1 ){				
						m_buf[z] = puiData[(x*4)+y] ;
						y++;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						x--;
						y++;
					}
				}
				else if ( (x % 2) == 1 ) {
					if ( y == 0 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
					}
					else if ( y == 3 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
					}
					else if ( (y % 2) == 1 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x--;
						y++;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++;
						y--;
					}
				}			
				else {
					if ( (y % 2) == 1 ) {				
						m_buf[z] = puiData[(x*4)+y] ;
						x++ ;
						y-- ;
					}
					else {				
						m_buf[z] = puiData[(x*4)+y] ;
						x-- ;
						y++ ;
					}
				}
			}
			
			memcpy( puiData, m_buf, k * KEYVALLENTH ) ;
		
			// line data block Encrypto
			for( i = 0 ; i < k ; i++, j += 16 ) {	// Key 와 pData 를 전치한다.
				puiData[j+0] = m_uiValKey[0] ^ puiData[j+0] ;
				puiData[j+1] = ~(m_uiValKey[2] ^ puiData[j+1]) ;
				puiData[j+2] = m_uiValKey[1] ^ puiData[j+2] ;
				puiData[j+3] = ~(m_uiValKey[3] ^ puiData[j+3]) ;
			}
			
			// char data Encrypto (잔텡이 데이타)
			//j += KEYVALLENTH ;
			for( h = 0 ; h < l ; h++ ) {
				pData[j+h] = ~(m_cValKey[h] ^ pData[j+h]) ;
			}
		}// try
		catch (...) {
			return false ;
		}
		return true ;
}



//////////////////////////////////////////////////////////////////////////
// 이 함수는 데이타를 암/복호화 한다.									//
// 함수 : Xor(char * pData, USHORT usDataLen)	인수 : 문자열 포인터(plantext)	//
// 설명 : 문자열을 입력된 키와 단순한 XOR 연산을 수행한다.              //
//////////////////////////////////////////////////////////////////////////
char * _j_Crypto::Xor(char *pData, USHORT usDataLen)	
{	
	for(USHORT i = 0; i < usDataLen; i++){
		pData[i]=~ pData[i]^m_cValKey[i % KEYVALLENTH];
	}
	
	return pData;
}	



bool _j_Crypto::GetKey(const char x, const char y, FILE* pKeyMapFile)	
{	
	//키 파일 읽어오기..
	int i = 0;
	int serverNum = 0;	
	
	fread(&m_cKeyBox[0][0][0], sizeof(char), sizeof(m_cKeyBox), pKeyMapFile) ;		
	// 키 파일 읽기 끝
	
	
	m_cValKey = &m_cKeyBox[x][y][0];
	m_uiValKey = (UINT *)&m_cKeyBox[x][y][0] ;		
	
	return true;	
}


#else

// 클래스를 생성할깨 초기 좌표값 (x,y) 를 넣어서 생성해 주세요..
_j_Crypto::_j_Crypto()
{				
	memset(m_cKeyBox,0,sizeof(m_cKeyBox));			
}
_j_Crypto::~_j_Crypto()
{
	memset(m_cKeyBox,0,sizeof(m_cKeyBox));
	Free();
}

// 초기화 (키값 생성)
// pathName : 키 맵으로 사용할 파일 이름..
bool _j_Crypto::Init(const char x, const char y, FILE* pKeyMapFile)
{
	if(!Cps()) return false; 		//암호화 관련 초기화
	if(!GetKey(abs(x)%KEY_X_RANGE, abs(y)%KEY_Y_RANGE, pKeyMapFile)) return false;	//키값 생성(키값 범위를 맞추기 위해 조작)
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 이 함수는 데이타를 암/복호화 한다.									//
// 함수 : Xor(char * pData, USHORT usDataLen)	인수 : 문자열 포인터(plantext)	//
// 설명 : 문자열을 입력된 키와 단순한 XOR 연산을 수행한다.              //
//////////////////////////////////////////////////////////////////////////
inline char * _j_Crypto::Xor(char *pData, USHORT usDataLen)	
{	
	for(USHORT i = 0; i < usDataLen; i++){
		pData[i]=~ pData[i]^m_cValKey[i % KEYVALLENTH];
	}
	
	return pData;		
}

//암호화//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 이 함수는 데이타를 암호화 한다.   									//
// 함수 : Encryption(char * pData, USHORT usDataLen)						//
// 설명 : 키에대한 암호화는 MD5(hash), RC2(block) 암호화알고리즘 사용	//
//        스트링(데이타) 암호화는 RC2/RC4(block)암호화 알고리즘 사용	//
//////////////////////////////////////////////////////////////////////////
inline bool _j_Crypto::Encryption(char * pData, USHORT usDataLen)	
{	
	DWORD dwDataLen = usDataLen;	
	
	// encrpytion
	if(!CryptEncrypt(hKey,0,true,0,(BYTE *)pData,&dwDataLen,dwDataLen))	return false;    	
	return true;	
}

//복호화/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 이 함수는 데이타를 복호화 한다.  									//
// 함수 : Decryption(char * pData, USHORT usDataLen)					//
// 설명 : 키에대한 암호화는 MD5(hash), RC2(block) 암호화알고리즘 사용	//
//        스트링(데이타) 암호화는 RC2/RC4(block)암호화 알고리즘 사용	//
//////////////////////////////////////////////////////////////////////////
inline bool _j_Crypto::Decryption(char * pData, USHORT usDataLen)	
{
	DWORD dwDataLen = usDataLen;
	
	// decryption	
	if(!CryptDecrypt(hKey, 0, true, 0, (BYTE *)pData, &dwDataLen))	return false;	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 이 함수는 CPS를 생성하고 해쉬오브젝트를 생성하는등 초기화 부분을...	//
// 함수 : Cps()															//
//////////////////////////////////////////////////////////////////////////
bool _j_Crypto::Cps() 
{
	
	if(!CryptAcquireContext(&hProv,NULL,MS_ENHANCED_PROV,PROV_RSA_FULL,0))	{			
		if(!CryptAcquireContext(&hProv,NULL,MS_ENHANCED_PROV,PROV_RSA_FULL,CRYPT_NEWKEYSET))	{
			return false;			
		}			
	}
	return true;
}									//암호화 모듈 초기화


bool _j_Crypto::GetKey(const char x, const char y, FILE* pKeyMapFile)	
{
	
	//키 파일 읽어오기..		
	int i = 0;
	int serverNum = 0;
	
	fread(&m_cKeyBox[0][0][0], sizeof(char), sizeof(m_cKeyBox), pKeyMapFile) ;		
	// 키 파일 읽기 끝
	
	m_cValKey = &m_cKeyBox[x][y][0];
	
	// Create an empty hash object.
	if(!CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)) return false;	
	// Hash the password string.
	if(!CryptHashData(hHash,(BYTE *)m_cValKey,KEYVALLENTH,0)) return false;
	// Create a session key based on the hash of the password.
	if(!CryptDeriveKey(hProv,ENCRYPT_ALGORITHM,hHash,KEYLENGTH,&hKey)) return false;
	
	CryptDestroyHash(hHash); 
	hHash = 0;
	
	return true;
	
}

//////////////////////////////////////////////////////////////////////////
// 이 함수는 해쉬오브젝트, 키, CPS 들의 핸들러를 해제한다..				//
// 함수 : Cps()															//
//////////////////////////////////////////////////////////////////////////
void _j_Crypto::Free()	
{
	// Destroy the hash object.
	if(hHash) 
		CryptDestroyHash(hHash);
	// Destroy the session key.
	if(hKey) 
		CryptDestroyKey(hKey);
	// Release the provider handle.
	if(hProv) 
		CryptReleaseContext(hProv, 0);
}	

#endif


#endif