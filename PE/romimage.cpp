//==================================
// PEDUMP - Matt Pietrek 1997-2001
// FILE: EXEDUMP.CPP
//==================================

#include <windows.h>
#include <stdio.h>
#include <time.h>
#pragma hdrstop
#include "common.h"
#include "exedump.h"
#include "COFFSymbolTable.h"
#include "extrnvar.h"

void DumpROMOptionalHeader( PIMAGE_ROM_OPTIONAL_HEADER pROMOptHdr )
{
    UINT width = 30;

    printf("Optional Header\n");
    
    printf("  %-*s%04X\n", width, "Magic", pROMOptHdr->Magic);
    printf("  %-*s%u.%02u\n", width, "linker version",
        pROMOptHdr->MajorLinkerVersion,
        pROMOptHdr->MinorLinkerVersion);
    printf("  %-*s%X\n", width, "size of code", pROMOptHdr->SizeOfCode);
    printf("  %-*s%X\n", width, "size of initialized data",
        pROMOptHdr->SizeOfInitializedData);
    printf("  %-*s%X\n", width, "size of uninitialized data",
        pROMOptHdr->SizeOfUninitializedData);
    printf("  %-*s%X\n", width, "entrypoint RVA",
        pROMOptHdr->AddressOfEntryPoint);
    printf("  %-*s%X\n", width, "base of code", pROMOptHdr->BaseOfCode);
    printf("  %-*s%X\n", width, "base of Bss", pROMOptHdr->BaseOfBss);
    printf("  %-*s%X\n", width, "GprMask", pROMOptHdr->GprMask);

	printf("  %-*s%X\n", width, "CprMask[0]", pROMOptHdr->CprMask[0] );
	printf("  %-*s%X\n", width, "CprMask[1]", pROMOptHdr->CprMask[1] );
	printf("  %-*s%X\n", width, "CprMask[2]", pROMOptHdr->CprMask[2] );
	printf("  %-*s%X\n", width, "CprMask[3]", pROMOptHdr->CprMask[3] );

    printf("  %-*s%X\n", width, "GpValue", pROMOptHdr->GpValue);
}

// VARIATION on the IMAGE_FIRST_SECTION macro from WINNT.H

#define IMAGE_FIRST_ROM_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((DWORD_PTR)ntheader +                                                  \
     FIELD_OFFSET( IMAGE_ROM_HEADERS, OptionalHeader ) +                 \
     ((PIMAGE_ROM_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

void DumpROMImage( PIMAGE_ROM_HEADERS pROMHeader )
{
    DumpHeader(&pROMHeader->FileHeader);
    printf("\n");

    DumpROMOptionalHeader(&pROMHeader->OptionalHeader);
    printf("\n");

    DumpSectionTable( IMAGE_FIRST_ROM_SECTION(pROMHeader), 
                        pROMHeader->FileHeader.NumberOfSections, TRUE);
    printf("\n");

	// Dump COFF symbols out here.  Get offsets from the header
}

