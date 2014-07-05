//==================================
// PEDUMP - Matt Pietrek 1998-2001
// FILE: CVSymbols.CPP
//==================================
#include <windows.h>
#include <stdio.h>
#include <imagehlp.h>
#include <string.h>
#include "common.h"
#include "cvexefmt.h"
#include "CVInfoNew.h"	// Our own definitions for "newer" CV info

void DecodeDataSym32( DATASYM32 * pDataSym32, PSTR szOutBuffer );
void DecodeDataSym32_New( DATASYM32_NEW *, PSTR szOutBuffer );
PSTR GetSubsectionTypeName( unsigned short sst );
PSTR GetSymRecordName( SYM_ENUM_e symrectype );



void DisplaySSTModule( PBYTE pRawData, DWORD cb )
{
	OMFModule * pOMFModule = (OMFModule*)pRawData;
	
	// Pointer math below!!!
	PBYTE pcbName = (PBYTE)(&pOMFModule->SegInfo + pOMFModule->cSeg);

	printf( "  Name: %-*s\n",  *pcbName, pcbName + 1 );
		
    printf( "  ovlNumber: %X  iLib: %X  cSeg: %X  Style: %c%c\n",
		pOMFModule->ovlNumber, pOMFModule->iLib, pOMFModule->cSeg,
		pOMFModule->Style[0], pOMFModule->Style[1] );
	
	for ( unsigned i = 0; i < pOMFModule->cSeg; i++ )
	{
	    printf( "  SegInfo[%X] Seg: %X  pad: %X  Off:% X  cbSeg: %X\n",
			i,
			pOMFModule->SegInfo[i].Seg,
			pOMFModule->SegInfo[i].pad,
			pOMFModule->SegInfo[i].Off,
			pOMFModule->SegInfo[i].cbSeg );
	}
}

void DisplaySSTGlobalPub( PBYTE pRawData, DWORD cb )
{
	OMFSymHash * pOMFSymHash = (OMFSymHash*)pRawData;

    printf( "symhash: %X\n", pOMFSymHash->symhash );
    printf( "addrhash: %X\n", pOMFSymHash->addrhash );
    printf( "cbSymbol: %X\n", pOMFSymHash->cbSymbol );
    printf( "cbHSym: %X\n", pOMFSymHash->cbHSym );
    printf( "cbHAddr: %X\n", pOMFSymHash->cbHAddr );

	unsigned long bytesLeft = pOMFSymHash->cbSymbol;
	SYMTYPE * pSymbol = (SYMTYPE*)(pRawData + sizeof(OMFSymHash));
	
	while ( bytesLeft )
	{
		char szSymDetail[512];
		
		switch( pSymbol->rectyp )
		{
			case S_LDATA32:
			case S_GDATA32:
			case S_PUB32:
				DecodeDataSym32( (DATASYM32 *)pSymbol, szSymDetail );
				break;
			case S_PUBSYM32_NEW:
				DecodeDataSym32_New( (DATASYM32_NEW *)pSymbol, szSymDetail );
				break;
			default:
				szSymDetail[0] = 0;
		}
		 
		printf( "  %s %s\n", GetSymRecordName((SYM_ENUM_e)pSymbol->rectyp),
				szSymDetail );
		
		bytesLeft -= (pSymbol->reclen + sizeof(unsigned short));
		pSymbol = NextSym( pSymbol );
	}	
}

void DisplaySSTSegMap( PBYTE pRawData, DWORD cb )
{
	OMFSegMap * pOMFSegMap = (OMFSegMap*)pRawData;

	printf( "cSeg: %X\n", pOMFSegMap->cSeg );
	printf( "cSegLog: %X\n", pOMFSegMap->cSegLog );

	OMFSegMapDesc * pOMFSegMapDesc = pOMFSegMap->rgDesc;
	
	for ( unsigned i=0; i < pOMFSegMap->cSeg; i++ )
	{
		printf( "\n" );
    	printf( "  flags: %X\n", pOMFSegMapDesc->flags );
    	printf( "  ovl: %X\n", pOMFSegMapDesc->ovl );
    	printf( "  group: %X\n", pOMFSegMapDesc->group );
    	printf( "  frame: %X\n", pOMFSegMapDesc->frame );
    	printf( "  iSegName: %X\n", pOMFSegMapDesc->iSegName );
    	printf( "  iClassName: %X\n", pOMFSegMapDesc->iClassName );
    	printf( "  offset: %X\n", pOMFSegMapDesc->offset );
    	printf( "  cbSeg: %X\n", pOMFSegMapDesc->cbSeg );
		printf( "\n" );
	
		pOMFSegMapDesc++;
	}
}

void DisplayCVSubsection( unsigned short sst, PBYTE pRawData, DWORD cb, WORD dirIndex )
{
	printf( "DirectoryEntry: %u %s\n", dirIndex, GetSubsectionTypeName(sst) );
	
	switch ( sst )
	{
		case sstModule: DisplaySSTModule( pRawData, cb ); break;
		case sstGlobalPub: DisplaySSTGlobalPub( pRawData, cb ); break;
		case sstSegMap: DisplaySSTSegMap( pRawData, cb ); break;
	}
}

BOOL DumpCVSymbolTable( PBYTE pRawCVSymbolData, PBYTE pFileBase )
{
	printf( "== CodeView Info at offset %08X==\n",
		pRawCVSymbolData - pFileBase );

	OMFSignature * pOMFSignature = (OMFSignature*)pRawCVSymbolData;
	
	printf( "Signature: %c%c%c%c\n",
			pOMFSignature->Signature[0], pOMFSignature->Signature[1],
			pOMFSignature->Signature[2], pOMFSignature->Signature[3] );
	printf( "lfoDirectory: %08X\n", pOMFSignature->filepos);

	printf( "\n" );
	
	PBYTE lfaBase = pRawCVSymbolData;
	
	OMFDirHeader * pOMFDirHeader =
		(OMFDirHeader *)(pRawCVSymbolData + pOMFSignature->filepos);

	printf( "OMFDirHeader:\n" );

    printf( "  cbDirHeader: %X\n", pOMFDirHeader->cbDirHeader );
    printf( "  cbDirEntry: %X\n", pOMFDirHeader->cbDirEntry );
    printf( "  cDir: %X\n", pOMFDirHeader->cDir );
    printf( "  lfoNextDir: %X\n", pOMFDirHeader->lfoNextDir );
    printf( "  flags: %X\n", pOMFDirHeader->flags );

	printf( "\n" );
	printf( "OMFDirEntries:\n" );
	
	OMFDirEntry * pOMFDirEntry = (OMFDirEntry *)(pOMFDirHeader+1);

	for ( unsigned i = 0; i < pOMFDirHeader->cDir; i++ )
	{
		printf( "\n" );

	    printf( "  SubSection: %X (%s)\n",
			pOMFDirEntry->SubSection,
			GetSubsectionTypeName(pOMFDirEntry->SubSection) );
	    printf( "  iMod: %X\n", pOMFDirEntry->iMod );
	    printf( "  lfo: %X\n", pOMFDirEntry->lfo );
	    printf( "  cb: %X\n", pOMFDirEntry->cb );
		
		pOMFDirEntry++;
	}

	pOMFDirEntry = (OMFDirEntry *)(pOMFDirHeader+1);

	for ( i = 0; i < pOMFDirHeader->cDir; i++ )
	{
		printf( "\n" );

		DisplayCVSubsection( pOMFDirEntry->SubSection,
							 pRawCVSymbolData + pOMFDirEntry->lfo,
							 pOMFDirEntry->cb,
							 i+1 );
		pOMFDirEntry++;
	}
		
	return TRUE;
}

void DecodeDataSym32( DATASYM32 * pDataSym32, PSTR szOutBuffer )
{
    char szSymbolName[512];

    PCSTR pszUndecoratedName = (PCSTR)&pDataSym32->name[1];

    if ( '?' == pDataSym32->name[1] )
    {
        UnDecorateSymbolName(   pszUndecoratedName,
                                szSymbolName,
                                sizeof(szSymbolName),
                                UNDNAME_NO_LEADING_UNDERSCORES   |
                                UNDNAME_NO_MS_KEYWORDS           |
                                UNDNAME_NO_FUNCTION_RETURNS      |
                                UNDNAME_NO_ALLOCATION_MODEL      |
                                UNDNAME_NO_ALLOCATION_LANGUAGE   |
                                UNDNAME_NO_MS_THISTYPE           |
                                UNDNAME_NO_CV_THISTYPE           |
                                UNDNAME_NO_THISTYPE              |
                                UNDNAME_NO_ACCESS_SPECIFIERS     |
                                UNDNAME_NO_THROW_SIGNATURES      |
                                UNDNAME_NO_MEMBER_TYPE           |
                                UNDNAME_NO_RETURN_UDT_MODEL      |
                                UNDNAME_32_BIT_DECODE );

        if ( '?' == szSymbolName[0] )
        {
            PSTR pszFirstAt, pszTwoAts;
            pszFirstAt = strchr( pszUndecoratedName, '@' );
            pszTwoAts = strstr( pszUndecoratedName, "@@" );
            
            if ( pszFirstAt && pszTwoAts )
            {
                sprintf( szSymbolName, "%.*s::%.*s(%s)",
                        pszTwoAts - pszFirstAt - 1,
                        pszFirstAt + 1,
                        pszFirstAt - pszUndecoratedName - 1,
                        pszUndecoratedName + 1,
                        pszTwoAts + 2 );
            }
        }
    }
    else    // Just copy the length prefixed name into the szSymbolName variable
    {
        strncpy( szSymbolName, (char *)&pDataSym32->name[1], pDataSym32->name[0] );
        szSymbolName[pDataSym32->name[0]] = 0;  // Null terminate it
    }

	sprintf( szOutBuffer, "%04X:%08X type:%X %s",
		pDataSym32->seg, pDataSym32->off, pDataSym32->typind, szSymbolName );
}

void DecodeDataSym32_New( DATASYM32_NEW * pDataSym32_New, PSTR szOutBuffer )
{
	sprintf( szOutBuffer, "%04X:%08X type:%X %.*s",
		pDataSym32_New->seg, pDataSym32_New->off, pDataSym32_New->typind,
		pDataSym32_New->name[0], &pDataSym32_New->name[1] );
}

PSTR GetSubsectionTypeName( unsigned short sst )
{
	#define CASE_STRING( x ) case x: s = #x; break;

	PSTR s = "<unknown>";

	switch( sst )
	{
		CASE_STRING( sstModule )
		CASE_STRING( sstTypes )
		CASE_STRING( sstPublic )
		CASE_STRING( sstPublicSym )
		CASE_STRING( sstSymbols )
		CASE_STRING( sstAlignSym )
		CASE_STRING( sstSrcLnSeg )
		CASE_STRING( sstSrcModule )
		CASE_STRING( sstLibraries )
		CASE_STRING( sstGlobalSym )
		CASE_STRING( sstGlobalPub )
		CASE_STRING( sstGlobalTypes )
		CASE_STRING( sstMPC )
		CASE_STRING( sstSegMap )
		CASE_STRING( sstSegName )
		CASE_STRING( sstPreComp )
		CASE_STRING( sstPreCompMap )
		CASE_STRING( sstOffsetMap16 )
		CASE_STRING( sstOffsetMap32 )
		CASE_STRING( sstFileIndex )
		CASE_STRING( sstStaticSym )
	}
	
	return s;
}

PSTR GetSymRecordName( SYM_ENUM_e symrectype )
{
	PSTR s = "<unknown>";

	switch( symrectype )
	{
    	CASE_STRING( S_COMPILE )
    	CASE_STRING( S_REGISTER )
    	CASE_STRING( S_CONSTANT )
    	CASE_STRING( S_UDT )
    	CASE_STRING( S_SSEARCH )
    	CASE_STRING( S_END )
    	CASE_STRING( S_SKIP )
    	CASE_STRING( S_CVRESERVE )
    	CASE_STRING( S_OBJNAME )
    	CASE_STRING( S_ENDARG )
    	CASE_STRING( S_COBOLUDT )
    	CASE_STRING( S_MANYREG )
    	CASE_STRING( S_RETURN )
    	CASE_STRING( S_ENTRYTHIS )
    	CASE_STRING( S_BPREL16 )
    	CASE_STRING( S_LDATA16 )
    	CASE_STRING( S_GDATA16 )
    	CASE_STRING( S_PUB16 )
    	CASE_STRING( S_LPROC16 )
    	CASE_STRING( S_GPROC16 )
    	CASE_STRING( S_THUNK16 )
    	CASE_STRING( S_BLOCK16 )
    	CASE_STRING( S_WITH16 )
    	CASE_STRING( S_LABEL16 )
    	CASE_STRING( S_CEXMODEL16 )
    	CASE_STRING( S_VFTABLE16 )
    	CASE_STRING( S_REGREL16 )
    	CASE_STRING( S_BPREL32 )
    	CASE_STRING( S_LDATA32 )
    	CASE_STRING( S_GDATA32 )
    	CASE_STRING( S_PUB32 )
    	CASE_STRING( S_LPROC32 )
    	CASE_STRING( S_GPROC32 )
    	CASE_STRING( S_THUNK32 )
    	CASE_STRING( S_BLOCK32 )
    	CASE_STRING( S_WITH32 )
    	CASE_STRING( S_LABEL32 )
    	CASE_STRING( S_CEXMODEL32 )
    	CASE_STRING( S_VFTABLE32 )
    	CASE_STRING( S_REGREL32 )
    	CASE_STRING( S_LTHREAD32 )
    	CASE_STRING( S_GTHREAD32 )
    	CASE_STRING( S_SLINK32 )
    	CASE_STRING( S_LPROCMIPS )
    	CASE_STRING( S_GPROCMIPS )
    	CASE_STRING( S_PROCREF )
    	CASE_STRING( S_DATAREF )
    	CASE_STRING( S_ALIGN )
    	CASE_STRING( S_LPROCREF )
		CASE_STRING( S_PUBSYM32_NEW )
		default:
		{
			static char szRetBuffer[128];
			
			sprintf( szRetBuffer, "<Unknown:%X>", symrectype );
			s = szRetBuffer;
		}
	}

	return s;
}
