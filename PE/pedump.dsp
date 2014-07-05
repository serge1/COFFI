# Microsoft Developer Studio Project File - Name="pedump" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=pedump - Win32 Win64 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pedump.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pedump.mak" CFG="pedump - Win32 Win64 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pedump - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "pedump - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "pedump - Win32 Win64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/pedump", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pedump - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 delayimp.lib dbghelp.lib kernel32.lib /nologo /subsystem:console /map /machine:I386 /DELAYLOAD:dbghelp.dll /OPT:NOWIN98 /merge:.rdata=.text /OPT:ICF /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pedump - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 delayimp.lib dbghelp.lib kernel32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /DELAYLOAD:dbghelp.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pedump - Win32 Win64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pedump___Win32_Win64_Debug"
# PROP BASE Intermediate_Dir "pedump___Win32_Win64_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win64_Debug"
# PROP Intermediate_Dir "Win64_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /X /I "\mssdk\include\win64\crt" /I "\mssdk\include\prerelease" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 delayimp.lib imagehlp.lib kernel32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /DELAYLOAD:imagehlp.dll
# ADD LINK32 delayimp.lib dbghelp.lib kernel32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"\mssdk\lib\ia64" /libpath:"C:\mssdk\lib\prerelease\ia64" /DELAYLOAD:dbghelp.dll /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pedump - Win32 Release"
# Name "pedump - Win32 Debug"
# Name "pedump - Win32 Win64 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\coffsymboltable.cpp
DEP_CPP_COFFS=\
	".\coffsymboltable.h"\
	
# End Source File
# Begin Source File

SOURCE=.\COMMON.cpp
DEP_CPP_COMMO=\
	".\COMMON.H"\
	".\EXTRNVAR.H"\
	".\symboltablesupport.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CVSymbols.CPP
DEP_CPP_CVSYM=\
	".\COMMON.H"\
	".\cvexefmt.h"\
	".\cvinfo.h"\
	".\CVInfoNew.H"\
	
# End Source File
# Begin Source File

SOURCE=.\DBGDUMP.cpp
DEP_CPP_DBGDU=\
	".\coffsymboltable.h"\
	".\COMMON.H"\
	".\cvsymbols.h"\
	".\DBGDUMP.H"\
	".\EXTRNVAR.H"\
	".\symboltablesupport.h"\
	
# End Source File
# Begin Source File

SOURCE=.\EXEDUMP.cpp
DEP_CPP_EXEDU=\
	".\coffsymboltable.h"\
	".\COMMON.H"\
	".\EXTRNVAR.H"\
	".\resdump.h"\
	".\symboltablesupport.h"\
	
# End Source File
# Begin Source File

SOURCE=.\LIBDUMP.cpp
DEP_CPP_LIBDU=\
	".\COMMON.H"\
	".\EXTRNVAR.H"\
	".\LIBDUMP.H"\
	".\OBJDUMP.H"\
	
# End Source File
# Begin Source File

SOURCE=.\OBJDUMP.cpp
DEP_CPP_OBJDU=\
	".\coffsymboltable.h"\
	".\COMMON.H"\
	".\EXTRNVAR.H"\
	".\symboltablesupport.h"\
	
# End Source File
# Begin Source File

SOURCE=.\PEDUMP.cpp
DEP_CPP_PEDUM=\
	".\COMMON.H"\
	".\DBGDUMP.H"\
	".\EXEDUMP.H"\
	".\EXTRNVAR.H"\
	".\LIBDUMP.H"\
	".\OBJDUMP.H"\
	".\RomImage.h"\
	
# End Source File
# Begin Source File

SOURCE=.\resdump.cpp
DEP_CPP_RESDU=\
	".\COMMON.H"\
	".\EXTRNVAR.H"\
	".\resdump.h"\
	
# End Source File
# Begin Source File

SOURCE=.\romimage.cpp
DEP_CPP_ROMIM=\
	".\coffsymboltable.h"\
	".\COMMON.H"\
	".\EXEDUMP.H"\
	".\EXTRNVAR.H"\
	
# End Source File
# Begin Source File

SOURCE=.\symboltablesupport.cpp
DEP_CPP_SYMBO=\
	".\coffsymboltable.h"\
	".\COMMON.H"\
	".\cv_dbg.h"\
	".\cvsymbols.h"\
	".\EXTRNVAR.H"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\coffsymboltable.h
# End Source File
# Begin Source File

SOURCE=.\COMMON.H
# End Source File
# Begin Source File

SOURCE=.\cv_dbg.h
# End Source File
# Begin Source File

SOURCE=.\cvexefmt.h
# End Source File
# Begin Source File

SOURCE=.\cvinfo.h
# End Source File
# Begin Source File

SOURCE=.\CVInfoNew.H
# End Source File
# Begin Source File

SOURCE=.\cvsymbols.h
# End Source File
# Begin Source File

SOURCE=.\DBGDUMP.H
# End Source File
# Begin Source File

SOURCE=.\EXEDUMP.H
# End Source File
# Begin Source File

SOURCE=.\EXTRNVAR.H
# End Source File
# Begin Source File

SOURCE=.\LIBDUMP.H
# End Source File
# Begin Source File

SOURCE=.\OBJDUMP.H
# End Source File
# Begin Source File

SOURCE=.\resdump.h
# End Source File
# Begin Source File

SOURCE=.\RomImage.h
# End Source File
# Begin Source File

SOURCE=.\symboltablesupport.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
