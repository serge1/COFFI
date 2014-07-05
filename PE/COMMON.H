//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: COMMON.H
//==================================

// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))

void DumpHeader(PIMAGE_FILE_HEADER pImageFileHeader);
void DumpSectionTable(PIMAGE_SECTION_HEADER section,
                      unsigned cSections,
                      BOOL IsEXE);
LPVOID GetSectionPtr(PSTR name, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase);

void DumpRawSectionData(PIMAGE_SECTION_HEADER section,
                        PVOID base,
                        unsigned cSections);
void DumpDebugDirectory(PIMAGE_DEBUG_DIRECTORY debugDir, DWORD size, PBYTE pImageBase );
void DumpCOFFHeader(PIMAGE_COFF_SYMBOLS_HEADER pDbgInfo);
void HexDump(PBYTE ptr, DWORD length);

bool IsValidMachineType( WORD wMachineType ); 

PSTR GetMachineTypeName( WORD wMachineType );

#define GetImgDirEntryRVA( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)

#define GetImgDirEntrySize( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

typedef struct
{
    WORD    flag;
    PSTR    name;
} WORD_FLAG_DESCRIPTIONS;

typedef struct
{
    DWORD   flag;
    PSTR    name;
} DWORD_FLAG_DESCRIPTIONS;

typedef struct
{
	WORD	wValue;
	PSTR	pszName;
} WORD_VALUE_NAMES;

#define ARRAY_SIZE( x ) (sizeof(x) / sizeof(x[0]))

//================================================================================
//
// Given an RVA, look up the section header that encloses it and return a
// pointer to its IMAGE_SECTION_HEADER
//
template <class T> PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, T* pNTHeader)	// 'T' == PIMAGE_NT_HEADERS 
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		DWORD size = section->Misc.VirtualSize;
		if ( 0 == size )
			size = section->SizeOfRawData;
			
        // Is the RVA within this section?
        if ( (rva >= section->VirtualAddress) && 
             (rva < (section->VirtualAddress + size)))
            return section;
    }
    
    return 0;
}

template <class T> LPVOID GetPtrFromRVA( DWORD rva, T* pNTHeader, PBYTE imageBase ) // 'T' = PIMAGE_NT_HEADERS 
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;
		
	pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
	if ( !pSectionHdr )
		return 0;

	delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
	return (PVOID) ( imageBase + rva - delta );
}

//
// Like GetPtrFromRVA, but works with addresses that already include the
// default image base
//
template <class T> LPVOID GetPtrFromVA( PVOID ptr, T* pNTHeader, PBYTE pImageBase ) // 'T' = PIMAGE_NT_HEADERS 
{
	// Yes, under Win64, we really are lopping off the high 32 bits of a 64 bit
	// value.  We'll knowingly believe that the two pointers are within the
	// same load module, and as such, are RVAs
	DWORD rva = PtrToLong( (PBYTE)ptr - pNTHeader->OptionalHeader.ImageBase );
	
	return GetPtrFromRVA( rva, pNTHeader, pImageBase );
}

//
// Given a section name, look it up in the section table and return a
// pointer to its IMAGE_SECTION_HEADER
//
template <class T> PIMAGE_SECTION_HEADER GetSectionHeader(PSTR name, T* pNTHeader)	// 'T' == PIMAGE_NT_HEADERS
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
        if ( 0 == strncmp((char *)section->Name,name,IMAGE_SIZEOF_SHORT_NAME) )
            return section;
    }
    
    return 0;
}


