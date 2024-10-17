#include <pspsdk.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspkernel.h>

#include "libpspexploit.h"
#include "slim_colors.h"
#include "colors_3g.h"

PSP_MODULE_INFO("Extened Colors", 0, 1, 1);

#define printf pspDebugScreenPrintf


static KernelFunctions _ktbl; KernelFunctions*k_tbl = &_ktbl;

SceCtrlData pad;

int (*_sceKernelGetModel)(void) = NULL;
u32 findGetModel() {
	u32 addr;
	for (addr = 0x88000000; addr < 0x88400000; addr += 4) {
		u32 data = _lw(addr);
		if (data == 0x3C03BC10) {
			return pspXploitFindFirstJALReverse(addr);
		}
	}
	return 0;
}

int prxCtrlReadBufferPositive(SceCtrlData*pad_data, int count)
{
    int k1 = pspSdkSetK1(0);
    int ret = sceCtrlReadBufferPositive(pad_data, count);
    pspSdkSetK1(k1);

    return ret;
}


int prxKernelGetModel(void) {
	int k1 = pspSdkSetK1(0);
	int g = _sceKernelGetModel();
	pspSdkSetK1(k1);
	return g;
}

int (*_sceCtrlReadBufferPositive)(SceCtrlData *, int) = NULL;
void kmain_thread() {

	if(_sceKernelGetModel() > 0) {
		pspDebugScreenSetTextColor(0xAA00FF00);
		printf("Model already has the colors!");
		k_tbl->KernelDelayThread(1000000);
		return;
	}
	k_tbl->IoUnassign("flash0:");
	k_tbl->IoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	int clr = k_tbl->KernelIOOpen("flash0:/vsh/resource/13-27.bmp", PSP_O_RDONLY, 0);
	int _3g = k_tbl->KernelIOOpen("flash0:/vsh/resource/01-12.bmp", PSP_O_RDONLY, 0);
	if(clr >= 0) {
		pspDebugScreenSetTextColor(0xAA00CCCC);
		printf("Files already on system...\n");
		k_tbl->KernelDelayThread(1000000);
		k_tbl->KernelIOClose(clr);
		int flash_3g = k_tbl->KernelIOOpen("flash0:/vsh/resource/01-12.bmp", PSP_O_RDONLY, 0);
		u8 buf1[size_colors_3g];
		int flash_3g_r = k_tbl->KernelIORead(flash_3g, buf1, size_colors_3g);
		int diff = -1;
		if(memcmp(buf1, colors_3g, size_colors_3g) != 0) {
			diff = 1;
			printf("Would you like to overwrite the original 01-12.bmp with the slim\nversions?\n\n");
			printf("X = Overwrite\nO = Exit\n");
		}
		else {
			return;
		}

		while(diff) {
			k_tbl->KernelDelayThread(8000);
			prxCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons == 0x1a044000 || pad.Buttons == PSP_CTRL_CROSS) { // CROSS
				k_tbl->KernelIOClose(flash_3g);
				printf("Overwriting 01-12.bmp with Slim versions\n");
				k_tbl->KernelIORemove("flash0:/vsh/resource/01-12.bmp");
				flash_3g = k_tbl->KernelIOOpen("flash0:/vsh/resource/01-12.bmp", PSP_O_WRONLY | PSP_O_CREAT, 0777);
				k_tbl->KernelIOWrite(flash_3g, colors_3g, size_colors_3g);
				k_tbl->KernelIOClose(flash_3g);
				k_tbl->KernelDelayThread(1000000);
				return;
			}
			if(pad.Buttons == 0x1a042000 || pad.Buttons == PSP_CTRL_CIRCLE) { // CIRCLE
				k_tbl->KernelIOClose(flash_3g);
				break;
			}

		}
		return;
	}
	else {
		pspDebugScreenSetTextColor(0xAA00FF00);
		printf("Writing 13-27.bmp to flash0! DO NOT TURN OFF!\n");
		clr = k_tbl->KernelIOOpen("flash0:/vsh/resource/13-27.bmp", PSP_O_WRONLY | PSP_O_CREAT, 0777);
		k_tbl->KernelIOWrite(clr, slim_colors, size_slim_colors);
		k_tbl->KernelIOClose(clr);
		k_tbl->KernelDelayThread(1000000);
		int flash_3g = k_tbl->KernelIOOpen("flash0:/vsh/resource/01-12.bmp", PSP_O_RDONLY, 0);
		u8 buf1[size_colors_3g];
		int flash_3g_r = k_tbl->KernelIORead(flash_3g, buf1, size_colors_3g);
		int diff = -1;
		if(memcmp(buf1, colors_3g, size_colors_3g) != 0) {
			diff = 1;
			printf("Would you like to overwrite the original 01-12.bmp with the slim\nversions?\n\n");
			printf("X = Overwrite\nO = Exit\n");
		}
		else {
			return;
		}

		while(diff) {
			k_tbl->KernelDelayThread(8000);
			prxCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons == 0x1a044000 || pad.Buttons == PSP_CTRL_CROSS) { // CROSS
				k_tbl->KernelIOClose(flash_3g);
				printf("Overwriting 01-12.bmp with Slim versions\n");
				k_tbl->KernelIORemove("flash0:/vsh/resource/01-12.bmp");
				flash_3g = k_tbl->KernelIOOpen("flash0:/vsh/resource/01-12.bmp", PSP_O_WRONLY | PSP_O_CREAT, 0777);
				k_tbl->KernelIOWrite(flash_3g, colors_3g, size_colors_3g);
				k_tbl->KernelIOClose(flash_3g);
				k_tbl->KernelDelayThread(1000000);
				return;
			}
			if(pad.Buttons == 0x1a042000 || pad.Buttons == PSP_CTRL_CIRCLE) { // CIRCLE
				k_tbl->KernelIOClose(flash_3g);
				break;
			}

		}

		return;
	}


}

void kmain() {
	int k1 = pspSdkSetK1(0);
	int ul = pspXploitSetUserLevel(8);
	pspXploitRepairKernel();
	pspXploitScanKernelFunctions(k_tbl);

	SceUID kthreadID = k_tbl->KernelCreateThread("kmain_kthread", (void*)KERNELIFY(&kmain_thread), 1, 0x20000, PSP_THREAD_ATTR_VFPU, NULL);
	_sceKernelGetModel = findGetModel();
	if (kthreadID >= 0){
		k_tbl->KernelStartThread(kthreadID, 0, NULL);
		k_tbl->waitThreadEnd(kthreadID, NULL);
	}
	pspXploitSetUserLevel(ul);
	pspSdkSetK1(k1);
}

int main(int argc, char *args[]) {

	pspDebugScreenInit();
	int res = pspXploitInitKernelExploit();
	if(res == 0) {
		res = pspXploitDoKernelExploit();
		if(res == 0)
			pspXploitExecuteKernel(kmain);
	}

	sceKernelExitGame();

	return 0;
}

