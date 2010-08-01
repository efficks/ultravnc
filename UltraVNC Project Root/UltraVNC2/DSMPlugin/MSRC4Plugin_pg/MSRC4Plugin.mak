# Microsoft Developer Studio Generated NMAKE File, Based on MSRC4Plugin.dsp
!IF "$(CFG)" == ""
CFG=MSRC4Plugin - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MSRC4Plugin - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MSRC4Plugin - Win32 Release" && "$(CFG)" != "MSRC4Plugin - Win32 Debug" && "$(CFG)" != "MSRC4Plugin - Win32 Release NoReg" && "$(CFG)" != "MSRC4Plugin - Win32 Debug NoReg"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSRC4Plugin.mak" CFG="MSRC4Plugin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MSRC4Plugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MSRC4Plugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MSRC4Plugin - Win32 Release NoReg" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MSRC4Plugin - Win32 Debug NoReg" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\MSRC4Plugin.dsm" "$(OUTDIR)\MSRC4Plugin.bsc"


CLEAN :
	-@erase "$(INTDIR)\crypto.obj"
	-@erase "$(INTDIR)\crypto.sbr"
	-@erase "$(INTDIR)\EnvReg.obj"
	-@erase "$(INTDIR)\EnvReg.sbr"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\logging.sbr"
	-@erase "$(INTDIR)\MSRC4Plugin.obj"
	-@erase "$(INTDIR)\MSRC4Plugin.res"
	-@erase "$(INTDIR)\MSRC4Plugin.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MSRC4Plugin.bsc"
	-@erase "$(OUTDIR)\MSRC4Plugin.dsm"
	-@erase "$(OUTDIR)\MSRC4Plugin.exp"
	-@erase "$(OUTDIR)\MSRC4Plugin.ilk"
	-@erase "$(OUTDIR)\MSRC4Plugin.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W4 /GX /O2 /Ob2 /D "_WITH_REGISTRY" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\MSRC4Plugin.res" /d "NDEBUG" /d "_WITH_REGISTRY" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MSRC4Plugin.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\crypto.sbr" \
	"$(INTDIR)\EnvReg.sbr" \
	"$(INTDIR)\logging.sbr" \
	"$(INTDIR)\MSRC4Plugin.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\MSRC4Plugin.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MSRC4Plugin.pdb" /machine:I386 /out:"$(OUTDIR)\MSRC4Plugin.dsm" /implib:"$(OUTDIR)\MSRC4Plugin.lib" 
LINK32_OBJS= \
	"$(INTDIR)\crypto.obj" \
	"$(INTDIR)\EnvReg.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\MSRC4Plugin.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\MSRC4Plugin.res"

"$(OUTDIR)\MSRC4Plugin.dsm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\MSRC4Plugin.dsm" "$(OUTDIR)\MSRC4Plugin.bsc"


CLEAN :
	-@erase "$(INTDIR)\crypto.obj"
	-@erase "$(INTDIR)\crypto.sbr"
	-@erase "$(INTDIR)\EnvReg.obj"
	-@erase "$(INTDIR)\EnvReg.sbr"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\logging.sbr"
	-@erase "$(INTDIR)\MSRC4Plugin.obj"
	-@erase "$(INTDIR)\MSRC4Plugin.res"
	-@erase "$(INTDIR)\MSRC4Plugin.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MSRC4Plugin.bsc"
	-@erase "$(OUTDIR)\MSRC4Plugin.dsm"
	-@erase "$(OUTDIR)\MSRC4Plugin.exp"
	-@erase "$(OUTDIR)\MSRC4Plugin.ilk"
	-@erase "$(OUTDIR)\MSRC4Plugin.lib"
	-@erase "$(OUTDIR)\MSRC4Plugin.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_WITH_REGISTRY" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\MSRC4Plugin.res" /d "_DEBUG" /d "_WITH_REGISTRY" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MSRC4Plugin.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\crypto.sbr" \
	"$(INTDIR)\EnvReg.sbr" \
	"$(INTDIR)\logging.sbr" \
	"$(INTDIR)\MSRC4Plugin.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\MSRC4Plugin.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MSRC4Plugin.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MSRC4Plugin.dsm" /implib:"$(OUTDIR)\MSRC4Plugin.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\crypto.obj" \
	"$(INTDIR)\EnvReg.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\MSRC4Plugin.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\MSRC4Plugin.res"

"$(OUTDIR)\MSRC4Plugin.dsm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\MSRC4Plugin_NoReg.dsm"


CLEAN :
	-@erase "$(INTDIR)\crypto.obj"
	-@erase "$(INTDIR)\EnvReg.obj"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\MSRC4Plugin.obj"
	-@erase "$(INTDIR)\MSRC4Plugin.res"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.dsm"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.exp"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.ilk"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W4 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\MSRC4Plugin.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MSRC4Plugin.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MSRC4Plugin_NoReg.pdb" /machine:I386 /out:"$(OUTDIR)\MSRC4Plugin_NoReg.dsm" /implib:"$(OUTDIR)\MSRC4Plugin_NoReg.lib" 
LINK32_OBJS= \
	"$(INTDIR)\crypto.obj" \
	"$(INTDIR)\EnvReg.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\MSRC4Plugin.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\MSRC4Plugin.res"

"$(OUTDIR)\MSRC4Plugin_NoReg.dsm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\MSRC4Plugin_NoReg.dsm" "$(OUTDIR)\MSRC4Plugin.bsc"


CLEAN :
	-@erase "$(INTDIR)\crypto.obj"
	-@erase "$(INTDIR)\crypto.sbr"
	-@erase "$(INTDIR)\EnvReg.obj"
	-@erase "$(INTDIR)\EnvReg.sbr"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\logging.sbr"
	-@erase "$(INTDIR)\MSRC4Plugin.obj"
	-@erase "$(INTDIR)\MSRC4Plugin.res"
	-@erase "$(INTDIR)\MSRC4Plugin.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MSRC4Plugin.bsc"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.dsm"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.exp"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.ilk"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.lib"
	-@erase "$(OUTDIR)\MSRC4Plugin_NoReg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\MSRC4Plugin.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MSRC4Plugin.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\crypto.sbr" \
	"$(INTDIR)\EnvReg.sbr" \
	"$(INTDIR)\logging.sbr" \
	"$(INTDIR)\MSRC4Plugin.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\MSRC4Plugin.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MSRC4Plugin_NoReg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MSRC4Plugin_NoReg.dsm" /implib:"$(OUTDIR)\MSRC4Plugin_NoReg.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\crypto.obj" \
	"$(INTDIR)\EnvReg.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\MSRC4Plugin.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\MSRC4Plugin.res"

"$(OUTDIR)\MSRC4Plugin_NoReg.dsm" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MSRC4Plugin.dep")
!INCLUDE "MSRC4Plugin.dep"
!ELSE 
!MESSAGE Warning: cannot find "MSRC4Plugin.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MSRC4Plugin - Win32 Release" || "$(CFG)" == "MSRC4Plugin - Win32 Debug" || "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg" || "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"
SOURCE=.\crypto.cpp

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\crypto.obj"	"$(INTDIR)\crypto.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\crypto.obj"	"$(INTDIR)\crypto.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\crypto.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\crypto.obj"	"$(INTDIR)\crypto.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\EnvReg.cpp

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\EnvReg.obj"	"$(INTDIR)\EnvReg.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\EnvReg.obj"	"$(INTDIR)\EnvReg.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\EnvReg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\EnvReg.obj"	"$(INTDIR)\EnvReg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\logging.cpp

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\logging.obj"	"$(INTDIR)\logging.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\logging.obj"	"$(INTDIR)\logging.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\logging.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\logging.obj"	"$(INTDIR)\logging.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MSRC4Plugin.cpp

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\MSRC4Plugin.obj"	"$(INTDIR)\MSRC4Plugin.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\MSRC4Plugin.obj"	"$(INTDIR)\MSRC4Plugin.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\MSRC4Plugin.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\MSRC4Plugin.obj"	"$(INTDIR)\MSRC4Plugin.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\registry.cpp

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\registry.obj"	"$(INTDIR)\registry.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\registry.obj"	"$(INTDIR)\registry.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\registry.obj"	"$(INTDIR)\registry.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\utils.cpp

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MSRC4Plugin.rc

!IF  "$(CFG)" == "MSRC4Plugin - Win32 Release"


"$(INTDIR)\MSRC4Plugin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x40c /fo"$(INTDIR)\MSRC4Plugin.res" /d "NDEBUG" /d "_WITH_REGISTRY" $(SOURCE)


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug"


"$(INTDIR)\MSRC4Plugin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\MSRC4Plugin.res" /d "_DEBUG" /d "_WITH_REGISTRY" $(SOURCE)


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Release NoReg"


"$(INTDIR)\MSRC4Plugin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x40c /fo"$(INTDIR)\MSRC4Plugin.res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "MSRC4Plugin - Win32 Debug NoReg"


"$(INTDIR)\MSRC4Plugin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\MSRC4Plugin.res" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

