#ifndef _COFFSymbolTable_H_
#define _COFFSymbolTable_H_

class COFFSymbolTable;

class COFFSymbol
{
	PSTR 			m_pStringTable;
	PIMAGE_SYMBOL 	m_pSymbolData;
	DWORD			m_index;
	PSTR			m_pszShortString;
	char			m_szTypeName[16];
			
	void	CleanUp( void );	// Called by destructor, COFFSymbolTable class

	public:

	COFFSymbol( PIMAGE_SYMBOL pSymbolData, PSTR pStringTable, DWORD index );
	~COFFSymbol( );

	DWORD GetIndex() { return m_index; }
	PSTR  GetName();
	DWORD GetValue() { return m_pSymbolData ? m_pSymbolData->Value : 0; }
	SHORT GetSectionNumber()
					{ return m_pSymbolData ? m_pSymbolData->SectionNumber : 0; }
	WORD  GetType() { return m_pSymbolData ? m_pSymbolData->Type : 0; }
	PSTR  GetTypeName();
	BYTE  GetStorageClass()
					{ return m_pSymbolData ? m_pSymbolData->StorageClass : 0; }
	PSTR  GetStorageClassName();
	BOOL  GetNumberOfAuxSymbols()
			{ return m_pSymbolData ? m_pSymbolData->NumberOfAuxSymbols : 0; }
	BOOL  GetAuxSymbolAsString( PSTR pszBuffer, unsigned cbBuffer );
	
	friend class COFFSymbolTable;
};

typedef COFFSymbol * PCOFFSymbol;

class COFFSymbolTable
{
	private:

	PIMAGE_SYMBOL	m_pSymbolBase;	// Array of IMAGE_SYMBOL's
	unsigned 		m_cSymbols;
	PSTR			m_pStringTable;

	public:

	COFFSymbolTable( PVOID pSymbolBase, unsigned cSymbols );
	~COFFSymbolTable( );

	unsigned GetNumberOfSymbols( void ) { return m_cSymbols; }

	PCOFFSymbol GetNextSymbol( PCOFFSymbol );	// 0 begins iteration

	PCOFFSymbol GetNearestSymbolFromRVA( DWORD rva, BOOL fExact );

	PCOFFSymbol GetSymbolFromIndex( DWORD index );
};

typedef COFFSymbolTable * PCOFFSymbolTable;

#endif
