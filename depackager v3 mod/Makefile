TARGET = PSP_depackager
OBJS =  src/main.o src/exports.o src/lib.o src/setk1.o src/imports.o src/aes.o

all:
	rm -rf EBOOT.PBP
	pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL PSP_depackager.prx NULL
	make clean
	

CFLAGS = -Wall -Wextra -O2 -G0
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1



PSP_FW_VERSION=660

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Game Depackager
PSP_EBOOT_ICON = ICON0.PNG  

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
