extern PIMAGE_RESOURCE_DIRECTORY_ENTRY g_pStrResEntries;
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY g_pDlgResEntries;
extern DWORD g_cStrResEntries;
extern DWORD g_cDlgResEntries;


void DumpResources( PBYTE pImageBase, PIMAGE_NT_HEADERS pNTHeader );

//============================================================================
DWORD GetOffsetToDataFromResEntry( 	PBYTE pResourceBase,
									PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry );

//============================================================================
//
// Dump the information about one resource directory.
//
void DumpResourceDirectory(	PIMAGE_RESOURCE_DIRECTORY pResDir,
							PBYTE pResourceBase,
							DWORD level,
							DWORD resourceType );

