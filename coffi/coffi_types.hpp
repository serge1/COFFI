/*
Copyright (C) 2014-2014 by Serge Lamikhov-Center

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/*! @file coffi_types.hpp
 * @brief COFFI library basic structures and types.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_TYPES_HPP
#define COFFI_TYPES_HPP

#include <string>

namespace COFFI {

#if !defined(COFFI_NO_OWN_TYPES) && !defined(COFFI_NO_CSTDINT) && \
    !defined(COFFI_NO_INTTYPES) && !defined(DOXYGEN)
#include <stdint.h>
#else
typedef unsigned char uint8_t;
typedef signed char   int8_t;
typedef uint16_t      uint16_t;
typedef signed short  int16_t;
#ifdef _MSC_VER
typedef unsigned __int32 uint32_t;
typedef signed __int32   int32_t;
typedef unsigned __int64 uint64_t;
typedef signed __int64   int64_t;
#else
typedef unsigned int       uint32_t;
typedef signed int         int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;
#endif // _MSC_VER
#endif

/*! @brief COFF symbol names size
 *
 * The name field in a symbol table consists of eight bytes that contain the name itself, if not too long, or else give an offset into the string table.
 * See COFFI::coffi_strings.
 */
#define COFFI_NAME_SIZE 8

//! @name Identification index
//! @{
#define CI_MAG0    0
#define CI_MAG1    1
#define CI_MAG2    0
#define CI_MAG3    1
#define CI_MAG4    2
#define CI_MAG5    3
#define CI_NIDENT0 2
#define CI_NIDENT1 4

//! @}

//! @name Magic numbers
//! @{
#define PEMAG0 'M'
#define PEMAG1 'Z'
#define PEMAG2 'P'
#define PEMAG3 'E'
#define PEMAG4 0
#define PEMAG5 0

//! @}

//! @name Magic numbers of optional header
//! @{
#define OH_MAGIC_PE32     0x10B //!< PE32 format
#define OH_MAGIC_PE32ROM  0x107
#define OH_MAGIC_PE32PLUS 0x20B //!< PE32+ format

//! @}

// clang-format off
//! @name PE file characteristics
//! @{
#define IMAGE_FILE_RELOCS_STRIPPED         0x0001 //!< Image only, Windows CE, Windows NT and above. Indicates that the file does not contain base relocations and must therefore be loaded at its preferred base address. If the base address is not available, the loader reports an error. Operating systems running on top of MS-DOS (Win32s™) are generally not able to use the preferred base address and so cannot run these images. However, beginning with version 4.0, Windows will use an application’s preferred base address. The default behavior of the linker is to strip base relocations from EXEs.
#define IMAGE_FILE_EXECUTABLE_IMAGE        0x0002 //!< Image only. Indicates that the image file is valid and can be run. If this flag is not set, it generally indicates a linker error.
#define IMAGE_FILE_LINE_NUMS_STRIPPED      0x0004 //!< COFF line numbers have been removed.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED     0x0008 //!< COFF symbol table entries for local symbols have been removed
#define IMAGE_FILE_AGGRESSIVE_WS_TRIM      0x0010 //!< Aggressively trim working set.
#define IMAGE_FILE_LARGE_ADDRESS_AWARE     0x0020 //!< App can handle > 2gb addresses.
#define IMAGE_FILE_16BIT_MACHINE           0x0040 //!< Use of this flag is reserved for future use.
#define IMAGE_FILE_BYTES_REVERSED_LO       0x0080 //!< Little endian: LSB precedes MSB in memory.
#define IMAGE_FILE_32BIT_MACHINE           0x0100 //!< Machine based on 32-bit-word architecture.
#define IMAGE_FILE_DEBUG_STRIPPED          0x0200 //!< Debugging information removed from image file.
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP 0x0400 //!< If image is on removable media, copy and run from swap file.
#define IMAGE_FILE_SYSTEM                  0x1000 //!< The image file is a system file, not a user program.
#define IMAGE_FILE_DLL                     0x2000 //!< The image file is a dynamic-link library (DLL). Such files are considered executable files for almost all purposes, although they cannot be directly run.
#define IMAGE_FILE_UP_SYSTEM_ONLY          0x4000 //!< File should be run only on a UP machine.
#define IMAGE_FILE_BYTES_REVERSED_HI       0x8000 //!< Big endian: MSB precedes LSB in memory.

//! @}

//! @name PE machine Types
//! @{
#define IMAGE_FILE_MACHINE_UNKNOWN   0x0000  //!< The contents of this field are assumed to be applicable to any machine type
#define IMAGE_FILE_MACHINE_AM33      0x01d3  //!< Matsushita AM33
#define IMAGE_FILE_MACHINE_AMD64     0x8664  //!< x64
#define IMAGE_FILE_MACHINE_ARM       0x01c0  //!< ARM little endian
#define IMAGE_FILE_MACHINE_ARMNT     0x01c4  //!< ARMv7( or higher ) Thumb mode only
#define IMAGE_FILE_MACHINE_ARM64     0xaa64  //!< ARMv8 in 64 - bit mode
#define IMAGE_FILE_MACHINE_EBC       0x0ebc  //!< EFI byte code
#define IMAGE_FILE_MACHINE_I386      0x014c  //!< Intel 386 or later processors and compatible processors
#define IMAGE_FILE_MACHINE_IA64      0x0200  //!< Intel Itanium processor family
#define IMAGE_FILE_MACHINE_M32R      0x9041  //!< Mitsubishi M32R little endian
#define IMAGE_FILE_MACHINE_MIPS16    0x0266  //!< MIPS16
#define IMAGE_FILE_MACHINE_MIPSFPU   0x0366  //!< MIPS with FPU
#define IMAGE_FILE_MACHINE_MIPSFPU16 0x0466  //!< MIPS16 with FPU
#define IMAGE_FILE_MACHINE_POWERPC   0x01f0  //!< Power PC little endian
#define IMAGE_FILE_MACHINE_POWERPCFP 0x01f1  //!< Power PC with floating point support
#define IMAGE_FILE_MACHINE_R4000     0x0166  //!< MIPS little endian
#define IMAGE_FILE_MACHINE_SH3       0x01a2  //!< Hitachi SH3
#define IMAGE_FILE_MACHINE_SH3DSP    0x01a3  //!< Hitachi SH3 DSP
#define IMAGE_FILE_MACHINE_SH4       0x01a6  //!< Hitachi SH4
#define IMAGE_FILE_MACHINE_SH5       0x01a8  //!< Hitachi SH5
#define IMAGE_FILE_MACHINE_THUMB     0x01c2  //!< ARM or Thumb( "interworking" )
#define IMAGE_FILE_MACHINE_WCEMIPSV2 0x0169  //!< MIPS little - endian WCE v2

//! @}

//! @name PE section flags
//! @{
#define IMAGE_SCN_TYPE_NO_PAD            0x00000008 //!< The section should not be padded to the next boundary.This flag is obsolete and is replaced by IMAGE_SCN_ALIGN_1BYTES.This is valid only for object files.
#define IMAGE_SCN_CNT_CODE               0x00000020 //!< The section contains executable code.
#define IMAGE_SCN_CNT_INITIALIZED_DATA   0x00000040 //!< The section contains initialized data.
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080 //!< The section contains uninitialized data.
#define IMAGE_SCN_LNK_OTHER              0x00000100 //!< Reserved for future use.
#define IMAGE_SCN_LNK_INFO               0x00000200 //!< The section contains comments or other information.The.drectve section has this type.This is valid for object files only.
#define IMAGE_SCN_LNK_REMOVE             0x00000800 //!< The section will not become part of the image.This is valid only for object files.
#define IMAGE_SCN_LNK_COMDAT             0x00001000 //!< The section contains COMDAT data.For more information, see section 5.5.6, "COMDAT Sections( Object Only )." This is valid only for object files.
#define IMAGE_SCN_GPREL                  0x00008000 //!< The section contains data referenced through the global pointer( GP ).
#define IMAGE_SCN_MEM_PURGEABLE          0x00020000 //!< Reserved for future use.
#define IMAGE_SCN_MEM_16BIT              0x00020000 //!< For ARM machine types, the section contains Thumb code.Reserved for future use with other machine types.
#define IMAGE_SCN_MEM_LOCKED             0x00040000 //!< Reserved for future use.
#define IMAGE_SCN_MEM_PRELOAD            0x00080000 //!< Reserved for future use.
#define IMAGE_SCN_ALIGN_1BYTES           0x00100000 //!< Align data on a 1 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_2BYTES           0x00200000 //!< Align data on a 2 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_4BYTES           0x00300000 //!< Align data on a 4 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_8BYTES           0x00400000 //!< Align data on an 8 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_16BYTES          0x00500000 //!< Align data on a 16 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_32BYTES          0x00600000 //!< Align data on a 32 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_64BYTES          0x00700000 //!< Align data on a 64 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_128BYTES         0x00800000 //!< Align data on a 128 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_256BYTES         0x00900000 //!< Align data on a 256 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_512BYTES         0x00A00000 //!< Align data on a 512 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_1024BYTES        0x00B00000 //!< Align data on a 1024 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_2048BYTES        0x00C00000 //!< Align data on a 2048 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_4096BYTES        0x00D00000 //!< Align data on a 4096 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_8192BYTES        0x00E00000 //!< Align data on an 8192 - byte boundary.Valid only for object files.
#define IMAGE_SCN_LNK_NRELOC_OVFL        0x01000000 //!< The section contains extended relocations.
#define IMAGE_SCN_MEM_DISCARDABLE        0x02000000 //!< The section can be discarded as needed.
#define IMAGE_SCN_MEM_NOT_CACHED         0x04000000 //!< The section cannot be cached.
#define IMAGE_SCN_MEM_NOT_PAGED          0x08000000 //!< The section is not pageable.
#define IMAGE_SCN_MEM_SHARED             0x10000000 //!< The section can be shared in memory.
#define IMAGE_SCN_MEM_EXECUTE            0x20000000 //!< The section can be executed as code.
#define IMAGE_SCN_MEM_READ               0x40000000 //!< The section can be read.
#define IMAGE_SCN_MEM_WRITE              0x80000000 //!< The section can be written to.

//! @}

//! @name Portable Executable directories index
//! @{
#define DIRECTORY_EXPORT_TABLE             0
#define DIRECTORY_IMPORT_TABLE             1
#define DIRECTORY_RESOURCE_TABLE           2
#define DIRECTORY_EXCEPTION_TABLE          3
#define DIRECTORY_CERTIFICATE_TABLE        4
#define DIRECTORY_BASE_RELOCATION_TABLE    5
#define DIRECTORY_DEBUG                    6
#define DIRECTORY_ARCHITECTURE             7
#define DIRECTORY_GLOBAL_PTR               8
#define DIRECTORY_TLS_TABLE                9
#define DIRECTORY_LOAD_CONFIG_TABLE       10
#define DIRECTORY_BOUND_IMPORT            11
#define DIRECTORY_IAT                     12
#define DIRECTORY_DELAY_IMPORT_DESCRIPTOR 13
#define DIRECTORY_COMPLUS_RUNTIME_HEADER  14
#define DIRECTORY_RESERVED                15

//! @}

//! @name COFF Relocations Type Indicators - Intel 386
//! @{
#define IMAGE_REL_I386_ABSOLUTE           0x0000 //!< This relocation is ignored.
#define IMAGE_REL_I386_DIR16              0x0001 //!< Not supported.
#define IMAGE_REL_I386_REL16              0x0002 //!< Not supported.
#define IMAGE_REL_I386_DIR32              0x0006 //!< The target’s 32-bit virtual address.
#define IMAGE_REL_I386_DIR32NB            0x0007 //!< The target’s 32-bit relative virtual address.
#define IMAGE_REL_I386_SEG12              0x0009 //!< Not supported.
#define IMAGE_REL_I386_SECTION            0x000A //!< The 16-bit-section index of the section containing the target. This is used to support debugging information.
#define IMAGE_REL_I386_SECREL             0x000B //!< The 32-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.
#define IMAGE_REL_I386_REL32              0x0014 //!< The 32-bit relative displacement to the target. This supports the x86 relative branch and call instructions.

//! @}

//! @name COFF Relocations Type Indicators - MIPS Processors
//! @{
#define IMAGE_REL_MIPS_ABSOLUTE           0x0000 //!< This relocation is ignored.
#define IMAGE_REL_MIPS_REFHALF            0x0001 //!< The high 16 bits of the target’s 32-bit virtual address.
#define IMAGE_REL_MIPS_REFWORD            0x0002 //!< The target’s 32-bit virtual address.
#define IMAGE_REL_MIPS_JMPADDR            0x0003 //!< The low 26 bits of the target’s virtual address. This supports the MIPS J and JAL instructions.
#define IMAGE_REL_MIPS_REFHI              0x0004 //!< The high 16 bits of the target’s 32-bit virtual address. Used for the first instruction in a two-instruction sequence that loads a full address. This relocation must be immediately followed by a PAIR relocations whose SymbolTableIndex contains a signed 16-bit displacement which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_MIPS_REFLO              0x0005 //!< The low 16 bits of the target’s virtual address.
#define IMAGE_REL_MIPS_GPREL              0x0006 //!< 16-bit signed displacement of the target relative to the Global Pointer (GP) register.
#define IMAGE_REL_MIPS_LITERAL            0x0007 //!< Same as IMAGE_REL_MIPS_GPREL.
#define IMAGE_REL_MIPS_SECTION            0x000A //!< The 16-bit section index of the section containing the target. This is used to support debugging information.
#define IMAGE_REL_MIPS_SECREL             0x000B //!< The 32-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.
#define IMAGE_REL_MIPS_SECRELLO           0x000C //!< The low 16 bits of the 32-bit offset of the target from the beginning of its section.
#define IMAGE_REL_MIPS_SECRELHI           0x000D //!< The high 16 bits of the 32-bit offset of the target from the beginning of its section. A AIR relocation must immediately follow this on. The SymbolTableIndex of the PAIR relocation contains a signed 16-bit displacement, which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_MIPS_JMPADDR16          0x0010 //!< The low 26 bits of the target’s virtual address. This supports the MIPS16 JAL instruction.
#define IMAGE_REL_MIPS_REFWORDNB          0x0022 //!< The target’s 32-bit relative virtual address.
#define IMAGE_REL_MIPS_PAIR               0x0025 //!< This relocation is only valid when it immediately follows a REFHI or SECRELHI relocation. Its SymbolTableIndex contains a displacement and not an index into the symbol table.

//! @}

//! @name COFF Relocations Type Indicators - Alpha Processors
//! @{
#define IMAGE_REL_ALPHA_ABSOLUTE          0x0000 //!< This relocation is ignored.
#define IMAGE_REL_ALPHA_REFLONG           0x0001 //!< The target’s 32-bit virtual address. This fixup is illegal in a PE32+ image unless the image has been sandboxed by clearing the IMAGE_FILE_LARGE_ADDRESS_AWARE bit in the File Header.
#define IMAGE_REL_ALPHA_REFQUAD           0x0002 //!< The target’s 64-bit virtual address.
#define IMAGE_REL_ALPHA_GPREL32           0x0003 //!< 32-bit signed displacement of the target relative to the Global Pointer (GP) register.
#define IMAGE_REL_ALPHA_LITERAL           0x0004 //!< 16-bit signed displacement of the target relative to the Global Pointer (GP) register.
#define IMAGE_REL_ALPHA_LITUSE            0x0005 //!< Reserved for future use.
#define IMAGE_REL_ALPHA_GPDISP            0x0006 //!< Reserved for future use.
#define IMAGE_REL_ALPHA_BRADDR            0x0007 //!< The 21-bit relative displacement to the target. This supports the Alpha relative branch instructions.
#define IMAGE_REL_ALPHA_HINT              0x0008 //!< 14-bit hints to the processor for the target of an Alpha jump instruction.
#define IMAGE_REL_ALPHA_INLINE_REFLONG    0x0009 //!< The target’s 32-bit virtual address split into high and low 16-bit parts. Either an ABSOLUTE or MATCH relocation must immediately follow this relocation. The high 16 bits of the target address are stored in the location identified by the INLINE_REFLONG relocation. The low 16 bits are stored four bytes later if the following relocation is of type ABSOLUTE or at a signed displacement given in the SymbolTableIndex if the following relocation is of type MATCH.
#define IMAGE_REL_ALPHA_REFHI             0x000A //!< The high 16 bits of the target’s 32-bit virtual address. Used for the first instruction in a two-instruction sequence that loads a full address. This relocation must be immediately followed by a PAIR relocations whose SymbolTableIndex contains a signed 16-bit displacement which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_ALPHA_REFLO             0x000B //!< The low 16 bits of the target’s virtual address.
#define IMAGE_REL_ALPHA_PAIR              0x000C //!< This relocation is only valid when it immediately follows a REFHI , REFQ3, REFQ2, or SECRELHI relocation. Its SymbolTableIndex contains a displacement and not an index into the symbol table.
#define IMAGE_REL_ALPHA_MATCH             0x000D //!< This relocation is only valid when it immediately follows INLINE_REFLONG relocation. Its SymbolTableIndex contains the displacement in bytes of the location for the matching low address and not an index into the symbol table.
#define IMAGE_REL_ALPHA_SECTION           0x000E //!< The 16-bit section index of the section containing the target. This is used to support debugging information.
#define IMAGE_REL_ALPHA_SECREL            0x000F //!< The 32-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.
#define IMAGE_REL_ALPHA_REFLONGNB         0x0010 //!< The target’s 32-bit relative virtual address. IMAGE_REL_ALPHA_SECRELLO 0x0011 The low 16 bits of the 32-bit offset of the target from the beginning of its section.
#define IMAGE_REL_ALPHA_SECRELHI          0x0012 //!< The high 16 bits of the 32-bit offset of the target from the beginning of its section. A PAIR relocation must immediately follow this on. The SymbolTableIndex of the PAIR relocation contains a signed 16-bit displacement which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_ALPHA_REFQ3             0x0013 //!< The low 16 bits of the high 32 bits of the target’s 64-bit virtual address. This relocation must be immediately followed by a PAIR relocations whose SymbolTableIndex contains a signed 32-bit displacement which is added to the 16 bits taken from the location being relocated. The 16 bits in the relocated location are shifted left by 32 before this addition.
#define IMAGE_REL_ALPHA_REFQ2             0x0014 //!< The high 16 bits of the low 32 bits of the target’s 64-bit virtual address. This relocation must be immediately followed by a PAIR relocations whose SymbolTableIndex contains a signed 16-bit displacement which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_ALPHA_REFQ1             0x0015 //!< The low 16 bits of the target’s 64-bit virtual address.
#define IMAGE_REL_ALPHA_GPRELLO           0x0016 //!< The low 16 bits of the 32-bit signed displacement of the target relative to the Global Pointer (GP) register.
#define IMAGE_REL_ALPHA_GPRELHI           0x0017 //!< The high 16 bits of the 32-bit signed displacement of the target relative to the Global Pointer (GP) register.

//! @}

//! @name COFF Relocations Type Indicators - IBM PowerPC Processors
//! @{
#define IMAGE_REL_PPC_ABSOLUTE            0x0000 //!< This relocation is ignored.
#define IMAGE_REL_PPC_ADDR64              0x0001 //!< The target’s 64-bit virtual address.
#define IMAGE_REL_PPC_ADDR32              0x0002 //!< The target’s 32-bit virtual address.
#define IMAGE_REL_PPC_ADDR24              0x0003 //!< The low 24 bits of the target’s virtual address. This is only valid when the target symbol is absolute and can be sign extended to its original value.
#define IMAGE_REL_PPC_ADDR16              0x0004 //!< The low 16 bits of the target’s virtual address.
#define IMAGE_REL_PPC_ADDR14              0x0005 //!< The low 14 bits of the target’s virtual address. This is only valid when the target symbol is absolute and can be sign extended to its original value.
#define IMAGE_REL_PPC_REL24               0x0006 //!< A 24-bit PC-relative offset to the symbol’s location.
#define IMAGE_REL_PPC_REL14               0x0007 //!< A 14-bit PC-relative offset to the symbol’s location.
#define IMAGE_REL_PPC_ADDR32NB            0x000A //!< The target’s 32-bit relative virtual address.
#define IMAGE_REL_PPC_SECREL              0x000B //!< The 32-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.
#define IMAGE_REL_PPC_SECTION             0x000C //!< The 16-bit section index of the section containing the target. This is used to support debugging information.
#define IMAGE_REL_PPC_SECREL16            0x000F //!< The 16-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.
#define IMAGE_REL_PPC_REFHI               0x0010 //!< The high 16 bits of the target’s 32-bit virtual address. Used for the first instruction in a two-instruction sequence that loads a full address. This relocation must be immediately followed by a PAIR relocations whose SymbolTableIndex contains a signed 16-bit displacement which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_PPC_REFLO               0x0011 //!< The low 16 bits of the target’s virtual address.
#define IMAGE_REL_PPC_PAIR                0x0012 //!< This relocation is only valid when it immediately follows a REFHI or SECRELHI relocation. Its SymbolTableIndex contains a displacement and not an index into the symbol table.
#define IMAGE_REL_PPC_SECRELLO            0x0013 //!< The low 16 bits of the 32-bit offset of the target from the beginning of its section.
#define IMAGE_REL_PPC_SECRELHI            0x0014 //!< The high 16 bits of the 32-bit offset of the target from the beginning of its section. A PAIR relocation must immediately follow this on. The SymbolTableIndex of the PAIR relocation contains a signed 16-bit displacement which is added to the upper 16 bits taken from the location being relocated.
#define IMAGE_REL_PPC_GPREL               0x0015 //!< 16-bit signed displacement of the target relative to the Global Pointer (GP) register.

//! @}

//! @name COFF Relocations Type Indicators - Hitachi SuperH Processors
//! @{
#define IMAGE_REL_SH3_ABSOLUTE            0x0000 //!< This relocation is ignored.
#define IMAGE_REL_SH3_DIRECT16            0x0001 //!< Reference to the 16-bit location that contains the virtual address of the target symbol.
#define IMAGE_REL_SH3_DIRECT32            0x0002 //!< The target’s 32-bit virtual address.
#define IMAGE_REL_SH3_DIRECT8             0x0003 //!< Reference to the 8-bit location that contains the virtual address of the target symbol.
#define IMAGE_REL_SH3_DIRECT8_WORD        0x0004 //!< Reference to the 8-bit instruction that contains the effective 16-bit virtual address of the target symbol.
#define IMAGE_REL_SH3_DIRECT8_LONG        0x0005 //!< Reference to the 8-bit instruction that contains the effective 32-bit virtual address of the target symbol.
#define IMAGE_REL_SH3_DIRECT4             0x0006 //!< Reference to the 8-bit location whose low 4 bits contain the virtual address of the target symbol.
#define IMAGE_REL_SH3_DIRECT4_WORD        0x0007 //!< Reference to the 8-bit instruction whose low 4 bits contain the effective 16-bit virtual address of the target symbol.
#define IMAGE_REL_SH3_DIRECT4_LONG        0x0008 //!< Reference to the 8-bit instruction whose low 4 bits contain the effective 32-bit virtual address of the target symbol.
#define IMAGE_REL_SH3_PCREL8_WORD         0x0009 //!< Reference to the 8-bit instruction which contains the effective 16-bit relative offset of the target symbol.
#define IMAGE_REL_SH3_PCREL8_LONG         0x000A //!< Reference to the 8-bit instruction which contains the effective 32-bit relative offset of the target symbol.
#define IMAGE_REL_SH3_PCREL12_WORD        0x000B //!< Reference to the 16-bit instruction whose low 12 bits contain the effective 16-bit relative offset of the target symbol.
#define IMAGE_REL_SH3_STARTOF_SECTION     0x000C //!< Reference to a 32-bit location that is the virtual address of the symbol’s section.
#define IMAGE_REL_SH3_SIZEOF_SECTION      0x000D //!< Reference to the 32-bit location that is the size of the symbol’s section.
#define IMAGE_REL_SH3_SECTION             0x000E //!< The 16-bit section index of the section containing the target. This is used to support debugging information.
#define IMAGE_REL_SH3_SECREL              0x000F //!< The 32-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.
#define IMAGE_REL_SH3_DIRECT32_NB         0x0010 //!< The target’s 32-bit relative virtual address.

//! @}

//! @name COFF Relocations Type Indicators - ARM Processors
//! @{
#define IMAGE_REL_ARM_ABSOLUTE            0x0000 //!< This relocation is ignored.
#define IMAGE_REL_ARM_ADDR32              0x0001 //!< The target’s 32-bit virtual address.
#define IMAGE_REL_ARM_ADDR32NB            0x0002 //!< The target’s 32-bit relative virtual address.
#define IMAGE_REL_ARM_BRANCH24            0x0003 //!< The 24-bit relative displacement to the target.
#define IMAGE_REL_ARM_BRANCH11            0x0004 //!< Reference to a subroutine call, consisting of two 16-bit instructions with 11-bit offsets.
#define IMAGE_REL_ARM_SECTION             0x000E //!< The 16-bit section index of the section containing the target. This is used to support debugging information.
#define IMAGE_REL_ARM_SECREL              0x000F //!< The 32-bit offset of the target from the beginning of its section. This is used to support debugging information as well as static thread local storage.

//! @}

//! @name COFF Symbol Table, Section Number Values (symbol.section_number)
//! @{
#define IMAGE_SYM_UNDEFINED  0 //!< Symbol record is not yet assigned a section. If the value is 0 this indicates a references to an external symbol defined elsewhere. If the value is non-zero this is a common symbol with a size specified by the value.
#define IMAGE_SYM_ABSOLUTE  -1 //!< The symbol has an absolute (non-relocatable) value and is not an address.
#define IMAGE_SYM_DEBUG     -2 //!< The symbol provides general type or debugging information but does not correspond to a section. Microsoft tools use this setting along with .file records (storage class FILE).

//! @}

//! @name COFF Symbol Table, Type Representation (symbol.type)
//! @{
#define IMAGE_SYM_TYPE_NULL    0 //!< No type information or unknown base type. Microsoft tools use this setting.
#define IMAGE_SYM_TYPE_VOID    1 //!< No valid type; used with void pointers and functions.
#define IMAGE_SYM_TYPE_CHAR    2 //!< Character (signed byte).
#define IMAGE_SYM_TYPE_SHORT   3 //!< Two-byte signed integer.
#define IMAGE_SYM_TYPE_INT     4 //!< Natural integer type (normally four bytes in Windows NT).
#define IMAGE_SYM_TYPE_LONG    5 //!< Four-byte signed integer.
#define IMAGE_SYM_TYPE_FLOAT   6 //!< Four-byte floating-point number.
#define IMAGE_SYM_TYPE_DOUBLE  7 //!< Eight-byte floating-point number.
#define IMAGE_SYM_TYPE_STRUCT  8 //!< Structure.
#define IMAGE_SYM_TYPE_UNION   9 //!< Union.
#define IMAGE_SYM_TYPE_ENUM   10 //!< Enumerated type.
#define IMAGE_SYM_TYPE_MOE    11 //!< Member of enumeration (a specific value).
#define IMAGE_SYM_TYPE_BYTE   12 //!< Byte; unsigned one-byte integer.
#define IMAGE_SYM_TYPE_WORD   13 //!< Word; unsigned two-byte integer.
#define IMAGE_SYM_TYPE_UINT   14 //!< Unsigned integer of natural size (normally, four bytes).
#define IMAGE_SYM_TYPE_DWORD  15 //!< Unsigned four-byte integer.

//! @}

//! @name Microsoft tools use the type field only to indicate whether or not the symbol is a function, so that the only two resulting values are 0x0 and 0x20 for the Type field.
//! @{
#define IMAGE_SYM_TYPE_NOT_FUNCTION 0
#define IMAGE_SYM_TYPE_FUNCTION     0x20

//! @}

//! @name Complex types
//! @{
#define IMAGE_SYM_DTYPE_NULL     0 //!< No derived type; the symbol is a simple scalar variable.
#define IMAGE_SYM_DTYPE_POINTER  1 //!< Pointer to base type.
#define IMAGE_SYM_DTYPE_FUNCTION 2 //!< Function returning base type.
#define IMAGE_SYM_DTYPE_ARRAY    3 //!< Array of base type.

//! @}

//! @name COFF Symbol Table, Storage Class (symbol.storage_class)
//! @{
#define IMAGE_SYM_CLASS_END_OF_FUNCTION   -1 //!< Special symbol representing end of function, for debugging purposes.
#define IMAGE_SYM_CLASS_NULL               0 //!< No storage class assigned.
#define IMAGE_SYM_CLASS_AUTOMATIC          1 //!< Automatic (stack) variable. The Value field specifies stack frame offset.
#define IMAGE_SYM_CLASS_EXTERNAL           2 //!< Used by Microsoft tools for external symbols. The Value field indicates the size if the section number is IMAGE_SYM_UNDEFINED (0). If the section number is not 0, then the Value field specifies the offset within the section.
#define IMAGE_SYM_CLASS_STATIC             3 //!< The Value field specifies the offset of the symbol within the section. If the Value is 0, then the symbol represents a section name.
#define IMAGE_SYM_CLASS_REGISTER           4 //!< Register variable. The Value field specifies register number.
#define IMAGE_SYM_CLASS_EXTERNAL_DEF       5 //!< Symbol is defined externally.
#define IMAGE_SYM_CLASS_LABEL              6 //!< Code label defined within the module. The Value field specifies the offset of the symbol within the section.
#define IMAGE_SYM_CLASS_UNDEFINED_LABEL    7 //!< Reference to a code label not defined. IMAGE_SYM_CLASS_MEMBER_OF_STRUCT 8 Structure member. The Value field specifies nth member.
#define IMAGE_SYM_CLASS_ARGUMENT           9 //!< Formal argument (parameter)of a function. The Value field specifies nth argument.
#define IMAGE_SYM_CLASS_STRUCT_TAG        10 //!< Structure tag-name entry.
#define IMAGE_SYM_CLASS_MEMBER_OF_UNION   11 //!< Union member. The Value field specifies nth member.
#define IMAGE_SYM_CLASS_UNION_TAG         12 //!< Union tag-name entry.
#define IMAGE_SYM_CLASS_TYPE_DEFINITION   13 //!< Typedef entry.
#define IMAGE_SYM_CLASS_UNDEFINED_STATIC  14 //!< Static data declaration.
#define IMAGE_SYM_CLASS_ENUM_TAG          15 //!< Enumerated type tagname entry.
#define IMAGE_SYM_CLASS_MEMBER_OF_ENUM    16 //!< Member of enumeration. Value specifies nth member.
#define IMAGE_SYM_CLASS_REGISTER_PARAM    17 //!< Register parameter.
#define IMAGE_SYM_CLASS_BIT_FIELD         18 //!< Bit-field reference. Value specifies nth bit in the bit field.
#define IMAGE_SYM_CLASS_BLOCK            100 //!< A .bb (beginning of block) or .eb (end of block) record. Value is the relocatable address of the code location.
#define IMAGE_SYM_CLASS_FUNCTION         101 //!< Used by Microsoft tools for symbol records that define the extent of a function: begin function (named .bf), end function (.ef), and lines in function (.lf). For .lf records, Value gives the number of source lines in the function. For .ef records, Value gives the size of function code.
#define IMAGE_SYM_CLASS_END_OF_STRUCT    102 //!< End of structure entry.
#define IMAGE_SYM_CLASS_FILE             103 //!< Used by Microsoft tools, as well as traditional COFF format, for the source-file symbol record. The symbol is followed by auxiliary records that name the file.
#define IMAGE_SYM_CLASS_SECTION          104 //!< Definition of a section (Microsoft tools use STATIC storage class instead).
#define IMAGE_SYM_CLASS_WEAK_EXTERNAL    105 //!< Weak external

//! @}

//! @name COMDAT Sections (auxiliary_symbol_record_5.selection)
//! @{
#define IMAGE_COMDAT_SELECT_NODUPLICATES 1
#define IMAGE_COMDAT_SELECT_ANY          2
#define IMAGE_COMDAT_SELECT_SAME_SIZE    3
#define IMAGE_COMDAT_SELECT_EXACT_MATCH  4
#define IMAGE_COMDAT_SELECT_ASSOCIATIVE  5
#define IMAGE_COMDAT_SELECT_LARGEST      6

//! @}

// See Texas Instruments documentation spraao8

//! @name Texas Instruments File Header Flags
//! @{
#define F_RELFLG 0x0001
#define F_EXEC 0x0002
#define F_LNNO 0x0004
#define F_LSYMS 0x0008
#define F_LITTLE 0x0100
#define F_BIG 0x0200
#define F_SYMMERGE 0x1000

//! @}

//! @name Texas Instruments Section Header Flags
//! @{
#define STYP_REG                    0x00000000
#define STYP_DSECT                  0x00000001
#define STYP_NOLOAD                 0x00000002
#define STYP_GROUP                  0x00000004
#define STYP_PAD                    0x00000008
#define STYP_COPY                   0x00000010
#define STYP_TEXT                   0x00000020
#define STYP_DATA                   0x00000040
#define STYP_BSS                    0x00000080
#define STYP_BLOCK                  0x00001000
#define STYP_PASS                   0x00002000
#define STYP_CLINK                  0x00004000
#define STYP_VECTOR                 0x00008000
#define STYP_PADDED                 0x00010000
#define STYP_ALIGN_2                0x00000100
#define STYP_ALIGN_4                0x00000200
#define STYP_ALIGN_8                0x00000300
#define STYP_ALIGN_16               0x00000400
#define STYP_ALIGN_32               0x00000500
#define STYP_ALIGN_64               0x00000600
#define STYP_ALIGN_128              0x00000700
#define STYP_ALIGN_256              0x00000800
#define STYP_ALIGN_512              0x00000900
#define STYP_ALIGN_1024             0x00000A00
#define STYP_ALIGN_2048             0x00000B00
#define STYP_ALIGN_4096             0x00000C00
#define STYP_ALIGN_8192             0x00000D00
#define STYP_ALIGN_16384            0x00000E00
#define STYP_ALIGN_32768            0x00000F00

//! @}

//! @name Texas Instruments target ID
//! @{
#define TMS470          0x0097
#define TMS320C5400     0x0098
#define TMS320C6000     0x0099
#define TMS320C5500     0x009C
#define TMS320C2800     0x009D
#define MSP430          0x00A0
#define TMS320C5500plus 0x00A1

//! @}

//! @name Texas Instruments Generic Relocation Types (rel_entry.type)
//! @{
#define RE_ADD             0x4000 //!< Addition (+)
#define RE_SUB             0x4001 //!< Subtraction (-)
#define RE_NEG             0x4002 //!< Negate (-)
#define RE_MPY             0x4003 //!< Multiplication (*)
#define RE_DIV             0x4004 //!< Division (/)
#define RE_MOD             0x4005 //!< Modulus (%)
#define RE_SR              0x4006 //!< Logical shift right (unsigned >>)
#define RE_ASR             0x4007 //!< Arithmetic shift right (signed >>)
#define RE_SL              0x4008 //!< Shift left (<<)
#define RE_AND             0x4009 //!< And (&)
#define RE_OR              0x400A //!< Or (|)
#define RE_XOR             0x400B //!< Exclusive Or (^)
#define RE_NOTB            0x400C //!< Not (~)
#define RE_ULDFLD          0x400D //!< Unsigned relocation field load
#define RE_SLDFLD          0x400E //!< Signed relocation field load
#define RE_USTFLD          0x400F //!< Unsigned relocation field store
#define RE_SSTFLD          0x4010 //!< Signed relocation field store
#define RE_PUSH            0x4011 //!< Push symbol on the stack
#define RE_PUSHSK          0x4012 //!< Push signed constant on the stack
#define RE_PUSHUK          0x4013 //!< Push unsigned constant on the stack
#define RE_PUSHPC          0x4014 //!< Push current section PC on the stack
#define RE_DUP             0x4015 //!< Duplicate top-of-stack and 0xpus // a copy
#define RE_XSTFLD          0x4016 //!< Relocation field store, signedness is irrelevant
#define RE_PUSHSV          0xC011 //!< Push symbol: SEGVALUE flag is set

//! @}

//! @name Texas Instruments C6000 Relocation Types (rel_entry.type)
//! @{
#define R_C60_ABS          0x0000 //!< No relocation
#define R_C60_RELBYTE      0x000F //!< 8-bit direct reference to symbol's address
#define R_C60_RELWORD      0x0010 //!< 16-bit direct reference to symbol's address
#define R_C60_RELLONG      0x0011 //!< 32-bit direct reference to symbol's address
#define R_C60_BASE         0x0050 //!< Data page pointer-based offset
#define R_C60_DIR15        0x0051 //!< Load or store long displacement
#define R_C60_PCR21        0x0052 //!< 21-bit packet, PC relative
#define R_C60_PCR10        0x0053 //!< 10-bit Packet PC Relative (BDEC, BPOS)
#define R_C60_LO16         0x0054 //!< MVK instruction low half register
#define R_C60_HI16         0x0055 //!< MVKH or MVKLH 0xhig // half register
#define R_C60_SECT         0x0056 //!< Section-based offset
#define R_C60_S16          0x0057 //!< Signed 16-bit offset for MVK
#define R_C60_PCR7         0x0070 //!< 7-bit Packet PC Relative (ADDKPC)
#define R_C60_PCR12        0x0071 //!< 12-bit Packet PC Relative (BNOP)

//! @}

//! @name Texas Instruments C2800 Relocation Types (rel_entry.type)
//! @{
#define R_C28_ABS          0x0000 //!< No relocation
#define R_C28_RELBYTE      0x000F //!< 8-bit direct reference to symbol's address
#define R_C28_RELWORD      0x0010 //!< 16-bit direct reference to symbol's address
#define R_C28_RELLONG      0x0011 //!< 32-bit direct reference to symbol's address
#define R_C28_PARTLS7      0x0028 //!< 7-bit offset of a 22-bit address
#define R_C28_PARTLS6      0x005D //!< 6-bit offset of a 22-bit address
#define R_C28_PARTMID10    0x005E //!< Middle 10 bits of a 22-bit address
#define R_C28_REL22        0x005F //!< 22-bit direct reference to a symbol's address
#define R_C28_PARTMS6      0x0060 //!< Upper 6 bits of an 22-bit address
#define R_C28_PARTMS16     0x0061 //!< Upper 16 bits of an 22-bit address
#define R_C28_PCR16        0x0062 //!< PC relative 16-bit address
#define R_C28_PCR8         0x0063 //!< PC relative 8-bit address
#define R_C28_PTR          0x0064 //!< 22-bit pointer
#define R_C28_HI16         0x0065 //!< High 16 bits of address data
#define R_C28_LOPTR        0x0066 //!< Pointer to low 64K
#define R_C28_NWORD        0x0067 //!< 16-bit negated relocation
#define R_C28_NBYTE        0x0068 //!< 8-bit negated relocation
#define R_C28_HIBYTE       0x0069 //!< High 8 bits of a 16-bit data
#define R_C28_RELS13       0x006A //!< Signed 13-bit value relocated as a 16-bit value

//! @}

//! @name Texas Instruments C5400 Relocation Types (rel_entry.type)
//! @{
#define R_C54_ABS          0x0000 //!< No relocation
#define R_C54_REL24        0x0005 //!< 24-bit reference to symbol's address
#define R_C54_RELBYTE      0x0017 //!< 8-bit direct reference to symbol's address
#define R_C54_RELWORD      0x0020 //!< 16-bit direct reference to symbol's address
#define R_C54_RELLONG      0x0021 //!< 32-bit direct reference to symbol's address
#define R_C54_PARTLS7      0x0028 //!< 7 LSBs of an address
#define R_C54_PARTMS9      0x0029 //!< 9 MSBs of an address
#define R_C54_REL13        0x002A //!< 13-bit direct reference to symbol's address

//! @}

//! @name Texas Instruments C5500 Relocation Types (rel_entry.type)
//! @{
#define R_C55_ABS          0x0000 //!< No relocation
#define R_C55_REL24        0x0005 //!< 24-bit direct reference to symbol's address
#define R_C55_RELBYTE      0x0017 //!< 8-bit direct reference to symbol's address
#define R_C55_RELWORD      0x0020 //!< 16-bit direct reference to symbol's address
#define R_C55_RELLONG      0x0021 //!< 32-bit direct reference to symbol's address
#define R_C55_LD3_DMA      0x0170 //!< 7 MSBs of a byte, unsigned; used in DMA address
#define R_C55_LD3_MDP      0x0172 //!< 7 bits spanning 2 bytes, unsigned; used as MDP register value
#define R_C55_LD3_PDP      0x0173 //!< 9 bits spanning 2 bytes, unsigned; used as PDP register value
#define R_C55_LD3_REL23    0x0174 //!< 23-bit unsigned value in 24-bit field
#define R_C55_LD3_k8       0x0210 //!< 8-bit unsigned direct reference
#define R_C55_LD3_k16      0x0211 //!< 16-bit unsigned direct reference
#define R_C55_LD3_K8       0x0212 //!< 8-bit signed direct reference
#define R_C55_LD3_K16      0x0213 //!< 16-bit signed direct reference
#define R_C55_LD3_I8       0x0214 //!< 8-bit unsigned PC-relative reference
#define R_C55_LD3_I16      0x0215 //!< 16-bit unsigned PC-relative reference
#define R_C55_LD3_L8       0x0216 //!< 8-bit signed PC-relative reference
#define R_C55_LD3_L16      0x0217 //!< 16-bit signed PC-relative reference
#define R_C55_LD3_k4       0x0220 //!< Unsigned 4-bit shift immediate
#define R_C55_LD3_k5       0x0221 //!< Unsigned 5-bit shift immediate
#define R_C55_LD3_K5       0x0222 //!< Signed 5-bit shift immediate
#define R_C55_LD3_k6       0x0223 //!< Unsigned 6-bit shift immediate
#define R_C55_LD3_k12      0x0224 //!< Unigned 12-bit shift immediate

//! @}

//! @name Texas Instruments MSP430 Relocation Types (rel_entry.type)
//! @{
#define R_MSP430_RELLONG   0x0011 //!< 32-bit direct reference to symbol's address
#define R_MSP430_PCR23H    0x0016 //!< 23-bit PC-relative reference to a symbol's address, in halfwords (divided by 2)
#define R_MSP430_PCR24W    0x0017 //!< 24-bit PC-relative reference to a symbol's address, in words (divided by 4)

//! @}

//! @name Texas Instruments Symbol Storage Classes
//! @{
#define C_NULL      0 //!< No storage class
#define C_AUTO      1 //!< Reserved
#define C_EXT       2 //!< External definition
#define C_STAT      3 //!< Static
#define C_REG       4 //!< Reserved
#define C_EXTREF    5 //!< External reference
#define C_LABEL     6 //!< Label
#define C_ULABEL    7 //!< Undefined label
#define C_MOS       8 //!< Reserved
#define C_ARG       9 //!< Reserved
#define C_STRTAG   10 //!< Reserved
#define C_MOU      11 //!< Reserved
#define C_UNTAG    12 //!< Reserved
#define C_TPDEF    13 //!< Reserved
#define C_USTATIC  14 //!< Undefined static
#define C_ENTAG    15 //!< Reserved
#define C_MOE      16 //!< Reserved
#define C_REGPARM  17 //!< Reserved
#define C_FIELD    18 //!< Reserved
#define C_UEXT     19 //!< Tentative external definition
#define C_STATLAB  20 //!< Static load time label
#define C_EXTLAB   21 //!< External load time label
#define C_VARARG   27 //!< Last declared parameter of a function with a variable number of arguments
#define C_BLOCK   100 //!< Reserved
#define C_FCN     101 //!< Reserved
#define C_EOS     102 //!< Reserved
#define C_FILE    103 //!< Reserved
#define C_LINE    104 //!< Used only by utility programs

//! @}


/*! @name CEVA-X architecture
 *
 * Could not find any documentation.
 * Reverse-engineered some values from the binary files.
 */
//! @{
#define CEVA_MACHINE_XC4210_LIB 0xDCA6
#define CEVA_MACHINE_XC4210_OBJ 0x8CA6
#define CEVA_INVALID_SYMBOL_TABLE_INDEX 0xFFFFFFFF
#define CEVA_UNINITIALIZED_DATA 0x80

//! @}

//! Architectures supported by COFFI
typedef enum coffi_architecture_t {
    //! Architecture unknown, or file not initialized.
    COFFI_ARCHITECTURE_NONE  = 0,
    //! Windows portable executable (PE or PE+)
    COFFI_ARCHITECTURE_PE    = 1,
    //! Texas Instruments
    COFFI_ARCHITECTURE_TI    = 2,
    //! CEVA Inc.
    COFFI_ARCHITECTURE_CEVA = 3,
} coffi_architecture_t;

// clang-format on

//------------------------------------------------------------------------------
// Packing the structures
// The GCC syntax does not work for other compilers (Visual)
#pragma pack(1)

//------------------------------------------------------------------------------

//! MS-DOS file header
struct msdos_header
{
    //! 0x4d, 0x5a. This is the "magic number" of an EXE file
    uint16_t signature;
    //! @brief The number of bytes in the last block of the program that are actually used.
    //!
    //! If this value is zero, that means the entire last block is used (i.e. the effective value is 512)
    uint16_t bytes_in_last_block;
    //! @brief Number of blocks in the file that are part of the EXE file.
    //!
    //! If [02-03] is non-zero, only that much of the last block is used
    uint16_t blocks_in_file;
    //! Number of relocation entries stored after the header. May be zero
    uint16_t num_relocs;
    //! @brief Number of paragraphs in the header.
    //!
    //! The program's data begins just after the header, and this field can be used to calculate the appropriate file offset.
    //! The header includes the relocation entries.
    //! Note that some OSs and/or programs may fail if the header is not a multiple of 512 bytes.
    uint16_t header_paragraphs;
    //! @brief Number of paragraphs of additional memory that the program will need.
    //!
    //! This is the equivalent of the BSS size in a Unix program.
    //! The program can't be loaded if there isn't at least this much memory available to it
    uint16_t min_extra_paragraphs;
    //! @brief Maximum number of paragraphs of additional memory.
    //!
    //! Normally, the OS reserves all the remaining conventional memory for your program, but you can limit it with this field.
    uint16_t max_extra_paragraphs;
    //! @brief Relative value of the stack segment.
    //!
    //! This value is added to the segment the program was loaded at, and the result is used to initialize the SS register.
    uint16_t ss;
    //! Initial value of the SP register.
    uint16_t sp;
    //! @brief Word checksum.
    //!
    //! If set properly, the 16-bit sum of all words in the file should be zero. Usually, this isn't filled in.
    uint16_t checksum;
    //! Initial value of the IP register (entry point).
    uint16_t ip;
    //! Initial value of the CS register, relative to the segment the program was loaded at.
    uint16_t cs;
    //! Offset of the first relocation item in the file.
    uint16_t reloc_table_offset;
    //! Overlay number. Normally zero, meaning that it's the main program.
    uint16_t overlay_number;
    uint16_t reserved1[4];
    uint16_t oem_id;
    uint16_t oem_info;
    uint16_t reserved2[10];
    //! Location of PE format signature.
    int32_t pe_sign_location;
};

struct msdos_header_rel
{
    uint16_t offset;
    uint16_t segment;
};

//------------------------------------------------------------------------------
// COFF headers

//! PE COFF header
struct coff_file_header
{
    uint16_t machine;         //!< Identifies the type of target machine
    uint16_t sections_count;  //!< The size of the section table
    uint32_t time_data_stamp; //!< A C run-time time_t value
    uint32_t
        symbol_table_offset; //!< The file offset of the COFF symbol table, or zero if no COFF symbol table is present
    uint32_t symbols_count;  //!< The number of entries in the symbol table
    uint16_t
        optional_header_size; //!< Which is required for executable files but not for object files.
    uint16_t flags; //!< The flags that indicate the attributes of the file
};

//! Texas Instruments COFF header
struct coff_file_header_ti
{
    uint16_t version;         //!< Indicates version of COFF file structure
    uint16_t sections_count;  //!< The size of the section table
    uint32_t time_data_stamp; //!< A C run-time time_t value
    uint32_t
        symbol_table_offset; //!< The file offset of the COFF symbol table, or zero if no COFF symbol table is present
    uint32_t symbols_count;  //!< The number of entries in the symbol table
    uint16_t
        optional_header_size; //!< Which is required for executable files but not for object files.
    uint16_t flags; //!< The flags that indicate the attributes of the file
    uint16_t
        target_id; //!< Magic number indicates the file can be executed in a specific TI system
};

//------------------------------------------------------------------------------
// COFF optional headers

//! PE32 COFF optional header
struct coff_optional_header_pe
{
    uint16_t magic;                //!< State of the image file identifier
    uint8_t  major_linker_version; //!< The linker major version number
    uint8_t  minor_linker_version; //!< The linker minor version number
    uint32_t code_size;            //!< The sum of all code sections
    uint32_t
        initialized_data_size; //!< The sum of all initialized data sections
    uint32_t
        uninitialized_data_size; //!< The sum of all uninitialized data sections
    uint32_t
        entry_point_address; //!< The address of the entry point relative to the image base
    uint32_t code_base; //!< base of the beginning-of-code section
    uint32_t data_base; //!< base of the beginning-of-data section
};

//! PE32+ COFF optional header
struct coff_optional_header_pe_plus
{
    uint16_t magic;                //!< State of the image file identifier
    uint8_t  major_linker_version; //!< The linker major version number
    uint8_t  minor_linker_version; //!< The linker minor version number
    uint32_t code_size;            //!< The sum of all code sections
    uint32_t
        initialized_data_size; //!< The sum of all initialized data sections
    uint32_t
        uninitialized_data_size; //!< The sum of all uninitialized data sections
    uint32_t
        entry_point_address; //!< The address of the entry point relative to the image base
    uint32_t code_base; //!< base of the beginning-of-code section
};

//! Texas Instruments COFF optional header
struct common_optional_header_ti
{
    uint16_t magic;          //!< State of the image file identifier
    uint16_t linker_version; //!< The linker version number
    uint32_t code_size;      //!< The sum of all code sections
    uint32_t
        initialized_data_size; //!< The sum of all initialized data sections
    uint32_t
        uninitialized_data_size; //!< The sum of all uninitialized data sections
    uint32_t
        entry_point_address; //!< The address of the entry point relative to the image base
    uint32_t code_base; //!< The address that is relative to the image
    uint32_t data_base; //!<
};

//------------------------------------------------------------------------------
// Windows NT headers

//! PE32 Windows NT header
struct win_header_pe
{
    uint32_t image_base;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t major_os_version;
    uint16_t minor_os_version;
    uint16_t major_image_version;
    uint16_t minor_image_version;
    uint16_t major_subsystem_version;
    uint16_t minor_subsystem_version;
    uint32_t win32_version_value;
    uint32_t image_size;
    uint32_t headers_size;
    uint32_t checksum;
    uint16_t subsystem;
    uint16_t dll_flags;
    uint32_t stack_reserve_size;
    uint32_t stack_commit_size;
    uint32_t heap_reserve_size;
    uint32_t heap_commit_size;
    uint32_t loader_flags;
    uint32_t number_of_rva_and_sizes;
};

//! PE32+ Windows NT header
struct win_header_pe_plus
{
    uint64_t image_base;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t major_os_version;
    uint16_t minor_os_version;
    uint16_t major_image_version;
    uint16_t minor_image_version;
    uint16_t major_subsystem_version;
    uint16_t minor_subsystem_version;
    uint32_t win32_version_value;
    uint32_t image_size;
    uint32_t headers_size;
    uint32_t checksum;
    uint16_t subsystem;
    uint16_t dll_flags;
    uint64_t stack_reserve_size;
    uint64_t stack_commit_size;
    uint64_t heap_reserve_size;
    uint64_t heap_commit_size;
    uint32_t loader_flags;
    uint32_t number_of_rva_and_sizes;
};

//------------------------------------------------------------------------------
// Symbol records

//! Symbol record
struct symbol_record
{
    char     name[8];
    uint32_t value;
    uint16_t section_number;
    uint16_t type;
    uint8_t  storage_class;
    uint8_t  aux_symbols_number;
};

//! Generic auxiliary symbol record, covers any type of auxiliary symbol
struct auxiliary_symbol_record
{
    char value[sizeof(symbol_record)];
};

//! PE auxiliary format 1: Function definitions
struct auxiliary_symbol_record_1
{
    uint32_t tag_index;
    uint32_t total_size;
    uint32_t pointer_to_linenumber;
    uint32_t pointer_to_next_function;
    uint16_t unused;
};

//! PE auxiliary format 2: .bf and .ef symbols
struct auxiliary_symbol_record_2
{
    uint32_t unused0;
    uint16_t linenumber;
    uint8_t  unused1[6];
    uint32_t pointer_to_next_function;
    uint16_t unused2;
};

//! PE auxiliary format 3: Weak externals
struct auxiliary_symbol_record_3
{
    uint32_t tag_index;
    uint32_t characteristics;
    uint8_t  unused1[10];
};

//! PE auxiliary format 4: Files
struct auxiliary_symbol_record_4
{
    char file_name[18];
};

//! PE auxiliary format 5: Section definitions
struct auxiliary_symbol_record_5
{
    uint32_t length;
    uint16_t number_of_relocations;
    uint16_t number_of_linenumbers;
    uint32_t check_sum;
    uint16_t number;
    uint8_t  selection;
    uint8_t  unused[3];
};

//------------------------------------------------------------------------------
//! PE data directory
struct image_data_directory
{
    uint32_t virtual_address;
    uint32_t size;
};

//------------------------------------------------------------------------------
// Section headers

//! PE section header
struct section_header
{
    char     name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;
    uint32_t data_size;
    uint32_t data_offset;
    uint32_t reloc_offset;
    uint32_t line_num_offset;
    uint16_t reloc_count;
    uint16_t line_num_count;
    uint32_t flags;
};

//! Texas Instruments section header
struct section_header_ti
{
    char     name[8];
    uint32_t physical_address;
    uint32_t virtual_address;
    uint32_t data_size;
    uint32_t data_offset;
    uint32_t reloc_offset;
    uint32_t reserved_0;
    uint32_t reloc_count;
    uint32_t line_num_count;
    uint32_t flags;
    uint16_t reserved_1;
    uint16_t page_number;
};

//------------------------------------------------------------------------------
// Relocation entries

//! PE relocation entry
struct rel_entry
{
    uint32_t virtual_address;
    uint32_t symbol_table_index;
    uint16_t type;
};

//! Texas Instruments relocation entry
struct rel_entry_ti
{
    uint32_t virtual_address;
    uint32_t symbol_table_index;
    uint16_t reserved;
    uint16_t type;
};

//! CEVA relocation entry
struct rel_entry_ceva
{
    uint32_t virtual_address;
    uint32_t symbol_table_index;
    uint32_t type;
};

//! Structure capable of storing all the architecture-specific relocation entry structures
struct rel_entry_generic
{
    uint32_t virtual_address;
    uint32_t symbol_table_index;
    uint32_t type;
    uint16_t reserved;
};

//------------------------------------------------------------------------------
//! Line number structure in a COFF binary file.
struct line_number
{
    //! Symbol table index or virtual address. Whether symbol table index or RVA is used depends on the value of line_number.
    uint32_t type;
    //! When nonzero, this field specifies a one-based line number. When zero, the Type field is interpreted as a symbol table index for a function.
    uint16_t line_number;
};

//------------------------------------------------------------------------------
// Interfaces classes (pure virtual classes)
//------------------------------------------------------------------------------
class symbol;

//! Interface for accessing to the string table
class string_to_name_provider
{
  public:
    //! @brief Converts an 8-bytes symbol short name into a full name, eventually by looking into the strings table.
    //!
    //! @note Symbol short names that reference the string table start with "\x00\x00\x00\x00".
    virtual std::string string_to_name(const char* str) const = 0;
    //! @brief Converts an 8-bytes section short name into a full name, eventually by looking into the strings table.
    //!
    //! @note Section short names that reference the string table start with "/".
    virtual std::string section_string_to_name(const char* str) const = 0;
    //! Converts symbol full name into an 8-bytes short name, eventually creating an entry in the strings table.
    virtual void name_to_string(const std::string& name, char* str) = 0;
    //! Converts section full name into an 8-bytes short name, eventually creating an entry in the strings table.
    virtual void name_to_section_string(const std::string& name, char* str) = 0;
};

//------------------------------------------------------------------------------
//! Interface for accessing to the symbol table
class symbol_provider
{
  public:
    //! Gets a symbol from its index.
    virtual const symbol* get_symbol(uint32_t index) const = 0;
    //! Gets a symbol from its index.
    virtual symbol* get_symbol(uint32_t index) = 0;
    //! Gets a symbol from its name.
    virtual const symbol* get_symbol(const std::string& name) const = 0;
    //! Gets a symbol from its name.
    virtual symbol* get_symbol(const std::string& name) = 0;
    //! @brief Adds a symbol in the table.
    //!
    //! Eventually creates an entry in the strings table for the symbol name.
    virtual symbol* add_symbol(const std::string& name) = 0;
};

//------------------------------------------------------------------------------
//! @brief Interface for architecture information
//!
//! This interface is implemented by the coffi class.
class architecture_provider
{
  public:
    //! @brief Returns the coffi object architecture.
    //!
    //! @return #COFFI_ARCHITECTURE_NONE if the coffi object is not initialized, or in case of unrecognized architecture when loading a file.
    virtual coffi_architecture_t get_architecture() const = 0;

    //! @brief Returns the character type size in bytes.
    //!
    //! Auto-detects the addressable unit: are the addresses in bytes or 2-bytes words?
    //! <br>Some targets have 2-bytes characters, this changes how offsets are computed in the file.
    //! @return 1 in most cases (characters are 1 byte), 2 for some specific targets (TMS320 C2000 for example).
    virtual int get_addressable_unit() const = 0;
};

//------------------------------------------------------------------------------
class dos_header;
class coff_header;
class optional_header;
class win_header;
class sections;

//! @brief Interface that gives the headers and sections.
//!
//! This interface is implemented by the coffi class.
class sections_provider : public virtual architecture_provider
{
  public:
    //! Returns the MS-DOS header.
    virtual const dos_header* get_msdos_header() const = 0;
    //! Returns the COFF file header.
    virtual const coff_header* get_header() const = 0;
    //! Returns the COFF optional header.
    virtual const optional_header* get_optional_header() const = 0;
    //! Returns the Windows NT header.
    virtual const win_header* get_win_header() const = 0;
    //! Returns the sections.
    virtual const sections& get_sections() const = 0;
};

} // namespace COFFI

#pragma pack()

#endif // COFFI_TYPES_HPP
