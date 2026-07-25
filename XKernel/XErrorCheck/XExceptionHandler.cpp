// XExceptionHandler.cpp: implementation of the _XExceptionHandler namespace
//
// Created Date : 2003.08.12
// Author		: Sohyun, Park
// DESC			: Exception Handler
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "xsystem.h"
#include "XExceptionHandler.h"

void _XExceptionHandler::GenerateExceptionInformation(DWORD _code, const void* _address)
{
	_XLog("============================Unhandled Exception Handler============================");

	//  information about exception type
	GenerateExceptionString(_code);

	// information about where the exception occured
	TCHAR		moduleString[1024];
	DWORD		section, offset;
	GetExceptionAddress(_address, moduleString, sizeof(moduleString), section, offset);

	_XLog("Exception Address		:		%08X %02X:%08X\n							%s", _address, section, offset, moduleString);

	_XLog("====================================================================================");

}

void _XExceptionHandler::GenerateExceptionString(DWORD _code)
{
	switch(_code)
	{
	case EXCEPTION_ACCESS_VIOLATION :
		_XLog("Exception code		:		ACCESS_VIOLATION[%08X]", _code);
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT :
		_XLog("Exception code		:		DATATYPE_MISALIGNMENT[%08X]", _code);
		break;
	case EXCEPTION_BREAKPOINT :
		_XLog("Exception code		:		BREAKPOINT[%08X]", _code);
		break;
	case EXCEPTION_SINGLE_STEP :
		_XLog("Exception code		:		SINGLE_STEP[%08X]", _code);
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED :
		_XLog("Exception code		:		ARRAY_BOUNDS_EXCEEDED[%08X]", _code);
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND :
		_XLog("Exception code		:		FLT_DENORMAL_OPERAND[%08X]", _code);
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO :
		_XLog("Exception code		:		FLT_DIVIDE_BY_ZERO[%08X]", _code);
		break;
	case EXCEPTION_FLT_INEXACT_RESULT :
		_XLog("Exception code		:		FLT_INEXACT_RESULT[%08X]", _code);
		break;
	case EXCEPTION_FLT_INVALID_OPERATION :
		_XLog("Exception code		:		FLT_INVALID_OPERATION[%08X]", _code);
		break;
	case EXCEPTION_FLT_OVERFLOW :
		_XLog("Exception code		:		FLT_OVERFLOW[%08X]", _code);
		break;
	case EXCEPTION_FLT_STACK_CHECK :
		_XLog("Exception code		:		FLT_STACK_CHECK[%08X]", _code);
		break;
	case EXCEPTION_FLT_UNDERFLOW :
		_XLog("Exception code		:		FLT_UNDERFLOW[%08X]", _code);
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO :
		_XLog("Exception code		:		INT_DIVIDE_BY_ZERO[%08X]", _code);
		break;
	case EXCEPTION_INT_OVERFLOW :
		_XLog("Exception code		:		INT_OVERFLOW[%08X]", _code);
		break;
	case EXCEPTION_PRIV_INSTRUCTION :
		_XLog("Exception code		:		PRIV_INSTRUCTION[%08X]", _code);
		break;
	case EXCEPTION_IN_PAGE_ERROR :
		_XLog("Exception code		:		IN_PAGE_ERROR[%08X]", _code);
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION :
		_XLog("Exception code		:		ILLEGAL_INSTRUCTION[%08X]", _code);
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION :
		_XLog("Exception code		:		NONCONTINUABLE_EXCEPTION[%08X]", _code);
		break;
	case EXCEPTION_STACK_OVERFLOW :
		_XLog("Error code		:		STACK_OVERFLOW[%08X]", _code);
		break;
	case EXCEPTION_INVALID_DISPOSITION :
		_XLog("Exception code		:		INVALID_DISPOSITION[%08X]", _code);
		break;
	case EXCEPTION_GUARD_PAGE :
		_XLog("Exception code		:		GUARD_PAGE[%08X]", _code);
		break;
	case EXCEPTION_INVALID_HANDLE :
		_XLog("Exception code		:		INVALID_HANDLE[%08X]", _code);
		break;
	default :
		_XLog("Exception code		:		UNKNOWN_ERROR[%08X]", _code);
		break;
	}
}

BOOL _XExceptionHandler::GetExceptionAddress(const void* _address, TCHAR* _modulename, DWORD _len, DWORD& _section, DWORD& _offset)
{
	MEMORY_BASIC_INFORMATION		memoryInfo;

	if(!VirtualQuery(_address, &memoryInfo, sizeof(memoryInfo)))
		return FALSE;

	DWORD hMod = (DWORD)memoryInfo.AllocationBase;

	if(!GetModuleFileName((HMODULE)hMod, _modulename, _len))
		return FALSE;

	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
	
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
	
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );
	
    DWORD rva = (DWORD)_address - hMod;
	
	for( unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++ )
	{
		DWORD sectionStart = pSection->VirtualAddress;
		DWORD sectionEnd = sectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);
		
		if ( (rva >= sectionStart) && (rva <= sectionEnd) )
		{	
			_section = i+1;
			_offset = rva - sectionStart;
			return TRUE;
		}
	}
	
	return FALSE;
}

