#pragma once


/*
암호화 모듈이 두가지 있습니다.
API 를 사용한 암호화 모듈과.. OS 독립적인 모듈이 있습니다.
아래 '//#define API_CRYPTO_FUNC' 주석을 제거하면..
API를 이용한 암호화 모듈로 컴파일 됩니다.
*/

//#define API_CRYPTO_FUNC


#ifndef API_CRYPTO_FUNC 
// API 암호화 함수 사용하지 않을경우


#define KEYBOX_FILE			"lump.dat"	// 키박스 파일 입니다.
#define PACKET_HEADER_LEN	1			// 패킷의 헤더 길이(설명 : 패킷에서 헤더 부분(길이정보)은 암호화 하지 않습니다.)

#define KEYVALLENTH			16			// 랜덤생성키의 최대 크기
#define KEY_X_RANGE			100			// x 키값 범위 (0~99) : 범위를 넘지 말아야함..
#define KEY_Y_RANGE			100			// y 키값 범위 (0~99)


/*================================================================================
 CLASS NAME : _j_Crypto 
 EXP		: 데이타 암호화 / 복호화
 AUTHOR		: ozzywow
 DATE		: 2003.11.
=================================================================================*/


class _j_Crypto
{

protected:

	CRITICAL_SECTION	cs ;
	
	DWORD	m_dwKeyLength;									// Key 의 길이	
	char m_cKeyBox[KEY_X_RANGE][KEY_Y_RANGE][KEYVALLENTH];	// Key 들이 들어있는 BOX
	char * m_cValKey;										// 지금 키
	UINT * m_uiValKey ;

	UINT m_buf[2048] ;

	int x,y,z, h,i,j,k,l,m ;

public:		
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
	bool _j_Crypto::Init(const char x, const char y, const char * pathName)
	{
		if( InitializeCriticalSectionAndSpinCount( &cs, (0xf0000000)|2000 )  == FALSE ) {
			printf( " cs Error Critical Section\n" ) ;
			return false ;
		}
		
		if(!GetKey(abs(x)%KEY_X_RANGE, abs(y)%KEY_Y_RANGE, pathName)) return false;	//키값 생성(키값 범위를 맞추기 위해 조작)
		return true;
	}
	

	//암호화//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 이 함수는 데이타를 암호화 한다.   									//
	// 함수 : Encryption(char * pData, USHORT usDataLen)						//
	// 설명 : 키와 비트 연산을 하고 대각선형 전치 조합으로 데이타를 혼돈시켰음//
	//////////////////////////////////////////////////////////////////////////
	inline bool _j_Crypto::Encryption(char * pData, USHORT usDataLen)
	{
		try {
			memset(m_buf, 0, sizeof(m_buf)) ;						// 임시 버퍼 초기화
			UINT * puiData = (UINT *) pData ;						// 'int *' type data 로 변환		

			j = 0 ;							// line step
			k = usDataLen / KEYVALLENTH ;	// line count
			l = usDataLen % KEYVALLENTH ;	// char count (잔텡이 데이타)
			m = k * 4 ;
		
			// line data block Encrypto
			for( i = 0 ; i < k ; i++, j += 4 ) {	// Key 와 pData 를 전치한다.
				puiData[j+0] = m_uiValKey[0] ^ puiData[j+0] ;
				puiData[j+1] = ~(m_uiValKey[2] ^ puiData[j+1]) ;
				puiData[j+2] = m_uiValKey[1] ^ puiData[j+2] ;
				puiData[j+3] = ~(m_uiValKey[3] ^ puiData[j+3]) ;
			}
			
			// char data Encrypto (잔텡이 데이타)
			j += KEYVALLENTH ;
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
	// 함수 : Decryption(char * pData, USHORT usDataLen)						//
	// 설명 : 키와 비트 연산을 하고 대각선형 전치 조합으로 데이타를 혼돈시켰음//
	//////////////////////////////////////////////////////////////////////////
	inline bool _j_Crypto::Decryption(char * pData, USHORT usDataLen)
	{
		try {
			memset(m_buf, 0, sizeof(m_buf)) ;						// 임시 버퍼 초기화
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
		for( i = 0 ; i < k ; i++, j += 4 ) {	// Key 와 pData 를 전치한다.
			puiData[j+0] = m_uiValKey[0] ^ puiData[j+0] ;
			puiData[j+1] = ~(m_uiValKey[2] ^ puiData[j+1]) ;
			puiData[j+2] = m_uiValKey[1] ^ puiData[j+2] ;
			puiData[j+3] = ~(m_uiValKey[3] ^ puiData[j+3]) ;
		}
		
		// char data Encrypto (잔텡이 데이타)
		j += KEYVALLENTH ;
		for( h = 0 ; h < l ; h++ ) {
			pData[j+h] = ~(m_cValKey[h] ^ pData[j+h]) ;
		}
		}
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
	inline char * _j_Crypto::Xor(char *pData, USHORT usDataLen)	
	{	
		for(USHORT i = 0; i < usDataLen; i++){
			pData[i]=~ pData[i]^m_cValKey[i % KEYVALLENTH];
		}

		return pData;		
	}	


protected:
	bool _j_Crypto::GetKey(const char x, const char y, const char * pathName)	
	{
		
		//키 파일 읽어오기..
		FILE *	stream = NULL;
		try{
			stream = fopen(pathName,"r");
		}
		catch (...) {		
			fclose(stream);
			return false;
		}

		if(stream == NULL) {
			fclose(stream);
			return false;
		}

		int i = 0;
		int serverNum = 0;

		//while(!feof(stream)) {
		//	if(fread(&m_cKeyBox[0][0][0], sizeof(char), sizeof(m_cKeyBox), stream) == NULL) ;//break;
		//}
		fread(&m_cKeyBox[0][0][0], sizeof(char), sizeof(m_cKeyBox), stream) ;

		if(fclose(stream) != 0) return false;

		// 키 파일 읽기 끝


		m_cValKey = &m_cKeyBox[x][y][0];
		m_uiValKey = (UINT *)&m_cKeyBox[x][y][0] ;		

		return true;

	}

private:
};


#else

#include <Wincrypt.h>


/* If a 128-bit RC4 session key is to be generated, 
the value 0x00800000 is combined with any other dwFlags 
predefined value with a bitwise OR operation. */
//#define _WIN32_WINNT 0x0400 
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#define KEYLENGTH			0x00800000	
#define ENCRYPT_ALGORITHM	CALG_RC4	//암호화 알고리즘 종류
#define KEYBOX_FILE			"lump.dat"	// 키박스 파일 입니다.
#define PACKET_HEADER_LEN	1			// 패킷의 헤더 길이(설명 : 패킷에서 헤더 부분(길이정보)은 암호화 하지 않습니다.)

#define KEYVALLENTH			10			// 랜덤생성키의 최대 크기
#define KEY_X_RANGE			100			// x 키값 범위 (0~99) : 범위를 넘지 말아야함..
#define KEY_Y_RANGE			100			// y 키값 범위 (0~99)


/*================================================================================
 CLASS NAME : _j_Crypto 
 EXP		: 데이타 암호화 / 복호화
 AUTHOR		: ozzywow
 DATE		: 2003.11.
=================================================================================*/


class _j_Crypto
{

protected:
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	HCRYPTKEY hKey;

	DWORD	m_dwKeyLength;						//Key 의 길이	
	char m_cKeyBox[KEY_X_RANGE][KEY_Y_RANGE][KEYVALLENTH];
	char * m_cValKey;

public:	
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
	bool _j_Crypto::Init(const char x, const char y, const char * pathName)
	{
		if(!Cps()) return false; 		//암호화 관련 초기화
		if(!GetKey(abs(x)%KEY_X_RANGE, abs(y)%KEY_Y_RANGE, pathName)) return false;	//키값 생성(키값 범위를 맞추기 위해 조작)
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


protected:
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

	bool _j_Crypto::GetKey(const char x, const char y, const char * pathName)	
	{
		
		//키 파일 읽어오기..
		FILE *	stream = NULL;
		try{
			stream = fopen(pathName,"r");
		}
		catch (...) {		
			fclose(stream);
			return false;
		}

		if(stream == NULL) {
			fclose(stream);
			return false;
		}

		int i = 0;
		int serverNum = 0;

		//while(!feof(stream)) {
		//	if(fread(&m_cKeyBox[0][0][0], sizeof(char), sizeof(m_cKeyBox), stream) == NULL) ;//break;
		//}
		fread(&m_cKeyBox[0][0][0], sizeof(char), sizeof(m_cKeyBox), stream) ;

		if(fclose(stream) != 0) return false;

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

private:
};




#endif