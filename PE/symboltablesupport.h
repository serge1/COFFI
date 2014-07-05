class COFFSymbolTable;

BOOL LookupSymbolName(DWORD index, PSTR buffer, UINT length);
void DumpCOFFSymbolTable( COFFSymbolTable * pSymTab );
void DumpMiscDebugInfo( PIMAGE_DEBUG_MISC PMiscDebugInfo );
void DumpCVDebugInfoRecord( PDWORD pCVHeader );
void DumpLineNumbers(PIMAGE_LINENUMBER pln, DWORD count);

