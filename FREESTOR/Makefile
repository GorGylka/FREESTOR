#release: all
#	psp-packer -y EBOOT.PBP
#	bin2c EBOOT.PBP $(TARGET)_bin.c $(TARGET)
#	sed 's/static//g' $(TARGET)_bin.c
#	rm -f *.o

TARGET = downloader
OBJS = src/main.o src/download.o

INCDIR = ../../include
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1

LIBDIR = ../../libs
LDFLAGS =
LIBS = -lpsputility -lpspgum -lpspgu -lpsprtc -lpsppower -lpsphttp -lm
# LIBS += 

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = FREESTOR
PSP_EBOOT_ICON = ICON0.PNG 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
