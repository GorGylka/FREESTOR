TARGET = curl_test

SRC_CXX  += $(wildcard src/*.cpp)

OBJS = $(SRC_CXX:%.cpp=%.o)

INCDIR   = include
CFLAGS   = -G0 -O2 -D__GLIBC_USE\(...\)=0
CXXFLAGS = $(CFLAGS) -fno-rtti -Wall -Wextra
ASFLAGS  = $(CFLAGS) -std=c++17

BUILD_PRX = 1

LIBDIR  =
LDFLAGS =
LIBS    = -lstdc++ -lpsphttp -lpspgum -lpspgu -lcurl -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspnet -lpspnet_apctl -lz -lm

EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = FREESTOR
PSP_EBOOT_ICON = ICON0.PNG 

PSPSDK = $(shell psp-config --pspsdk-path)
#USE_PSPSDK_LIBC=1
include $(PSPSDK)/lib/build.mak