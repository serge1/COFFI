//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: EXEDUMP.H
//==================================

void DumpExeFile( PIMAGE_DOS_HEADER dosHeader );
template <class T> void DumpOptionalHeader(T* pImageOptionalHeader);	// 'T' is IMAGE_OPTIONAL_HEADER32/64
