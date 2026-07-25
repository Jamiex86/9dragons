// XException.cpp: implementation of the _XException class.
//
// Created Date : 2003.08.12
// Author		: Sohyun, Park
// DESC			: Structure Exception Handler Class
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "eh.h"
#include "XException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static void Translator(unsigned, EXCEPTION_POINTERS *info)
{
	throw _XException(*info);
}

_XException::_XException(EXCEPTION_POINTERS const &info) throw()
{
	EXCEPTION_RECORD const &exception = *(info.ExceptionRecord);
	m_Address = exception.ExceptionAddress;
	m_Code = exception.ExceptionCode;
	m_ContextRecord = info.ContextRecord;
}

_XException::~_XException()
{

}

void _XException::Install() throw()
{
	_set_se_translator(Translator);
}

void const *_XException::Where() const throw()
{
	return m_Address;
}

DWORD _XException::What() const throw()
{
	return m_Code;
}
