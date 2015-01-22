INCLUDES =  -I$(top_srcdir)/src/include -I$(top_builddir)/src -I$(top_srcdir)/src -I$(top_srcdir)

AM_CXXFLAGS = $(CFLAGS) -DUSE_GUICHAN

noinst_LIBRARIES = libguidep.a

libguidep_a_SOURCES = guichan-gui.cpp menuMessage.cpp menuLoad_guichan.cpp menuConfig.cpp \
	menuPaths.cpp menuFloppy.cpp menuHdd.cpp menuAddEditHdd.cpp menuCpuFpu.cpp menuChipset.cpp \
	menuRom.cpp menuRam.cpp menuDisplay.cpp menuSound.cpp menuSaveState.cpp menuControl.cpp \
	menuCustom.cpp menuOnScreen.cpp sdltruetypefont.cpp
