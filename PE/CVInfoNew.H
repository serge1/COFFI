//
// Newer CV info defined after 1994.  CVINFO.H doesn't appear to have been
// publicly updated since 1994.  However, the MSDN doc describes newer CV
// style CV info, which includes:
//	- 4 byte type indices
//	- rearranged structure layouts to reduce # of variable length fields
//

#define S_PUBSYM32_NEW	0x1009

typedef unsigned long CVTYPEINDEX;

typedef struct DATASYM32_NEW {
    unsigned short  reclen;         // Record length
    unsigned short  rectyp;         // S_LDATA32, S_GDATA32 or S_PUB32
	CVTYPEINDEX		typind;
	unsigned long   off;
    unsigned short  seg;
    unsigned char   name[1];        // Length-prefixed name
} DATASYM32_NEW;

typedef DATASYM32_NEW PUBSYM32_NEW;
