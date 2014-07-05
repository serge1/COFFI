//==================================
// PEDUMP - Matt Pietrek 1997-2001
// FILE: RESDUMP.CPP
//==================================

#include <windows.h>
#include <stdio.h>
#include <time.h>
#pragma hdrstop
#include "common.h"
#include "extrnvar.h"
#include "resdump.h"

// The predefined resource types
char *SzResourceTypes[] = {
"???_0",
"CURSOR",
"BITMAP",
"ICON",
"MENU",
"DIALOG",
"STRING",
"FONTDIR",
"FONT",
"ACCELERATORS",
"RCDATA",
"MESSAGETABLE",
"GROUP_CURSOR",
"???_13",
"GROUP_ICON",
"???_15",
"VERSION",
"DLGINCLUDE",
"???_18",
"PLUGPLAY",
"VXD",
"ANICURSOR",
"ANIICON"
};

PIMAGE_RESOURCE_DIRECTORY_ENTRY g_pStrResEntries = 0;
PIMAGE_RESOURCE_DIRECTORY_ENTRY g_pDlgResEntries = 0;
DWORD g_cStrResEntries = 0;
DWORD g_cDlgResEntries = 0;

DWORD GetOffsetToDataFromResEntry( 	PBYTE pResourceBase,
									PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry )
{
	// The IMAGE_RESOURCE_DIRECTORY_ENTRY is gonna point to a single
	// IMAGE_RESOURCE_DIRECTORY, which in turn will point to the
	// IMAGE_RESOURCE_DIRECTORY_ENTRY, which in turn will point
	// to the IMAGE_RESOURCE_DATA_ENTRY that we're really after.  In
	// other words, traverse down a level.

	PIMAGE_RESOURCE_DIRECTORY pStupidResDir;
	pStupidResDir = (PIMAGE_RESOURCE_DIRECTORY)(pResourceBase + pResEntry->OffsetToDirectory);

    PIMAGE_RESOURCE_DIRECTORY_ENTRY pResDirEntry =
	    	(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pStupidResDir + 1);// PTR MATH

	PIMAGE_RESOURCE_DATA_ENTRY pResDataEntry =
			(PIMAGE_RESOURCE_DATA_ENTRY)(pResourceBase + pResDirEntry->OffsetToData);

	return pResDataEntry->OffsetToData;
}


// Get an ASCII string representing a resource type
void GetResourceTypeName(DWORD type, PSTR buffer, UINT cBytes)
{
    if ( type <= (WORD)RT_ANIICON )
        strncpy(buffer, SzResourceTypes[type], cBytes);
    else
        sprintf(buffer, "%X", type);
}

//
// If a resource entry has a string name (rather than an ID), go find
// the string and convert it from unicode to ascii.
//
void GetResourceNameFromId
(
    DWORD id, PBYTE pResourceBase, PSTR buffer, UINT cBytes
)
{
    PIMAGE_RESOURCE_DIR_STRING_U prdsu;

    // If it's a regular ID, just format it.
    if ( !(id & IMAGE_RESOURCE_NAME_IS_STRING) )
    {
        sprintf(buffer, "%X", id);
        return;
    }
    
    id &= 0x7FFFFFFF;
    prdsu = MakePtr(PIMAGE_RESOURCE_DIR_STRING_U, pResourceBase, id);

    // prdsu->Length is the number of unicode characters
    WideCharToMultiByte(CP_ACP, 0, prdsu->NameString, prdsu->Length,
                        buffer, cBytes, 0, 0);
    buffer[ min(cBytes-1,prdsu->Length) ] = 0;  // Null terminate it!!!
}

//
// Dump the information about one resource directory entry.  If the
// entry is for a subdirectory, call the directory dumping routine
// instead of printing information in this routine.
//
void DumpResourceEntry(
    PIMAGE_RESOURCE_DIRECTORY_ENTRY pResDirEntry,
    PBYTE pResourceBase,
    DWORD level )
{
    UINT i;
    char nameBuffer[128];
    PIMAGE_RESOURCE_DATA_ENTRY pResDataEntry;
    
    if ( pResDirEntry->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY )
    {
        DumpResourceDirectory( (PIMAGE_RESOURCE_DIRECTORY)
            ((pResDirEntry->OffsetToData & 0x7FFFFFFF) + pResourceBase),
            pResourceBase, level, pResDirEntry->Name);
        return;
    }

    // Spit out the spacing for the level indentation
    for ( i=0; i < level; i++ )
        printf("    ");

    if ( pResDirEntry->Name & IMAGE_RESOURCE_NAME_IS_STRING )
    {
        GetResourceNameFromId(pResDirEntry->Name, pResourceBase, nameBuffer,
                              sizeof(nameBuffer));
        printf("Name: %s  DataEntryOffs: %08X\n",
            nameBuffer, pResDirEntry->OffsetToData);
    }
    else
    {
        printf("ID: %08X  DataEntryOffs: %08X\n",
                pResDirEntry->Name, pResDirEntry->OffsetToData);
    }
    
    // the resDirEntry->OffsetToData is a pointer to an
    // IMAGE_RESOURCE_DATA_ENTRY.  Go dump out that information.  First,
    // spit out the proper indentation
    for ( i=0; i < level; i++ )
        printf("    ");
    
    pResDataEntry = MakePtr(PIMAGE_RESOURCE_DATA_ENTRY, pResourceBase, pResDirEntry->OffsetToData);
    printf("DataRVA: %05X  DataSize: %05X  CodePage: %X\n",
            pResDataEntry->OffsetToData, pResDataEntry->Size,
            pResDataEntry->CodePage);
}

//
// Dump the information about one resource directory.
//
void DumpResourceDirectory(	PIMAGE_RESOURCE_DIRECTORY pResDir,
							PBYTE pResourceBase,
							DWORD level,
							DWORD resourceType )
{
    PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry;
    char szType[64];
    UINT i;

    // Level 1 resources are the resource types
    if ( level == 1 )
    {
		printf( "    ---------------------------------------------------"
	            "-----------\n" );

		if ( resourceType & IMAGE_RESOURCE_NAME_IS_STRING )
		{
			GetResourceNameFromId( resourceType, pResourceBase,
									szType, sizeof(szType) );
		}
		else
		{
	        GetResourceTypeName( resourceType, szType, sizeof(szType) );
		}
	}
    else    // All other levels, just print out the regular id or name
    {
        GetResourceNameFromId( resourceType, pResourceBase, szType,
                               sizeof(szType) );
    }
	    
    // Spit out the spacing for the level indentation
    for ( i=0; i < level; i++ )
        printf("    ");

    printf(
        "ResDir (%s) Entries:%02X (Named:%02X, ID:%02X) TimeDate:%08X",
        szType, pResDir->NumberOfNamedEntries+ pResDir->NumberOfIdEntries,
        pResDir->NumberOfNamedEntries, pResDir->NumberOfIdEntries,
        pResDir->TimeDateStamp );
        
	if ( pResDir->MajorVersion || pResDir->MinorVersion )
		printf( " Vers:%u.%02u", pResDir->MajorVersion, pResDir->MinorVersion );
	if ( pResDir->Characteristics)
		printf( " Char:%08X", pResDir->Characteristics );
	printf( "\n" );

	//
	// The "directory entries" immediately follow the directory in memory
	//
    resDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDir+1);

	// If it's a stringtable, save off info for future use
	if ( level == 1 && (resourceType == (WORD)RT_STRING))
	{
		g_pStrResEntries = resDirEntry;
		g_cStrResEntries = pResDir->NumberOfIdEntries;
	}

	// If it's a stringtable, save off info for future use
	if ( level == 1 && (resourceType == (WORD)RT_DIALOG))
	{
		g_pDlgResEntries = resDirEntry;
		g_cDlgResEntries = pResDir->NumberOfIdEntries;
	}
	    
    for ( i=0; i < pResDir->NumberOfNamedEntries; i++, resDirEntry++ )
        DumpResourceEntry(resDirEntry, pResourceBase, level+1);

    for ( i=0; i < pResDir->NumberOfIdEntries; i++, resDirEntry++ )
        DumpResourceEntry(resDirEntry, pResourceBase, level+1);
}

template <class T>
void DumpStringTable( 	PBYTE pImageBase,
						T* pNTHeader,	// 'T' = PIMAGE_NT_HEADERS/32/64
						PBYTE pResourceBase,
						PIMAGE_RESOURCE_DIRECTORY_ENTRY pStrResEntry,
						DWORD cStrResEntries )
{
	for ( unsigned i = 0; i < cStrResEntries; i++, pStrResEntry++ )
	{
		DWORD offsetToData
			= GetOffsetToDataFromResEntry( pResourceBase, pStrResEntry );
			
 		PWORD pStrEntry = (PWORD)GetPtrFromRVA(	offsetToData,
												pNTHeader, pImageBase );
		if ( !pStrEntry)
			break;
		
		unsigned id = (pStrResEntry->Name - 1) << 4;

		for ( unsigned j = 0; j < 16; j++ )
		{
			WORD len = *pStrEntry++;
			if ( len )
			{
				printf( "%-5u: ", id + j );

				for ( unsigned k = 0; k < min(len, (WORD)64); k++ )
				{
					char * s;
					char szBuff[20];
					char c = (char)pStrEntry[k];
					switch( c )
					{
						case '\t': s = "\\t"; break;
						case '\r': s = "\\r"; break;
						case '\n': s = "\\n"; break;
						default:
							sprintf( szBuff, "%c", isprint(c) ? c : '.' );
							s=szBuff;
							break;
					}

					printf( s );
				}

				printf( "\n" );
			}

			pStrEntry += len;
		}
	}
}

//============================================================================

template <class T>
void DumpDialogs( 	PBYTE pImageBase,
					T* pNTHeader,	// 'T' = PIMAGE_NT_HEADERS/32/64
					PBYTE pResourceBase,
					PIMAGE_RESOURCE_DIRECTORY_ENTRY pDlgResEntry,
					DWORD cDlgResEntries )
{
	for ( unsigned i = 0; i < cDlgResEntries; i++, pDlgResEntry++ )
	{
		DWORD offsetToData
			= GetOffsetToDataFromResEntry( pResourceBase, pDlgResEntry );
			
 		PDWORD pDlgStyle = (PDWORD)GetPtrFromRVA(	offsetToData,
													pNTHeader, pImageBase );
		if ( !pDlgStyle )
			break;
													
		printf( "  ====================\n" );
		if ( HIWORD(*pDlgStyle) != 0xFFFF )
		{
			//	A regular DLGTEMPLATE
			DLGTEMPLATE * pDlgTemplate = ( DLGTEMPLATE * )pDlgStyle;

			printf( "  style: %08X\n", pDlgTemplate->style );			
			printf( "  extended style: %08X\n", pDlgTemplate->dwExtendedStyle );			

			printf( "  controls: %u\n", pDlgTemplate->cdit );
			printf( "  (%u,%u) - (%u,%u)\n",
						pDlgTemplate->x, pDlgTemplate->y,
						pDlgTemplate->x + pDlgTemplate->cx,
						pDlgTemplate->y + pDlgTemplate->cy );
			PWORD pMenu = (PWORD)(pDlgTemplate + 1);	// ptr math!

			//
			// First comes the menu
			//
			if ( *pMenu )
			{
				if ( 0xFFFF == *pMenu )
				{
					pMenu++;
					printf( "  ordinal menu: %u\n", *pMenu );
				}
				else
				{
					printf( "  menu: " );
					while ( *pMenu )
						printf( "%c", LOBYTE(*pMenu++) );				

					pMenu++;
					printf( "\n" );
				}
			}
			else
				pMenu++;	// Advance past the menu name

			//
			// Next comes the class
			//			
			PWORD pClass = pMenu;
						
			if ( *pClass )
			{
				if ( 0xFFFF == *pClass )
				{
					pClass++;
					printf( "  ordinal class: %u\n", *pClass );
				}
				else
				{
					printf( "  class: " );
					while ( *pClass )
					{
						printf( "%c", LOBYTE(*pClass++) );				
					}		
					pClass++;
					printf( "\n" );
				}
			}
			else
				pClass++;	// Advance past the class name
			
			//
			// Finally comes the title
			//

			PWORD pTitle = pClass;
			if ( *pTitle )
			{
				printf( "  title: " );

				while ( *pTitle )
					printf( "%c", LOBYTE(*pTitle++) );
					
				pTitle++;
			}
			else
				pTitle++;	// Advance past the Title name

			printf( "\n" );

			PWORD pFont = pTitle;
						
			if ( pDlgTemplate->style & DS_SETFONT )
			{
				printf( "  Font: %u point ",  *pFont++ );
				while ( *pFont )
					printf( "%c", LOBYTE(*pFont++) );

				pFont++;
				printf( "\n" );
			}
	        else
    	        pFont = pTitle; 

			// DLGITEMPLATE starts on a 4 byte boundary
			LPDLGITEMTEMPLATE pDlgItemTemplate = (LPDLGITEMTEMPLATE)pFont;
			
			for ( unsigned i=0; i < pDlgTemplate->cdit; i++ )
			{
				// Control item header....
				pDlgItemTemplate = (DLGITEMTEMPLATE *)
									(((DWORD_PTR)pDlgItemTemplate+3) & ~3);
				
				printf( "    style: %08X\n", pDlgItemTemplate->style );			
				printf( "    extended style: %08X\n",
						pDlgItemTemplate->dwExtendedStyle );			

				printf( "    (%u,%u) - (%u,%u)\n",
							pDlgItemTemplate->x, pDlgItemTemplate->y,
							pDlgItemTemplate->x + pDlgItemTemplate->cx,
							pDlgItemTemplate->y + pDlgItemTemplate->cy );
				printf( "    id: %u\n", pDlgItemTemplate->id );
				
				//
				// Next comes the control's class name or ID
				//			
				PWORD pClass = (PWORD)(pDlgItemTemplate + 1);
				if ( *pClass )
				{							
					if ( 0xFFFF == *pClass )
					{
						pClass++;
						printf( "    ordinal class: %u", *pClass++ );
					}
					else
					{
						printf( "    class: " );
						while ( *pClass )
							printf( "%c", LOBYTE(*pClass++) );

						pClass++;
						printf( "\n" );
					}
				}
				else
					pClass++;
					
				printf( "\n" );			

				//
				// next comes the title
				//

				PWORD pTitle = pClass;
				
				if ( *pTitle )
				{
					printf( "    title: " );
					if ( 0xFFFF == *pTitle )
					{
						pTitle++;
						printf( "%u\n", *pTitle++ );
					}
					else
					{
						while ( *pTitle )
							printf( "%c", LOBYTE(*pTitle++) );
						pTitle++;
						printf( "\n" );
					}
				}
				else	
					pTitle++;	// Advance past the Title name

				printf( "\n" );
				
				PBYTE pCreationData = (PBYTE)(((DWORD_PTR)pTitle + 1) & (0-2) );	// Round up or down to 2 byte boundary (?)
				
				if ( *pCreationData )
					pCreationData += *pCreationData;
				else
					pCreationData++;

				pDlgItemTemplate = (DLGITEMTEMPLATE *)pCreationData;	
				
				printf( "\n" );
			}
			
			printf( "\n" );
		}
		else
		{
			// A DLGTEMPLATEEX		
		}
		
		printf( "\n" );
	}
}


//============================================================================

//
// Top level routine called to dump out the entire resource hierarchy
//
template <class T> void DumpResourceSection(PBYTE pImageBase, T * pNTHeader)	// 'T' = PIMAGE_NT_HEADERS 32/64
{
	DWORD resourcesRVA;
    PBYTE pResDir;

	bool bIs64Bit = ( pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC );

	resourcesRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_RESOURCE);
	if ( !resourcesRVA )
		return;

    pResDir = (PBYTE)GetPtrFromRVA( resourcesRVA, pNTHeader, pImageBase );

	if ( !pResDir )
		return;
		
    printf("Resources (RVA: %X)\n", resourcesRVA );

    DumpResourceDirectory((PIMAGE_RESOURCE_DIRECTORY)pResDir, pResDir, 0, 0);

	printf( "\n" );

	if ( !fShowResources )
		return;
		
	if ( g_cStrResEntries )
	{
		printf( "String Table\n" );

		if ( bIs64Bit )
		{
			DumpStringTable( pImageBase, (PIMAGE_NT_HEADERS64)pNTHeader, pResDir, g_pStrResEntries, g_cStrResEntries );
		}
		else
		{
			DumpStringTable( pImageBase, (PIMAGE_NT_HEADERS32)pNTHeader, pResDir, g_pStrResEntries, g_cStrResEntries );
		}
		printf( "\n" );
	}

	if ( g_cDlgResEntries )
	{
		printf( "Dialogs\n" );

		if ( bIs64Bit )
		{
			DumpDialogs( pImageBase, (PIMAGE_NT_HEADERS64)pNTHeader, pResDir, g_pDlgResEntries, g_cDlgResEntries );
		}
		else
		{
			DumpDialogs( pImageBase, (PIMAGE_NT_HEADERS32)pNTHeader, pResDir, g_pDlgResEntries, g_cDlgResEntries );
		}
		printf( "\n" );
	}

	printf( "\n" );
}

void DumpResources( PBYTE pImageBase, PIMAGE_NT_HEADERS pNTHeader )
{
	if ( pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
		DumpResourceSection( pImageBase, (PIMAGE_NT_HEADERS64)pNTHeader );
	else
		DumpResourceSection( pImageBase, (PIMAGE_NT_HEADERS32)pNTHeader );
}
