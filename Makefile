TARGET = eboot

OBJS = main.o


CFLAGS = -std=c99

PSP_EBOOT_TITLE = Slim Colors for Phat

PSP_EBOOT_ICON = ICON0.PNG

EXTRA_TARGETS = EBOOT.PBP


BUILD_PRX = 1

INCDIR = ./inc
LIBDIR = ./lib
LIBS = -lpspexploit -lpsprtc


PSPSDK = $(shell psp-config -p)
include $(PSPSDK)/lib/build.mak
