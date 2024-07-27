#include <pspsdk.h>
#include <pspdebug.h>
#include <pspkernel.h>

#include "libpspexploit.h"
#include "colors.h"

PSP_MODULE_INFO("Extened Colors", 0, 1, 0);

#define printf pspDebugScreenPrintf


static KernelFunctions _ktbl; KernelFunctions*k_tbl = &_ktbl;

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

int prxKernelGetModel(void) {
	int k1 = pspSdkSetK1(0);
	int g = _sceKernelGetModel();
	pspSdkSetK1(k1);
	return g;
}

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
	if(clr >= 0) {
		pspDebugScreenSetTextColor(0xAA00CCCC);
		printf("Files already on system exiting...\n");
		k_tbl->KernelDelayThread(1000000);
		k_tbl->KernelIOClose(clr);
		return;
	}
	else {
		pspDebugScreenSetTextColor(0xAA00FF00);
		printf("Writing 13-27.bmp to flash0! DO NOT TURN OFF!\n");
		clr = k_tbl->KernelIOOpen("flash0:/vsh/resource/13-27.bmp", PSP_O_WRONLY | PSP_O_CREAT, 0777);
		k_tbl->KernelIOWrite(clr, colors, size_colors);
		k_tbl->KernelIOClose(clr);
		printf("Done. Exiting...");
		k_tbl->KernelDelayThread(1000000);
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

