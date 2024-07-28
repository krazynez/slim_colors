TARGET = eboot

OBJS = main.o


CFLAGS = -std=c99

PSP_EBOOT_TITLE = Slim Colors for Phat

PSP_EBOOT_ICON = ICON0.PNG

EXTRA_TARGETS = EBOOT.PBP


BUILD_PRX = 1

INCDIR = ./inc
LIBDIR = ./lib
LIBS = -lpspexploit -lpsprtc -lpspctrl


PSPSDK = $(shell psp-config -p)
include $(PSPSDK)/lib/build.mak

clean:
	rm -rf $(TARGET).prx $(TARGET).elf $(OBJS) PARAM.SFO $(EXTRA_TARGETS) PSP *.zip


pkg: clean all
	-mkdir -p PSP/GAME/Slim_Colors_For_Phat
	-cp EBOOT.PBP PSP/GAME/Slim_Colors_For_Phat/
	-zip -r Slim_Colors_For_Phat.zip README.md PSP

