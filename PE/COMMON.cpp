//==================================
// PEDUMP - Matt Pietrek 1997-2001
// FILE: COMMON.C
//==================================

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "symboltablesupport.h"
#include "extrnvar.h"

PIMAGE_DEBUG_MISC g_pMiscDebugInfo = 0;
PDWORD g_pCVHeader = 0;
PIMAGE_COFF_SYMBOLS_HEADER g_pCOFFHeader = 0;
COFFSymbolTable * g_pCOFFSymbolTable = 0;

/*----------------------------------------------------------------------------*/
//
// Header related stuff
//
/*----------------------------------------------------------------------------*/


// Bitfield values and names for the IMAGE_FILE_HEADER flags
WORD_FLAG_DESCRIPTIONS ImageFileHeaderCharacteristics[] = 
{
{ IMAGE_FILE_RELOCS_STRIPPED, "RELOCS_STRIPPED" },
{ IMAGE_FILE_EXECUTABLE_IMAGE, "EXECUTABLE_IMAGE" },
{ IMAGE_FILE_LINE_NUMS_STRIPPED, "LINE_NUMS_STRIPPED" },
{ IMAGE_FILE_LOCAL_SYMS_STRIPPED, "LOCAL_SYMS_STRIPPED" },
{ IMAGE_FILE_AGGRESIVE_WS_TRIM, "AGGRESIVE_WS_TRIM" },
{ IMAGE_FILE_LARGE_ADDRESS_AWARE, "LARGE_ADDRESS_AWARE" },
{ IMAGE_FILE_BYTES_REVERSED_LO, "BYTES_REVERSED_LO" },
{ IMAGE_FILE_32BIT_MACHINE, "32BIT_MACHINE" },
{ IMAGE_FILE_DEBUG_STRIPPED, "DEBUG_STRIPPED" },
{ IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "REMOVABLE_RUN_FROM_SWAP" },
{ IMAGE_FILE_NET_RUN_FROM_SWAP, "NET_RUN_FROM_SWAP" },
{ IMAGE_FILE_SYSTEM, "SYSTEM" },
{ IMAGE_FILE_DLL, "DLL" },
{ IMAGE_FILE_UP_SYSTEM_ONLY, "UP_SYSTEM_ONLY" },
{ IMAGE_FILE_BYTES_REVERSED_HI, "BYTES_REVERSED_HI" }
// { IMAGE_FILE_MINIMAL_OBJECT, "MINIMAL_OBJECT" }, // Removed in NT 3.5
// { IMAGE_FILE_UPDATE_OBJECT, "UPDATE_OBJECT" },   // Removed in NT 3.5
// { IMAGE_FILE_16BIT_MACHINE, "16BIT_MACHINE" },   // Removed in NT 3.5
// { IMAGE_FILE_PATCH, "PATCH" },
};

#define NUMBER_IMAGE_HEADER_FLAGS \
    (sizeof(ImageFileHeaderCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

//
// Dump the IMAGE_FILE_HEADER for a PE file or an OBJ
//
void DumpHeader(PIMAGE_FILE_HEADER pImageFileHeader)
{
    UINT headerFieldWidth = 30;
    UINT i;
    
    printf("File Header\n");

    printf("  %-*s%04X (%s)\n", headerFieldWidth, "Machine:", 
                pImageFileHeader->Machine,
                GetMachineTypeName(pImageFileHeader->Machine) );
    printf("  %-*s%04X\n", headerFieldWidth, "Number of Sections:",
                pImageFileHeader->NumberOfSections);
	time_t timeStamp = pImageFileHeader->TimeDateStamp;
    printf("  %-*s%08X -> %s", headerFieldWidth, "TimeDateStamp:",
                pImageFileHeader->TimeDateStamp, ctime( &timeStamp) );
    printf("  %-*s%08X\n", headerFieldWidth, "PointerToSymbolTable:",
                pImageFileHeader->PointerToSymbolTable);
    printf("  %-*s%08X\n", headerFieldWidth, "NumberOfSymbols:",
                pImageFileHeader->NumberOfSymbols);
    printf("  %-*s%04X\n", headerFieldWidth, "SizeOfOptionalHeader:",
                pImageFileHeader->SizeOfOptionalHeader);
    printf("  %-*s%04X\n", headerFieldWidth, "Characteristics:",
                pImageFileHeader->Characteristics);
    for ( i=0; i < NUMBER_IMAGE_HEADER_FLAGS; i++ )
    {
        if ( pImageFileHeader->Characteristics & 
             ImageFileHeaderCharacteristics[i].flag )
            printf( "    %s\n", ImageFileHeaderCharacteristics[i].name );
    }
}

WORD_VALUE_NAMES g_arMachines[] = 
{
{ IMAGE_FILE_MACHINE_UNKNOWN, "UNKNOWN" },
{ IMAGE_FILE_MACHINE_I386, "I386" },
{ IMAGE_FILE_MACHINE_R3000, "R3000" },
{ IMAGE_FILE_MACHINE_R4000, "R4000" },
{ IMAGE_FILE_MACHINE_R10000, "R10000" },
{ IMAGE_FILE_MACHINE_WCEMIPSV2, "WCEMIPSV2" },
{ IMAGE_FILE_MACHINE_ALPHA, "ALPHA" },
{ IMAGE_FILE_MACHINE_SH3, "SH3" },
{ IMAGE_FILE_MACHINE_SH3DSP, "SH3DSP" },
{ IMAGE_FILE_MACHINE_SH3E, "SH3E" },
{ IMAGE_FILE_MACHINE_SH4, "SH4" },
{ IMAGE_FILE_MACHINE_SH5, "SH5" },
{ IMAGE_FILE_MACHINE_ARM, "ARM" },
{ IMAGE_FILE_MACHINE_THUMB, "THUMB" },
{ IMAGE_FILE_MACHINE_AM33, "AM33" },
{ IMAGE_FILE_MACHINE_POWERPC, "POWERPC" },
{ IMAGE_FILE_MACHINE_POWERPCFP, "POWERPCFP" },
{ IMAGE_FILE_MACHINE_IA64, "IA64" },
{ IMAGE_FILE_MACHINE_MIPS16, "MIPS16" },
{ IMAGE_FILE_MACHINE_ALPHA64, "ALPHA64" },
{ IMAGE_FILE_MACHINE_MIPSFPU, "MIPSFPU" },
{ IMAGE_FILE_MACHINE_MIPSFPU16, "MIPSFPU16" },
{ IMAGE_FILE_MACHINE_TRICORE, "TRICORE" },
{ IMAGE_FILE_MACHINE_CEF, "CEF" },
{ IMAGE_FILE_MACHINE_EBC, "EBC" },
{ IMAGE_FILE_MACHINE_AMD64, "AMD64" },
{ IMAGE_FILE_MACHINE_M32R, "M32R" },
{ IMAGE_FILE_MACHINE_CEE, "CEE" },
};


PSTR GetMachineTypeName( WORD wMachineType )
{
	for ( unsigned i = 0; i < ARRAY_SIZE(g_arMachines); i++ )
		if ( wMachineType == g_arMachines[i].wValue )
			return g_arMachines[i].pszName;

	return "unknown";
}

bool IsValidMachineType( WORD wMachineType )
{
	for ( unsigned i = 0; i < ARRAY_SIZE(g_arMachines); i++ )
		if ( wMachineType == g_arMachines[i].wValue )
			return true;

	return false;
}

/*----------------------------------------------------------------------------*/
//
// Section related stuff
//
/*----------------------------------------------------------------------------*/

// Bitfield values and names for the IMAGE_SECTION_HEADER flags
DWORD_FLAG_DESCRIPTIONS SectionCharacteristics[] = 
{

// { IMAGE_SCN_TYPE_DSECT, "DSECT" },
// { IMAGE_SCN_TYPE_NOLOAD, "NOLOAD" },
// { IMAGE_SCN_TYPE_GROUP, "GROUP" },
{ IMAGE_SCN_TYPE_NO_PAD, "NO_PAD" },
// { IMAGE_SCN_TYPE_COPY, "COPY" },
{ IMAGE_SCN_CNT_CODE, "CODE" },
{ IMAGE_SCN_CNT_INITIALIZED_DATA, "INITIALIZED_DATA" },
{ IMAGE_SCN_CNT_UNINITIALIZED_DATA, "UNINITIALIZED_DATA" },
{ IMAGE_SCN_LNK_OTHER, "OTHER" },
{ IMAGE_SCN_LNK_INFO, "INFO" },
// { IMAGE_SCN_TYPE_OVER, "OVER" },
{ IMAGE_SCN_LNK_REMOVE, "REMOVE" },
{ IMAGE_SCN_LNK_COMDAT, "COMDAT" },
// { IMAGE_SCN_MEM_PROTECTED, "PROTECTED" },
#define IMAGE_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section.
#define IMAGE_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
{ IMAGE_SCN_MEM_FARDATA, "FARDATA" },
// { IMAGE_SCN_MEM_SYSHEAP, "SYSHEAP" },
{ IMAGE_SCN_MEM_PURGEABLE, "PURGEABLE" },
{ IMAGE_SCN_MEM_LOCKED, "LOCKED" },
{ IMAGE_SCN_MEM_PRELOAD, "PRELOAD" },
{ IMAGE_SCN_LNK_NRELOC_OVFL, "NRELOC_OVFL" },
{ IMAGE_SCN_MEM_DISCARDABLE, "DISCARDABLE" },
{ IMAGE_SCN_MEM_NOT_CACHED, "NOT_CACHED" },
{ IMAGE_SCN_MEM_NOT_PAGED, "NOT_PAGED" },
{ IMAGE_SCN_MEM_SHARED, "SHARED" },
{ IMAGE_SCN_MEM_EXECUTE, "EXECUTE" },
{ IMAGE_SCN_MEM_READ, "READ" },
{ IMAGE_SCN_MEM_WRITE, "WRITE" },
};


#define NUMBER_SECTION_CHARACTERISTICS \
    (sizeof(SectionCharacteristics) / sizeof(DWORD_FLAG_DESCRIPTIONS))

//
// Dump the section table from a PE file or an OBJ
//
void DumpSectionTable(PIMAGE_SECTION_HEADER section,
                      unsigned cSections,
                      BOOL IsEXE)
{
    printf("Section Table\n");
    
    for ( unsigned i=1; i <= cSections; i++, section++ )
    {
        printf( "  %02X %-8.8s  %s: %08X  VirtAddr:  %08X\n",
                i, section->Name,
                IsEXE ? "VirtSize" : "PhysAddr",
                section->Misc.VirtualSize, section->VirtualAddress);
        printf( "    raw data offs:   %08X  raw data size: %08X\n",
                section->PointerToRawData, section->SizeOfRawData );
        printf( "    relocation offs: %08X  relocations:   %08X\n",
                section->PointerToRelocations, section->NumberOfRelocations );
        printf( "    line # offs:     %08X  line #'s:      %08X\n",
                section->PointerToLinenumbers, section->NumberOfLinenumbers );
        printf( "    characteristics: %08X\n", section->Characteristics);

        printf("    ");
        for ( unsigned j=0; j < NUMBER_SECTION_CHARACTERISTICS; j++ )
        {
            if ( section->Characteristics & 
                SectionCharacteristics[j].flag )
                printf( "  %s", SectionCharacteristics[j].name );
        }

		unsigned alignment = (section->Characteristics & IMAGE_SCN_ALIGN_MASK);
		if ( alignment == 0 )
		{
			printf( "  ALIGN_DEFAULT(16)" );
		}
		else
		{
			// Yeah, it's hard to read this, but it works, and it's elegant
			alignment = alignment >>= 20;
			printf( "  ALIGN_%uBYTES", 1 << (alignment-1) );
		}

        printf("\n\n");
    }
}

//
// Given a section name, look it up in the section table and return a
// pointer to the start of its raw data area.
//
LPVOID GetSectionPtr(PSTR name, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
        if (strncmp((char *)section->Name, name, IMAGE_SIZEOF_SHORT_NAME) == 0)
            return (LPVOID)(section->PointerToRawData + imageBase);
    }
    
    return 0;
}


PIMAGE_COFF_SYMBOLS_HEADER PCOFFDebugInfo = 0;

char *SzDebugFormats[] = {
"UNKNOWN/BORLAND","COFF","CODEVIEW","FPO","MISC","EXCEPTION","FIXUP",
"OMAP_TO_SRC", "OMAP_FROM_SRC"};

//
// Dump the debug directory array
//
void DumpDebugDirectory(PIMAGE_DEBUG_DIRECTORY debugDir, DWORD size, PBYTE pImageBase)
{
    DWORD cDebugFormats = size / sizeof(IMAGE_DEBUG_DIRECTORY);
    PSTR szDebugFormat;
    unsigned i;
    
    if ( cDebugFormats == 0 )
        return;
    
    printf(
    "Debug Formats in File\n"
    "  Type            Size     Address  FilePtr  Charactr TimeDate Version\n"
    "  --------------- -------- -------- -------- -------- -------- --------\n"
    );
    
    for ( i=0; i < cDebugFormats; i++ )
    {
        szDebugFormat = (debugDir->Type <= IMAGE_DEBUG_TYPE_OMAP_FROM_SRC )
                        ? SzDebugFormats[debugDir->Type] : "???";

        printf("  %-15s %08X %08X %08X %08X %08X %u.%02u\n",
            szDebugFormat, debugDir->SizeOfData, debugDir->AddressOfRawData,
            debugDir->PointerToRawData, debugDir->Characteristics,
            debugDir->TimeDateStamp, debugDir->MajorVersion,
            debugDir->MinorVersion);

		switch( debugDir->Type )
		{
        	case IMAGE_DEBUG_TYPE_COFF:
	            g_pCOFFHeader =
                (PIMAGE_COFF_SYMBOLS_HEADER)(pImageBase+ debugDir->PointerToRawData);
				break;

			case IMAGE_DEBUG_TYPE_MISC:
				g_pMiscDebugInfo =
				(PIMAGE_DEBUG_MISC)(pImageBase + debugDir->PointerToRawData);
				break;

			case IMAGE_DEBUG_TYPE_CODEVIEW:
				g_pCVHeader = (PDWORD)(pImageBase + debugDir->PointerToRawData);
				break;
		}

        debugDir++;
    }
}

/*----------------------------------------------------------------------------*/
//
// Other assorted stuff
//
/*----------------------------------------------------------------------------*/

//
// Do a hexadecimal dump of the raw data for all the sections.  You
// could just dump one section by adjusting the PIMAGE_SECTION_HEADER
// and cSections parameters
//
void DumpRawSectionData(PIMAGE_SECTION_HEADER section,
                        PVOID base,
                        unsigned cSections)
{
    unsigned i;
    char name[IMAGE_SIZEOF_SHORT_NAME + 1];

    printf("Section Hex Dumps\n");
    
    for ( i=1; i <= cSections; i++, section++ )
    {
        // Make a copy of the section name so that we can ensure that
        // it's null-terminated
        memcpy(name, section->Name, IMAGE_SIZEOF_SHORT_NAME);
        name[IMAGE_SIZEOF_SHORT_NAME] = 0;

        // Don't dump sections that don't exist in the file!
        if ( section->PointerToRawData == 0 )
            continue;
        
        printf( "section %02X (%s)  size: %08X  file offs: %08X\n",
                i, name, section->SizeOfRawData, section->PointerToRawData);

        HexDump( MakePtr(PBYTE, base, section->PointerToRawData),
                 section->SizeOfRawData );
        printf("\n");
    }
}

// Number of hex values displayed per line
#define HEX_DUMP_WIDTH 16

//
// Dump a region of memory in a hexadecimal format
//
void HexDump(PBYTE ptr, DWORD length)
{
    char buffer[256];
    PSTR buffPtr, buffPtr2;
    unsigned cOutput, i;
    DWORD bytesToGo=length;

    while ( bytesToGo  )
    {
        cOutput = bytesToGo >= HEX_DUMP_WIDTH ? HEX_DUMP_WIDTH : bytesToGo;

        buffPtr = buffer;
        buffPtr += sprintf(buffPtr, "%08X:  ", length-bytesToGo );
        buffPtr2 = buffPtr + (HEX_DUMP_WIDTH * 3) + 1;
        
        for ( i=0; i < HEX_DUMP_WIDTH; i++ )
        {
            BYTE value = *(ptr+i);

            if ( i >= cOutput )
            {
                // On last line.  Pad with spaces
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
            }
            else
            {
                if ( value < 0x10 )
                {
                    *buffPtr++ = '0';
                    itoa( value, buffPtr++, 16);
                }
                else
                {
                    itoa( value, buffPtr, 16);
                    buffPtr+=2;
                }
 
                *buffPtr++ = ' ';
                *buffPtr2++ = isprint(value) ? value : '.';
            }
            
            // Put an extra space between the 1st and 2nd half of the bytes
            // on each line.
            if ( i == (HEX_DUMP_WIDTH/2)-1 )
                *buffPtr++ = ' ';
        }

        *buffPtr2 = 0;  // Null terminate it.
        puts(buffer);   // Can't use printf(), since there may be a '%'
                        // in the string.
        bytesToGo -= cOutput;
        ptr += HEX_DUMP_WIDTH;
    }
}


