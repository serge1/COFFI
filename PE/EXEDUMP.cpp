//==================================
// PEDUMP - Matt Pietrek 1994-2001
// FILE: EXEDUMP.CPP
//==================================

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <delayimp.h>
#include <wintrust.h>
#pragma hdrstop
#include "common.h"
#include "symboltablesupport.h"
#include "COFFSymbolTable.h"
#include "resdump.h"
#include "extrnvar.h"

//============================================================================

// Bitfield values and names for the DllCharacteritics flags
WORD_FLAG_DESCRIPTIONS DllCharacteristics[] = 
{
	{ IMAGE_DLLCHARACTERISTICS_NO_BIND, "NO_BIND" },		// Do not bind this image.
	{ IMAGE_DLLCHARACTERISTICS_WDM_DRIVER,	"WDM_DRIVER" },	// Driver uses WDM model
	{ IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, "TERMINAL_SERVER_AWARE" },
// Old, obsolete flags
//      IMAGE_LIBRARY_PROCESS_INIT           0x0001     // Reserved.
//      IMAGE_LIBRARY_PROCESS_TERM           0x0002     // Reserved.
//      IMAGE_LIBRARY_THREAD_INIT            0x0004     // Reserved.
//      IMAGE_LIBRARY_THREAD_TERM            0x0008     // Reserved.
};

#define NUMBER_DLL_CHARACTERISTICS \
    (sizeof(DllCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

// Names of the data directory elements that are defined
char *ImageDirectoryNames[] = {
    "EXPORT",
	"IMPORT",
	"RESOURCE",
	"EXCEPTION",
	"SECURITY",
	"BASERELOC",
    "DEBUG",
	"ARCHITECTURE",
	"GLOBALPTR",
	"TLS",
	"LOAD_CONFIG",
    "BOUND_IMPORT",  		// These two entries added for NT 3.51
	"IAT",
	"DELAY_IMPORT",			// This entry added in NT 5 time frame
	"COM_DESCRPTR" };		// For the .NET runtime (previously called COM+ 2.0)

#define NUMBER_IMAGE_DIRECTORY_ENTRYS \
    (sizeof(ImageDirectoryNames)/sizeof(char *))

void DisplayDataDirectoryEntry( PSTR pszName, IMAGE_DATA_DIRECTORY & dataDirEntry )
{
    printf( "  %-16s rva: %08X  size: %08X\n", pszName, dataDirEntry.VirtualAddress, dataDirEntry.Size );
}

//============================================================================
//
// Dump the IMAGE_OPTIONAL_HEADER from a PE file
//
template <class T> void DumpOptionalHeader(T* pImageOptionalHeader)		// 'T' is IMAGE_OPTIONAL_HEADER32/64
{
    UINT width = 30;
    char *s;
    UINT i;
 
	bool b64BitHeader = (IMAGE_NT_OPTIONAL_HDR64_MAGIC == pImageOptionalHeader->Magic);

    printf("Optional Header\n");
    
    printf("  %-*s%04X\n", width, "Magic", pImageOptionalHeader->Magic);
    printf("  %-*s%u.%02u\n", width, "linker version",
        pImageOptionalHeader->MajorLinkerVersion,
        pImageOptionalHeader->MinorLinkerVersion);
    printf("  %-*s%X\n", width, "size of code", pImageOptionalHeader->SizeOfCode);
    printf("  %-*s%X\n", width, "size of initialized data",
        pImageOptionalHeader->SizeOfInitializedData);
    printf("  %-*s%X\n", width, "size of uninitialized data",
        pImageOptionalHeader->SizeOfUninitializedData);
    printf("  %-*s%X\n", width, "entrypoint RVA",
        pImageOptionalHeader->AddressOfEntryPoint);
    printf("  %-*s%X\n", width, "base of code", pImageOptionalHeader->BaseOfCode);
	
	// 32/64 bit dependent code
	if ( b64BitHeader )
	{
		printf("  %-*s%I64X\n", width, "image base", pImageOptionalHeader->ImageBase);
	}
	else
	{
		// Can't refer to BaseOfData, since this field isn't in an IMAGE_NT_OPTIONAL_HDR64
		printf("  %-*s%X\n", width, "base of data", ((PIMAGE_OPTIONAL_HEADER32)pImageOptionalHeader)->BaseOfData );
		
		printf("  %-*s%X\n", width, "image base", pImageOptionalHeader->ImageBase);
	}
	// end of 32/64 bit dependent code

    printf("  %-*s%X\n", width, "section align",
        pImageOptionalHeader->SectionAlignment);
    printf("  %-*s%X\n", width, "file align", pImageOptionalHeader->FileAlignment);
    printf("  %-*s%u.%02u\n", width, "required OS version",
        pImageOptionalHeader->MajorOperatingSystemVersion,
        pImageOptionalHeader->MinorOperatingSystemVersion);
    printf("  %-*s%u.%02u\n", width, "image version",
        pImageOptionalHeader->MajorImageVersion,
        pImageOptionalHeader->MinorImageVersion);
    printf("  %-*s%u.%02u\n", width, "subsystem version",
        pImageOptionalHeader->MajorSubsystemVersion,
        pImageOptionalHeader->MinorSubsystemVersion);
    printf("  %-*s%X\n", width, "Win32 Version",
    	pImageOptionalHeader->Win32VersionValue);
    printf("  %-*s%X\n", width, "size of image", pImageOptionalHeader->SizeOfImage);
    printf("  %-*s%X\n", width, "size of headers",
            pImageOptionalHeader->SizeOfHeaders);
    printf("  %-*s%X\n", width, "checksum", pImageOptionalHeader->CheckSum);
    switch( pImageOptionalHeader->Subsystem )
    {
		case IMAGE_SUBSYSTEM_UNKNOWN: s = "UNKNOWN (0)"; break;
        case IMAGE_SUBSYSTEM_NATIVE: s = "Native"; break;
        case IMAGE_SUBSYSTEM_WINDOWS_GUI: s = "Windows GUI"; break;
        case IMAGE_SUBSYSTEM_WINDOWS_CUI: s = "Windows character"; break;
        case IMAGE_SUBSYSTEM_OS2_CUI: s = "OS/2 character"; break;
        case IMAGE_SUBSYSTEM_POSIX_CUI: s = "Posix character"; break;
		case IMAGE_SUBSYSTEM_NATIVE_WINDOWS: s = "Native Windows (Win9X driver)"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI: s = "Windows CE GUI"; break;
		case IMAGE_SUBSYSTEM_EFI_APPLICATION: s = "EFI_APPLICATION"; break;
		case IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER: s = "EFI_BOOT_SERVICE_DRIVER"; break;
		case IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER: s = "EFI_RUNTIME_DRIVER"; break;
		case IMAGE_SUBSYSTEM_EFI_ROM: s = "EFI_ROM";
		case IMAGE_SUBSYSTEM_XBOX: s = "XBOX";
        default: s = "unknown";
    }
    printf("  %-*s%04X (%s)\n", width, "Subsystem",
            pImageOptionalHeader->Subsystem, s);

// Marked as obsolete in MSDN CD 9
    printf("  %-*s%04X\n", width, "DLL flags",
            pImageOptionalHeader->DllCharacteristics);
    for ( i=0; i < NUMBER_DLL_CHARACTERISTICS; i++ )
    {
        if ( pImageOptionalHeader->DllCharacteristics & 
             DllCharacteristics[i].flag )
            printf( "  %-*s%s", width, " ", DllCharacteristics[i].name );
    }
    if ( pImageOptionalHeader->DllCharacteristics )
        printf("\n");

	PSTR pszSizeFmtString;

	if ( b64BitHeader )
		pszSizeFmtString = "  %-*s%I64X\n";
	else
		pszSizeFmtString = "  %-*s%X\n";

    printf( pszSizeFmtString, width, "stack reserve size",
        pImageOptionalHeader->SizeOfStackReserve);
    printf( pszSizeFmtString, width, "stack commit size",
        pImageOptionalHeader->SizeOfStackCommit);
    printf( pszSizeFmtString, width, "heap reserve size",
        pImageOptionalHeader->SizeOfHeapReserve);
    printf( pszSizeFmtString, width, "heap commit size",
        pImageOptionalHeader->SizeOfHeapCommit);

#if 0
// Marked as obsolete in MSDN CD 9
    printf("  %-*s%08X\n", width, "loader flags",
        pImageOptionalHeader->LoaderFlags);

    for ( i=0; i < NUMBER_LOADER_FLAGS; i++ )
    {
        if ( pImageOptionalHeader->LoaderFlags & 
             LoaderFlags[i].flag )
            printf( "  %s", LoaderFlags[i].name );
    }
    if ( pImageOptionalHeader->LoaderFlags )
        printf("\n");
#endif

    printf("  %-*s%X\n", width, "RVAs & sizes",
        pImageOptionalHeader->NumberOfRvaAndSizes);

    printf("\nData Directory\n");
    for ( i=0; i < pImageOptionalHeader->NumberOfRvaAndSizes; i++)
    {
		DisplayDataDirectoryEntry( (i >= NUMBER_IMAGE_DIRECTORY_ENTRYS) ? "unused" : ImageDirectoryNames[i],
									pImageOptionalHeader->DataDirectory[i] );
    }
}

template <class T> void DumpExeDebugDirectory(PBYTE pImageBase, T * pNTHeader)	// 'T' = PIMAGE_NT_HEADERS32 or PIMAGE_NT_HEADERS64
{
    PIMAGE_DEBUG_DIRECTORY debugDir;
    PIMAGE_SECTION_HEADER header;
    DWORD va_debug_dir;
    DWORD size;
    
    va_debug_dir = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG);

    if ( va_debug_dir == 0 )
        return;

    // If we found a .debug section, and the debug directory is at the
    // beginning of this section, it looks like a Borland file
    header = GetSectionHeader(".debug", pNTHeader);
    if ( header && (header->VirtualAddress == va_debug_dir) )
    {
        debugDir = MakePtr(PIMAGE_DEBUG_DIRECTORY, pImageBase, header->PointerToRawData);
        size = GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG)
                * sizeof(IMAGE_DEBUG_DIRECTORY);
    }
    else    // Look for the debug directory
    {
        header = GetEnclosingSectionHeader( va_debug_dir, pNTHeader );
        if ( !header )
            return;

        size = GetImgDirEntrySize( pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG );
    
		debugDir = (PIMAGE_DEBUG_DIRECTORY)GetPtrFromRVA( va_debug_dir, pNTHeader, pImageBase );
    }

    DumpDebugDirectory( debugDir, size, pImageBase );
}


template <class T, class U, class V>
void DumpImportsOfOneModule(	T* pINT, U* pIAT, V * pNTHeader,		// 'T', 'U' = IMAGE_THUNK_DATA, 'v' = IMAGE_NT_HEADERS
								PIMAGE_IMPORT_DESCRIPTOR pImportDesc,
								PBYTE pImageBase )
{
    PIMAGE_IMPORT_BY_NAME pOrdinalName;

	bool bIs64Bit = ( pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC );

	while ( 1 ) // Loop forever (or until we break out)
	{
		if ( pINT->u1.AddressOfData == 0 )
			break;

		ULONGLONG ordinal = -1;

		if ( bIs64Bit )
		{
			if ( IMAGE_SNAP_BY_ORDINAL64(pINT->u1.Ordinal) )
				ordinal = IMAGE_ORDINAL64(pINT->u1.Ordinal);
		}
		else
		{
			if ( IMAGE_SNAP_BY_ORDINAL32(pINT->u1.Ordinal) )
				ordinal = IMAGE_ORDINAL32(pINT->u1.Ordinal);			
		}

		if ( ordinal != -1 )
		{
			printf( "  %4u", ordinal );
		}
		else
		{
			// pINT->u1.AddressOfData is theoretically 32 or 64 bits, but in the on-disk representation,
			// we'll assume it's an RVA.  As such, we'll cast it to a DWORD.
			pOrdinalName = (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA(static_cast<DWORD>(pINT->u1.AddressOfData), pNTHeader, pImageBase);
			
			printf("  %4u  %s", pOrdinalName->Hint, pOrdinalName->Name);
		}
		
		// If it looks like the image has been bound, append the
		// bound address
		if ( pImportDesc->TimeDateStamp )
			printf( " (Bound to: %08X)", pIAT->u1.Function );
		
		printf( "\n" );
		
		pINT++;         // Advance to next thunk
		pIAT++;         // advance to next thunk
	}	
}

//
// Dump the imports table (the .idata section) of a PE file
//
template <class T> void DumpImportsSection(PBYTE pImageBase, T * pNTHeader)	// 'T' = PIMAGE_NT_HEADERS 
{
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    DWORD importsStartRVA;

    // Look up where the imports section is (normally in the .idata section)
    // but not necessarily so.  Therefore, grab the RVA from the data dir.
    importsStartRVA = GetImgDirEntryRVA(pNTHeader,IMAGE_DIRECTORY_ENTRY_IMPORT);
    if ( !importsStartRVA )
        return;

    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)GetPtrFromRVA(importsStartRVA,pNTHeader,pImageBase);
	if ( !pImportDesc )
		return;
            
	bool bIs64Bit = ( pNTHeader->FileHeader.SizeOfOptionalHeader == IMAGE_SIZEOF_NT_OPTIONAL64_HEADER );

    printf("Imports Table:\n");
    
    while ( 1 )
    {
        // See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
        if ( (pImportDesc->TimeDateStamp==0 ) && (pImportDesc->Name==0) )
            break;
        
        printf("  %s\n", GetPtrFromRVA(pImportDesc->Name, pNTHeader, pImageBase) );

        printf("  Import Lookup Table RVA:  %08X\n",
      			pImportDesc->Characteristics);

        printf("  TimeDateStamp:            %08X", pImportDesc->TimeDateStamp );
		if ( (pImportDesc->TimeDateStamp != 0) && (pImportDesc->TimeDateStamp != -1) )
		{
			time_t timeStamp = pImportDesc->TimeDateStamp;
			printf( " -> %s", ctime( &timeStamp ) );
		}
		else
			printf( "\n" );

        printf("  ForwarderChain:           %08X\n", pImportDesc->ForwarderChain);
        printf("  DLL Name RVA:             %08X\n", pImportDesc->Name);
        printf("  Import Address Table RVA: %08X\n", pImportDesc->FirstThunk);
    
        DWORD rvaINT = pImportDesc->OriginalFirstThunk;
        DWORD rvaIAT = pImportDesc->FirstThunk;

        if ( rvaINT == 0 )   // No Characteristics field?
        {
            // Yes! Gotta have a non-zero FirstThunk field then.
            rvaINT = rvaIAT;
            
            if ( rvaINT == 0 )   // No FirstThunk field?  Ooops!!!
                return;
        }
        
        // Adjust the pointer to point where the tables are in the
        // mem mapped file.
        PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)GetPtrFromRVA(rvaINT, pNTHeader, pImageBase);
		if (!pINT )
			return;

        PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)GetPtrFromRVA(rvaIAT, pNTHeader, pImageBase);
    
        printf("  Ordn  Name\n");
    
		bIs64Bit
			? DumpImportsOfOneModule( (PIMAGE_THUNK_DATA64)pINT, (PIMAGE_THUNK_DATA64)pIAT, pNTHeader, pImportDesc, pImageBase )
			: DumpImportsOfOneModule( (PIMAGE_THUNK_DATA32)pINT, (PIMAGE_THUNK_DATA32)pIAT, pNTHeader, pImportDesc, pImageBase );

        pImportDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
        printf("\n");
    }

    printf("\n");
}

//
// Dump the delayed imports table of a PE file
//

template <class T, class U> void DumpDelayedImportsImportNames( PBYTE pImageBase, T* pNTHeader, U* thunk, bool bUsingRVA )	// T = PIMAGE_NT_HEADER, U = 'IMAGE_THUNK_DATA'
{
    while ( 1 ) // Loop forever (or until we break out)
    {
        if ( thunk->u1.AddressOfData == 0 )
            break;

		ULONGLONG ordinalMask;
		if ( sizeof(thunk->u1.Ordinal) == sizeof(ULONGLONG) )	// Which ordinal mask should we use?
			ordinalMask = IMAGE_ORDINAL_FLAG64;
		else
			ordinalMask = IMAGE_ORDINAL_FLAG32;

        if ( thunk->u1.Ordinal & ordinalMask )
        {
            printf( "  %4u", (thunk->u1.Ordinal & 0xFFFF) );
        }
        else
        {
            PIMAGE_IMPORT_BY_NAME pOrdinalName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)0 + thunk->u1.AddressOfData);

			pOrdinalName = bUsingRVA
				? (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA((DWORD)thunk->u1.AddressOfData, pNTHeader, pImageBase)
				: (PIMAGE_IMPORT_BY_NAME)GetPtrFromVA((PVOID)pOrdinalName, pNTHeader, pImageBase);
            
			printf("  %4u  %s", pOrdinalName->Hint, pOrdinalName->Name);
        }
        
        printf( "\n" );

        thunk++;            // Advance to next thunk
    }

	printf( "\n" );
}

template <class T> void DumpDelayedImportsSection(PBYTE pImageBase, T* pNTHeader, bool bIs64Bit )	// 'T' = PIMAGE_NT_HEADERS 
{
	DWORD delayImportStartRVA;
    PCImgDelayDescr pDelayDesc;

    // Look up where the delay imports section is (normally in the .didat
	/// section) but not necessarily so.
 	delayImportStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );
    if ( !delayImportStartRVA )
        return;

	// This code is more complicated than it needs to be, thanks to Microsoft.  When the
	// ImgDelayDescr was originally created for Win32, portability to Win64 wasn't
	// considered.  As such, MS used pointers, rather than RVAs in the data structures.
	// Finally, MS issued a new DELAYIMP.H, which issued a flag indicating whether the
	// field values are RVAs or VAs.  Unfortunately, Microsoft has been rather slow to
	// get this header file out into general distibution.  Currently, you can get it as
	// part of the Win64 headers, or as part of VC7.  In the meanwhile, we'll use some
	// preprocessor trickery so that we can use the new field names, while still compiling
	// with the original DELAYIMP.H.

	#if _DELAY_IMP_VER < 2
	#define rvaDLLName		szName
	#define rvaHmod			phmod
	#define rvaIAT			pIAT
	#define rvaINT			pINT
	#define rvaBoundIAT		pBoundIAT
	#define rvaUnloadIAT	pUnloadIAT
	#endif

    pDelayDesc = (PCImgDelayDescr)GetPtrFromRVA(delayImportStartRVA, pNTHeader, pImageBase);
	if ( !pDelayDesc )
		return;
            
    printf("Delay Imports Table:\n");
	
	while ( pDelayDesc->rvaDLLName )
	{
		PSTR pszDLLName;
		
		// from more recent DELAYIMP.H:
		// enum DLAttr {                   // Delay Load Attributes
		//    dlattrRva = 0x1,                // RVAs are used instead of pointers
		//    };
		bool bUsingRVA = pDelayDesc->grAttrs & 1;

		DWORD dllNameRVA = (DWORD)pDelayDesc->rvaDLLName;
		PVOID dllNameVA = (PBYTE)0+(DWORD)pDelayDesc->rvaDLLName;

		pszDLLName = bUsingRVA
			? (PSTR)GetPtrFromRVA(dllNameRVA, pNTHeader,pImageBase)
			: (PSTR)GetPtrFromVA(dllNameVA,pNTHeader,pImageBase);

	    printf( "  %s\n", pszDLLName );
		printf( "  Attributes:                 %08X", pDelayDesc->grAttrs );
		if ( pDelayDesc->grAttrs & 1 )
			printf( "  dlattrRva" );
		printf( "\n" );
		printf( "  Name R(VA):                 %08X\n", pDelayDesc->rvaDLLName );
	    printf( "  HMODULE R(VA):              %08X\n", pDelayDesc->rvaHmod);
	    printf( "  Import Address Table R(VA): %08X\n", pDelayDesc->rvaIAT );
	    printf( "  Import Names Table R(VA):   %08X\n", pDelayDesc->rvaINT );
	    printf( "  Bound IAT R(VA):            %08X\n", pDelayDesc->rvaBoundIAT );
	    printf( "  Unload IAT R(VA):           %08X\n", pDelayDesc->rvaUnloadIAT );	    
		printf( "  TimeDateStamp:              %08X", pDelayDesc->dwTimeStamp );

		if ( pDelayDesc->dwTimeStamp )
		{
			time_t timeStamp = pDelayDesc->dwTimeStamp;
			printf( " -> %s", ctime(&timeStamp) );
		}
		else
			printf( "\n" );

		//
		// Display the Import Names Table.
		PVOID pvThunk;
		
		PVOID thunkVA = (PBYTE)0 + (DWORD)pDelayDesc->rvaINT;

		if ( pDelayDesc->grAttrs & 1 )	// see above comment on dlatttrRva being == 1
			pvThunk = GetPtrFromRVA((DWORD)pDelayDesc->rvaINT, pNTHeader, pImageBase );
		else
			pvThunk = GetPtrFromVA(thunkVA, pNTHeader, pImageBase );
    
        printf("  Ordn  Name\n");
        
		bIs64Bit
			? DumpDelayedImportsImportNames( pImageBase, pNTHeader, (PIMAGE_THUNK_DATA64)pvThunk, bUsingRVA )
			: DumpDelayedImportsImportNames( pImageBase, pNTHeader, (PIMAGE_THUNK_DATA32)pvThunk, bUsingRVA );

		pDelayDesc++;	// Pointer math.  Advance to next delay import desc.
	}

    printf("\n");

#if _DELAY_IMP_VER < 2 // Remove the alias names from the namespace
#undef szName
#undef phmod
#undef pIAT
#undef pINT
#undef pBoundIAT
#undef pUnloadIAT
#endif
}

//
// Dump the exports table (usually the .edata section) of a PE file
//
template <class T> void DumpExportsSection(PBYTE pImageBase, T * pNTHeader)	// 'T' = PIMAGE_NT_HEADERS 
{
    PIMAGE_EXPORT_DIRECTORY pExportDir;
    PIMAGE_SECTION_HEADER header;
    INT delta; 
    PSTR pszFilename;
    DWORD i;
    PDWORD pdwFunctions;
    PWORD pwOrdinals;
    DWORD *pszFuncNames;
    DWORD exportsStartRVA, exportsEndRVA;
    
    exportsStartRVA = GetImgDirEntryRVA(pNTHeader,IMAGE_DIRECTORY_ENTRY_EXPORT);
    exportsEndRVA = exportsStartRVA +
	   				GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);

    // Get the IMAGE_SECTION_HEADER that contains the exports.  This is
    // usually the .edata section, but doesn't have to be.
    header = GetEnclosingSectionHeader( exportsStartRVA, pNTHeader );
    if ( !header )
        return;

    delta = (INT)(header->VirtualAddress - header->PointerToRawData);
        
    pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetPtrFromRVA(exportsStartRVA, pNTHeader, pImageBase);
        
    pszFilename = (PSTR)GetPtrFromRVA( pExportDir->Name, pNTHeader, pImageBase );
        
    printf("exports table:\n\n");
    printf("  Name:            %s\n", pszFilename);
    printf("  Characteristics: %08X\n", pExportDir->Characteristics);
	
	time_t timeStamp = pExportDir->TimeDateStamp;
    printf("  TimeDateStamp:   %08X -> %s",
    			pExportDir->TimeDateStamp, ctime(&timeStamp) );
    printf("  Version:         %u.%02u\n", pExportDir->MajorVersion,
            pExportDir->MinorVersion);
    printf("  Ordinal base:    %08X\n", pExportDir->Base);
    printf("  # of functions:  %08X\n", pExportDir->NumberOfFunctions);
    printf("  # of Names:      %08X\n", pExportDir->NumberOfNames);
    
    pdwFunctions =	(PDWORD)GetPtrFromRVA( pExportDir->AddressOfFunctions, pNTHeader, pImageBase );
    pwOrdinals =	(PWORD)	GetPtrFromRVA( pExportDir->AddressOfNameOrdinals, pNTHeader, pImageBase );
    pszFuncNames =	(DWORD *)GetPtrFromRVA( pExportDir->AddressOfNames, pNTHeader, pImageBase );

    printf("\n  Entry Pt  Ordn  Name\n");
    for (	i=0;
			i < pExportDir->NumberOfFunctions;
			i++, pdwFunctions++ )
    {
        DWORD entryPointRVA = *pdwFunctions;

        if ( entryPointRVA == 0 )   // Skip over gaps in exported function
            continue;               // ordinals (the entrypoint is 0 for
                                    // these functions).

        printf("  %08X  %4u", entryPointRVA, i + pExportDir->Base );

        // See if this function has an associated name exported for it.
        for ( unsigned j=0; j < pExportDir->NumberOfNames; j++ )
            if ( pwOrdinals[j] == i )
                printf("  %s", GetPtrFromRVA(pszFuncNames[j], pNTHeader, pImageBase) );

        // Is it a forwarder?  If so, the entry point RVA is inside the
        // .edata section, and is an RVA to the DllName.EntryPointName
        if ( (entryPointRVA >= exportsStartRVA)
             && (entryPointRVA <= exportsEndRVA) )
        {
            printf(" (forwarder -> %s)", GetPtrFromRVA(entryPointRVA, pNTHeader, pImageBase) );
        }
        
        printf("\n");
    }

	printf( "\n" );
}

template <class T> void DumpRuntimeFunctions( PBYTE pImageBase, T* pNTHeader )
{
	DWORD rtFnRVA;

	rtFnRVA = GetImgDirEntryRVA( pNTHeader, IMAGE_DIRECTORY_ENTRY_EXCEPTION );
	if ( !rtFnRVA )
		return;

	DWORD cEntries =
		GetImgDirEntrySize( pNTHeader, IMAGE_DIRECTORY_ENTRY_EXCEPTION )
		/ sizeof( IMAGE_RUNTIME_FUNCTION_ENTRY );
	if ( 0 == cEntries )
		return;

	PIMAGE_IA64_RUNTIME_FUNCTION_ENTRY pRTFn = (PIMAGE_IA64_RUNTIME_FUNCTION_ENTRY)
							GetPtrFromRVA( rtFnRVA, pNTHeader, pImageBase );

	if ( !pRTFn )
		return;

	printf( "Runtime Function Table (Exception handling)\n" );
	printf( "  Begin     End       Unwind  \n" );
	printf( "  --------  --------  --------\n" );

	for ( unsigned i = 0; i < cEntries; i++, pRTFn++ )
	{
		printf(	"  %08X  %08X  %08X",
			pRTFn->BeginAddress, pRTFn->EndAddress, pRTFn->UnwindInfoAddress );

		if ( g_pCOFFSymbolTable )
		{
			PCOFFSymbol pSymbol
				= g_pCOFFSymbolTable->GetNearestSymbolFromRVA( pRTFn->BeginAddress, TRUE );
			if ( pSymbol )
				printf( "  %s", pSymbol->GetName() );

			delete pSymbol;
		}

		printf( "\n" );
	}
}

// The names of the available base relocations
char *SzRelocTypes[] = {
{"ABSOLUTE" },			   // 0
{"HIGH" },                 // 1
{"LOW" },                  // 2
{"HIGHLOW" },              // 3
{"HIGHADJ" },              // 4
{"MIPS_JMPADDR" },         // 5
{ "???6" },
{ "???7" },
{ "???8" },
{"IA64_IMM64" },           // 9
{"DIR64" },                // 10
};

//
// Dump the base relocation table of a PE file
//
template <class T> void DumpBaseRelocationsSection(PBYTE pImageBase, T * pNTHeader)
{
	DWORD dwBaseRelocRVA;
    PIMAGE_BASE_RELOCATION baseReloc;

	dwBaseRelocRVA =
		GetImgDirEntryRVA( pNTHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC );
    if ( !dwBaseRelocRVA )
        return;

    baseReloc = (PIMAGE_BASE_RELOCATION)
    				GetPtrFromRVA( dwBaseRelocRVA, pNTHeader, pImageBase );
	if ( !baseReloc )
		return;

    printf("base relocations:\n\n");

    while ( baseReloc->SizeOfBlock != 0 )
    {
        unsigned i,cEntries;
        PWORD pEntry;
        char *szRelocType;
        WORD relocType;

		// Sanity check to make sure the data looks OK.
		if ( 0 == baseReloc->VirtualAddress )
			break;
		if ( baseReloc->SizeOfBlock < sizeof(*baseReloc) )
			break;
		
        cEntries = (baseReloc->SizeOfBlock-sizeof(*baseReloc))/sizeof(WORD);
        pEntry = MakePtr( PWORD, baseReloc, sizeof(*baseReloc) );
        
        printf("Virtual Address: %08X  size: %08X\n",
                baseReloc->VirtualAddress, baseReloc->SizeOfBlock);
            
        for ( i=0; i < cEntries; i++ )
        {
            // Extract the top 4 bits of the relocation entry.  Turn those 4
            // bits into an appropriate descriptive string (szRelocType)
            relocType = (*pEntry & 0xF000) >> 12;
            szRelocType = relocType <= IMAGE_REL_BASED_DIR64 ? SzRelocTypes[relocType] : "unknown";
            
            printf("  %08X %s",
                    (*pEntry & 0x0FFF) + baseReloc->VirtualAddress,
                    szRelocType);

			if ( IMAGE_REL_BASED_HIGHADJ == relocType )
			{
				pEntry++;
				cEntries--;
				printf( " (%X)", *pEntry );
			}

			printf( "\n" );
            pEntry++;   // Advance to next relocation entry
        }
        
        baseReloc = MakePtr( PIMAGE_BASE_RELOCATION, baseReloc,
                             baseReloc->SizeOfBlock);
    }
}

//
// Dump out the new IMAGE_BOUND_IMPORT_DESCRIPTOR that NT 3.51 added
//
template <class T> void DumpBoundImportDescriptors( PBYTE pImageBase, T* pNTHeader )	// 'T' = PIMATE_NT_HEADERS
{
    DWORD bidRVA;   // Bound import descriptors RVA
    PIMAGE_BOUND_IMPORT_DESCRIPTOR pibid;

    bidRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
    if ( !bidRVA )
        return;
    
    pibid = MakePtr( PIMAGE_BOUND_IMPORT_DESCRIPTOR, pImageBase, bidRVA );
    
    printf( "Bound import descriptors:\n\n" );
    printf( "  Module        TimeDate\n" );
    printf( "  ------------  --------\n" );
    
    while ( pibid->TimeDateStamp )
    {
        unsigned i;
        PIMAGE_BOUND_FORWARDER_REF pibfr;

        time_t timeStamp = pibid->TimeDateStamp;

        printf( "  %-12s  %08X -> %s",
        		pImageBase + bidRVA + pibid->OffsetModuleName,
                pibid->TimeDateStamp,
                ctime(&timeStamp) );
                            
        pibfr = MakePtr(PIMAGE_BOUND_FORWARDER_REF, pibid,
                            sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR));

        for ( i=0; i < pibid->NumberOfModuleForwarderRefs; i++ )
        {
			timeStamp = pibfr->TimeDateStamp;

            printf("    forwarder:  %-12s  %08X -> %s", 
                            pImageBase + bidRVA + pibfr->OffsetModuleName,
                            pibfr->TimeDateStamp,
                            ctime(&timeStamp) );
            pibfr++;    // advance to next forwarder ref
                
            // Keep the outer loop pointer up to date too!
            pibid = MakePtr( PIMAGE_BOUND_IMPORT_DESCRIPTOR, pibid,
                             sizeof( IMAGE_BOUND_FORWARDER_REF ) );
        }

        pibid++;    // Advance to next pibid;
    }

    printf( "\n" );
}

//
// Dump the TLS data
//
template <class T,class U> void DumpTLSDirectory(PBYTE pImageBase, T* pNTHeader, U * pTLSDir )	// 'T' = IMAGE_NT_HEADERS, U = 'IMAGE_TLS_DIRECTORY'
{
    DWORD tlsRVA;   // TLS dirs RVA

    tlsRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_TLS);
    if ( !tlsRVA )
        return;

    pTLSDir = (U*)GetPtrFromRVA( tlsRVA, pNTHeader, pImageBase );

    printf("TLS directory:\n");
    printf( "  StartAddressOfRawData: %08X\n", pTLSDir->StartAddressOfRawData );
    printf( "  EndAddressOfRawData:   %08X\n", pTLSDir->EndAddressOfRawData );
    printf( "  AddressOfIndex:        %08X\n", pTLSDir->AddressOfIndex );
    printf( "  AddressOfCallBacks:    %08X\n", pTLSDir->AddressOfCallBacks );
    printf( "  SizeOfZeroFill:        %08X\n", pTLSDir->SizeOfZeroFill );
    printf( "  Characteristics:       %08X\n", pTLSDir->Characteristics );

	printf( "\n" );
}

template <class T> void DumpCOR20Header( PBYTE pImageBase, T* pNTHeader )	// T = PIMAGE_NT_HEADERS
{
    DWORD cor20HdrRVA;   // COR20_HEADER RVA

    cor20HdrRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR );
    if ( !cor20HdrRVA )
        return;

    PIMAGE_COR20_HEADER pCor20Hdr = (PIMAGE_COR20_HEADER)GetPtrFromRVA( cor20HdrRVA, pNTHeader, pImageBase );
	
	printf( ".NET Runtime Header:\n" );

	printf( "  Size:       %u\n", pCor20Hdr->cb );
	printf( "  Version:    %u.%u\n", pCor20Hdr->MajorRuntimeVersion, pCor20Hdr->MinorRuntimeVersion );
	printf( "  Flags:      %X\n", pCor20Hdr->Flags );
	if ( pCor20Hdr->Flags & COMIMAGE_FLAGS_ILONLY ) printf( "    ILONLY\n" );
	if ( pCor20Hdr->Flags & COMIMAGE_FLAGS_32BITREQUIRED ) printf( "    32BITREQUIRED\n" );
	if ( pCor20Hdr->Flags & COMIMAGE_FLAGS_IL_LIBRARY ) printf( "    IL_LIBRARY\n" );
	if ( pCor20Hdr->Flags & 8 ) printf( "    STRONGNAMESIGNED\n" );		// At this moment, WINNT.H and CorHdr.H are out of sync...
	if ( pCor20Hdr->Flags & COMIMAGE_FLAGS_TRACKDEBUGDATA ) printf( "    TRACKDEBUGDATA\n" );

	DisplayDataDirectoryEntry( "MetaData", pCor20Hdr->MetaData );
    DisplayDataDirectoryEntry( "Resources", pCor20Hdr->Resources );
    DisplayDataDirectoryEntry( "StrongNameSig", pCor20Hdr->StrongNameSignature );
    DisplayDataDirectoryEntry( "CodeManagerTable", pCor20Hdr->CodeManagerTable );
    DisplayDataDirectoryEntry( "VTableFixups", pCor20Hdr->VTableFixups );
    DisplayDataDirectoryEntry( "ExprtAddrTblJmps", pCor20Hdr->ExportAddressTableJumps );
    DisplayDataDirectoryEntry( "ManagedNativeHdr", pCor20Hdr->ManagedNativeHeader );

	printf( "\n" );
}

template <class T, class U> void DumpLoadConfigDirectory(PBYTE pImageBase, T* pNTHeader, U * pLCD )	// T = PIMAGE_NT_HEADERS, U = PIMAGE_LOAD_CONFIG_DIRECTORY 
{
    DWORD loadConfigDirRVA;

    loadConfigDirRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG );
    if ( !loadConfigDirRVA )
        return;

	pLCD = (U*)GetPtrFromRVA( loadConfigDirRVA, pNTHeader, pImageBase );

	printf( "Image Load Configuration Directory:\n" );

    printf( "  Size:    %X\n", pLCD->Size );
    printf( "  TimeDateStamp:    %X\n", pLCD->TimeDateStamp );
    printf( "  Version:    %u.%u\n", pLCD->MajorVersion, pLCD->MinorVersion );
    printf( "  GlobalFlagsClear:    %X\n", pLCD->GlobalFlagsClear );
    printf( "  GlobalFlagsSet:    %X\n", pLCD->GlobalFlagsSet );
    printf( "  CriticalSectionDefaultTimeout:    %X\n", pLCD->CriticalSectionDefaultTimeout );
    printf( "  DeCommitFreeBlockThreshold:    %X\n", pLCD->DeCommitFreeBlockThreshold );
    printf( "  DeCommitTotalFreeThreshold:    %X\n", pLCD->DeCommitTotalFreeThreshold );
	printf( "  LockPrefixTable:    %X\n", pLCD->LockPrefixTable );
    printf( "  MaximumAllocationSize:    %X\n", pLCD->MaximumAllocationSize );
    printf( "  VirtualMemoryThreshold:    %X\n", pLCD->VirtualMemoryThreshold );
    printf( "  ProcessHeapFlags:    %X\n", pLCD->ProcessHeapFlags );
    printf( "  ProcessAffinityMask:    %X\n", pLCD->ProcessAffinityMask );
    printf( "  CSDVersion:    %u\n", pLCD->CSDVersion );
    printf( "  Reserved1:    %X\n", pLCD->Reserved1 );
	printf( "  EditList:    %X\n", pLCD->EditList );

	printf( "\n" );
}

template <class T> void DumpCertificates(PBYTE pImageBase, T* pNTHeader)
{
	// Note that the this DataDirectory entry gives a >>> FILE OFFSET <<< rather than
	// an RVA.
    DWORD certOffset;

    certOffset = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_SECURITY );
    if ( !certOffset )
        return;
	
	DWORD dwTotalSize = GetImgDirEntrySize( pNTHeader, IMAGE_DIRECTORY_ENTRY_SECURITY );

	printf( "Certificates:\n" );

	while ( dwTotalSize )	// As long as there is unprocessed certificate data...
	{
		LPWIN_CERTIFICATE pCert = MakePtr( LPWIN_CERTIFICATE, pImageBase, certOffset );

		printf( "  Length:   %X\n", pCert->dwLength );
		printf( "  Revision: %X\n", pCert->wRevision );
		printf( "  Type:     %X", pCert->wCertificateType );
		
		switch( pCert->wCertificateType )
		{
			case WIN_CERT_TYPE_X509: printf( " (X509)" ); break;
			case WIN_CERT_TYPE_PKCS_SIGNED_DATA: printf( " (PKCS_SIGNED_DATA)" ); break;
			case WIN_CERT_TYPE_TS_STACK_SIGNED: printf( " (TS_STACK_SIGNED)" ); break;
		}

		printf( "\n" );

		dwTotalSize -= pCert->dwLength;
		certOffset += pCert->dwLength;		// Get offset to next certificate
	}

	printf( "\n" );
}

//
// top level routine called from PEDUMP.CPP to dump the components of a PE file
//
void DumpExeFile( PIMAGE_DOS_HEADER dosHeader )
{
    PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_NT_HEADERS64 pNTHeader64;
    PBYTE pImageBase = (PBYTE)dosHeader;
    
	// Make pointers to 32 and 64 bit versions of the header.
    pNTHeader = MakePtr( PIMAGE_NT_HEADERS, dosHeader,
                                dosHeader->e_lfanew );

	pNTHeader64 = (PIMAGE_NT_HEADERS64)pNTHeader;

    // First, verify that the e_lfanew field gave us a reasonable
    // pointer, then verify the PE signature.
	if ( IsBadReadPtr( pNTHeader, sizeof(pNTHeader->Signature) ) )
	{
		printf("Not a Portable Executable (PE) EXE\n");
		return;
	}

    if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
    {
        printf("Not a Portable Executable (PE) EXE\n");
        return;
    }

    DumpHeader((PIMAGE_FILE_HEADER)&pNTHeader->FileHeader);
    printf("\n");

	bool bIs64Bit = ( pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC );

	if ( bIs64Bit )
		DumpOptionalHeader(&pNTHeader64->OptionalHeader);
	else
		DumpOptionalHeader(&pNTHeader->OptionalHeader);

    printf("\n");

    DumpSectionTable( IMAGE_FIRST_SECTION(pNTHeader), 
                        pNTHeader->FileHeader.NumberOfSections, TRUE);
    printf("\n");

	if ( bIs64Bit )
		DumpExeDebugDirectory( pImageBase, pNTHeader64 );
	else
		DumpExeDebugDirectory( pImageBase, pNTHeader );

    if ( pNTHeader->FileHeader.PointerToSymbolTable == 0 )
        g_pCOFFHeader = 0; // Doesn't really exist!
    printf("\n");

	DumpResources(pImageBase, pNTHeader);

	bIs64Bit
		? DumpTLSDirectory( pImageBase, pNTHeader64, (PIMAGE_TLS_DIRECTORY64)0 )	// Passing NULL ptr is a clever hack
		: DumpTLSDirectory( pImageBase, pNTHeader, (PIMAGE_TLS_DIRECTORY32)0 );		// See if you can figure it out! :-)

	bIs64Bit
	    ? DumpImportsSection(pImageBase, pNTHeader64 )
		: DumpImportsSection(pImageBase, pNTHeader);
    
	bIs64Bit
		? DumpDelayedImportsSection(pImageBase, pNTHeader64, bIs64Bit )
		: DumpDelayedImportsSection(pImageBase, pNTHeader, bIs64Bit );

	bIs64Bit 
		? DumpBoundImportDescriptors( pImageBase, pNTHeader64 )
		: DumpBoundImportDescriptors( pImageBase, pNTHeader );
    
	bIs64Bit
	    ? DumpExportsSection( pImageBase, pNTHeader64 )
		: DumpExportsSection( pImageBase, pNTHeader );

	bIs64Bit
		? DumpCOR20Header( pImageBase, pNTHeader64 )
		: DumpCOR20Header( pImageBase, pNTHeader );

	bIs64Bit
		? DumpLoadConfigDirectory( pImageBase, pNTHeader64, (PIMAGE_LOAD_CONFIG_DIRECTORY64)0 )	// Passing NULL ptr is a clever hack
		: DumpLoadConfigDirectory( pImageBase, pNTHeader, (PIMAGE_LOAD_CONFIG_DIRECTORY32)0 );	// See if you can figure it out! :-)

	bIs64Bit
		? DumpCertificates( pImageBase, pNTHeader64 )
		: DumpCertificates( pImageBase, pNTHeader );

	//=========================================================================
	//
	// If we have COFF symbols, create a symbol table now
	//
	//=========================================================================

	if ( g_pCOFFHeader )	// Did we see a COFF symbols header while looking
	{						// through the debug directory?
		g_pCOFFSymbolTable = new COFFSymbolTable(
				pImageBase+ pNTHeader->FileHeader.PointerToSymbolTable,
				pNTHeader->FileHeader.NumberOfSymbols );
	}

	if ( fShowPDATA )
	{
		bIs64Bit
			? DumpRuntimeFunctions( pImageBase, pNTHeader64 )
			: DumpRuntimeFunctions( pImageBase, pNTHeader ); 
				
		printf( "\n" );
	}

    if ( fShowRelocations )
    {
        bIs64Bit
			? DumpBaseRelocationsSection(pImageBase, pNTHeader64 )
			: DumpBaseRelocationsSection( pImageBase, pNTHeader );
        printf("\n");
    } 

	if ( fShowSymbolTable && g_pMiscDebugInfo )
	{
		DumpMiscDebugInfo( g_pMiscDebugInfo );
		printf( "\n" );
	}

	if ( fShowSymbolTable && g_pCVHeader )
	{
		DumpCVDebugInfoRecord( g_pCVHeader );
		printf( "\n" );
	}

    if ( fShowSymbolTable && g_pCOFFHeader )
    {
        DumpCOFFHeader( g_pCOFFHeader );
        printf("\n");
    }
    
    if ( fShowLineNumbers && g_pCOFFHeader )
    {
        DumpLineNumbers( MakePtr(PIMAGE_LINENUMBER, g_pCOFFHeader,
                            g_pCOFFHeader->LvaToFirstLinenumber),
                            g_pCOFFHeader->NumberOfLinenumbers);
        printf("\n");
    }

    if ( fShowSymbolTable )
    {
        if ( pNTHeader->FileHeader.NumberOfSymbols 
            && pNTHeader->FileHeader.PointerToSymbolTable
			&& g_pCOFFSymbolTable )
        {
            DumpCOFFSymbolTable( g_pCOFFSymbolTable );
            printf("\n");
        }
    }
    
    if ( fShowRawSectionData )
    {
		PIMAGE_SECTION_HEADER pSectionHdr;

		pSectionHdr = bIs64Bit ? (PIMAGE_SECTION_HEADER)(pNTHeader64+1) : (PIMAGE_SECTION_HEADER)(pNTHeader+1);

        DumpRawSectionData( pSectionHdr, dosHeader, pNTHeader->FileHeader.NumberOfSections);
    }

	if ( g_pCOFFSymbolTable )
		delete g_pCOFFSymbolTable;
}
