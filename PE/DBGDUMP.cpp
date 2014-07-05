//==================================
// PEDUMP - Matt Pietrek 1997-2001
// FILE: DBGDUMP.CPP
//==================================

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "common.h"
#include "COFFSymbolTable.h"
#include "SymbolTableSupport.h"
#include "extrnvar.h"
#include "dbgdump.h"
#include "cvsymbols.h"

void DumpImageDbgHeader(PIMAGE_SEPARATE_DEBUG_HEADER pImageSepDbgHeader)
{
    UINT headerFieldWidth = 30;

    printf("  %-*s%04X\n", headerFieldWidth, "Flags:",
                pImageSepDbgHeader->Flags);
    printf("  %-*s%04X %s\n", headerFieldWidth, "Machine:",
                pImageSepDbgHeader->Machine,
                GetMachineTypeName(pImageSepDbgHeader->Machine));
    printf("  %-*s%04X\n", headerFieldWidth, "Characteristics:",
                pImageSepDbgHeader->Characteristics);
	time_t timeStamp = pImageSepDbgHeader->TimeDateStamp;
    printf("  %-*s%08X -> %s", headerFieldWidth, "TimeDateStamp:",
                pImageSepDbgHeader->TimeDateStamp, ctime(&timeStamp) );
    printf("  %-*s%08X\n", headerFieldWidth, "CheckSum:",
                pImageSepDbgHeader->CheckSum);
    printf("  %-*s%08X\n", headerFieldWidth, "ImageBase:",
                pImageSepDbgHeader->ImageBase);
    printf("  %-*s%08X\n", headerFieldWidth, "Size of Image:",
                pImageSepDbgHeader->SizeOfImage);
    printf("  %-*s%04X\n", headerFieldWidth, "Number of Sections:",
                pImageSepDbgHeader->NumberOfSections);
    printf("  %-*s%04X\n", headerFieldWidth, "ExportedNamesSize:",
                pImageSepDbgHeader->ExportedNamesSize);
    printf("  %-*s%08X\n", headerFieldWidth, "DebugDirectorySize:",
                pImageSepDbgHeader->DebugDirectorySize);
    printf("  %-*s%08X\n", headerFieldWidth, "SectionAlignment:",
                pImageSepDbgHeader->SectionAlignment);
}

void DumpDbgFile( PIMAGE_SEPARATE_DEBUG_HEADER pImageSepDbgHeader )
{
    DumpImageDbgHeader(pImageSepDbgHeader);
    printf("\n");
    
    DumpSectionTable( (PIMAGE_SECTION_HEADER)(pImageSepDbgHeader+1),
                        pImageSepDbgHeader->NumberOfSections, TRUE);
                    
    DumpDebugDirectory(
        MakePtr(PIMAGE_DEBUG_DIRECTORY,
        pImageSepDbgHeader, sizeof(IMAGE_SEPARATE_DEBUG_HEADER) +
        (pImageSepDbgHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER))
        + pImageSepDbgHeader->ExportedNamesSize),
        pImageSepDbgHeader->DebugDirectorySize,
        (PBYTE)pImageSepDbgHeader);
    
    printf("\n");
    
    if ( g_pCOFFHeader )
	{
        DumpCOFFHeader( g_pCOFFHeader );
    
		printf("\n");

		g_pCOFFSymbolTable = new COFFSymbolTable(
			MakePtr( PVOID, g_pCOFFHeader, g_pCOFFHeader->LvaToFirstSymbol),
			g_pCOFFHeader->NumberOfSymbols );


		DumpCOFFSymbolTable( g_pCOFFSymbolTable );

		delete g_pCOFFSymbolTable;
	}
	
	if ( g_pCVHeader )
	{
		DumpCVSymbolTable( (PBYTE)g_pCVHeader, g_pMappedFileBase );
	}
}
