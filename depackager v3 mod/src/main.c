#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <psploadexec_kernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <string.h>
#include <stdio.h>
#include "lib.h"
#include "sctrl.h"
#include "aes.h"

PSP_MODULE_INFO("PSP_depackager", PSP_MODULE_KERNEL, 1, 0);
PSP_HEAP_SIZE_KB(128);

int homebrew_version = 3;

int sceDisplaySetFrameBufForKernel(void *, int, int, int);
void sceDisplayWaitVblankStart(void);
int sceMgrAESDecrypt(void *dst, void *src, int size, u8 *key, u8 *iv);
int sceMgrAESEncrypt(void *dst, void *src, int size, u8 *key, u8 *iv);
int sceMgrAESInit(void);

//#define MAGICGATE

char *pkgBuf, *tmpBuf;

u8 public_key[16], static_public_key[16], xor_key[16], gameid[10];

u8 PSPAESKey[16] __attribute__((aligned(16))) = {
	0x07, 0xF2, 0xC6, 0x82, 0x90, 0xB5, 0x0D, 0x2C, 0x33, 0x81, 0x8D, 0x70, 0x9B, 0x60, 0xE6, 0x2B
};

u8 PS3AESKey[16] __attribute__((aligned(16))) = {
	0x2E, 0x7B, 0x71, 0xD7, 0xC9, 0xC9, 0xA1, 0x4E, 0xA3, 0x22, 0x1F, 0x18, 0x88, 0x28, 0xB8, 0xF8
};

void printScreenY_(int y, const char *fmt, int color, int a1, int a2, int a3, int t0, int t1, int t2, int t3)
{
	pspDebugScreenSetXY(0, y);
	pspDebugScreenSetTextColor(color);
	pspDebugScreenKprintf(fmt, a1, a2, a3, t0, t1, t2, t3);
}

void (* printScreenY)(int y, const char *, int color, ...) = (void *)&printScreenY_;

void printScreenXY_(int x, int y, const char *fmt, int color, int a1, int a2, int a3, int t0, int t1, int t2, int t3)
{
	pspDebugScreenSetXY(x, y);
	pspDebugScreenSetTextColor(color);
	pspDebugScreenKprintf(fmt, a1, a2, a3, t0, t1, t2, t3);
}

void (* printScreenXY)(int x, int y, const char *, int color, ...) = (void *)&printScreenXY_;

char strbuf[256];
void printScreenCenter_(int y, const char *fmt, int color, int a1, int a2, int a3, int t0, int t1, int t2, int t3)
{
	sprintf(strbuf, fmt, a1, a2, a3, t0, t1, t2, t3);
	int len = strlen(strbuf);

	pspDebugScreenSetXY(34 - (len / 2), y);
	pspDebugScreenSetTextColor(color);
	pspDebugScreenKprintf(strbuf);
}

void (* printScreenCenter)(int y, const char *, int color, ...) = (void *)&printScreenCenter_;

SceCtrlData pad;
SceIoDirent Dir;
SceIoDirent DirList[256];
int dircount = 0;
u32 Buttons = 0;

int padPress(int buttons)
{
	if (Buttons != pad.Buttons)
		return pad.Buttons & buttons;

	return 0;
}

u32 toU32(char *_buf)
{
	u8 *buf = (u8 *)_buf;
	u32 b1 = buf[0] << 24;
	u32 b2 = buf[1] << 16;
	u32 b3 = buf[2] << 8;
	u32 size = b1 | b2 | b3 | buf[3];

	return size;
}

int is_pkg_supported(const char *file)
{
	char buf[256];
	sprintf(buf, "ms0:/pkg/%s", file);

	SceUID fd = sceIoOpen(buf, PSP_O_RDONLY, 0777);

	if (fd >= 0) {
		sceIoRead(fd, buf, 4);
		sceIoClose(fd);

		return memcmp(buf, "\x7FPKG", 4) == 0;
	} else
		return 0;
}

int is_pkg_type_supported(const char *file)
{
	char buf[256];
	sprintf(buf, "ms0:/pkg/%s", file);

	SceUID fd = sceIoOpen(buf, PSP_O_RDONLY, 0777);

	if (fd >= 0) {
		sceIoRead(fd, buf, 8);
		sceIoClose(fd);

		return memcmp(buf, "\x7FPKG\x80\x00\x00\x02", 8) == 0;
	} else
		return 0;
}

void view_pkg_info(const char *file)
{
	sceDisplayWaitVblankStart();
	pspDebugScreenClear();
	printScreenCenter(0, "PSP depackager v%d", 0xFFFF00, homebrew_version);
	printScreenCenter(1, "PKG info viewer", 0xFF00);

	char buf[256];
	sprintf(buf, "ms0:/pkg/%s", file);

	SceUID fd = sceIoOpen(buf, PSP_O_RDONLY, 0777);
	sceIoRead(fd, buf, sizeof(buf));
	sceIoClose(fd);

	u8 version = (u8)buf[4];

	printScreenXY(0, 2, "PKG type:       %s", -1, buf[7] == 1 ? "PS3 (currently unsupported)" : buf[7] == 2 ? "PSP" : "unknown");
	printScreenXY(0, 3, "PKG version:    %s", -1, (version == 0x80) ? "retail" : (version == 0x90) ? "debug (currently unsupported)" : "unknown");
	printScreenXY(0, 4, "Content ID:     %s", -1, buf + 0x30);
	printScreenXY(0, 5, "PKG file count: %d", -1, toU32(buf + 0x14));
	printScreenXY(0, 6, "PKG size:       %d bytes", -1, toU32(buf + 0x1C));
	printScreenXY(0, 7, "Triangle - Go back", 0xFF00);

	while (!padPress(PSP_CTRL_TRIANGLE)) {
		sceCtrlPeekBufferPositive(&pad, 1);
		sceKernelDelayThread(0);
	}

	pspDebugScreenClear();
}

void xor128(u8 *dst, u8 *xor1, u8 *xor2)
{
	int i;

	for (i = 0; i < 16; i++)
		dst[i] = xor1[i] ^ xor2[i];
}

void iter128(u8 *buf)
{
	int i;

	for (i = 15; i >= 0; i--) {
		buf[i]++;

		if (buf[i])
			break;
	}
}

void setiter128(u8 *dst, int size)
{
	memcpy(dst, static_public_key, 16);

	int i;

	for (i = 0; i < size; i++)
		iter128(dst);
}

void install_pkg(const char *file)
{
	SceIoStat stat;
	int keysbin = 0;

	sceDisplayWaitVblankStart();
	pspDebugScreenClear();
	printScreenCenter(0, "PSP depackager v%d", 0xFFFF00, homebrew_version);
	printScreenCenter(1, "PSP PKG installer", 0xFF00);

	if (!is_pkg_type_supported(file)) {
		printScreenXY(0, 2, "Nothing to install!", 0xFF);
		printScreenXY(0, 3, "Try somehow press TRIANGLE and HOME simultaneously to exit", 0xFF00);

		while (!padPress(PSP_CTRL_TRIANGLE)) {
			sceCtrlPeekBufferPositive(&pad, 1);
			sceKernelDelayThread(0);
		}

		pspDebugScreenClear();

		return;
	}

	SceUID blockid = sceKernelAllocPartitionMemory(2, "buffer", PSP_SMEM_Low, 1024 * 1024, NULL); //1 MiB buffer

	if (blockid & 0x80000000) {
		printScreenXY(0, 2, "Error allocating memory: 0x%08X", 0xFF, blockid);
		printScreenXY(0, 3, "Triangle - Go back", 0xFF00);

		while (!padPress(PSP_CTRL_TRIANGLE)) {
			sceCtrlPeekBufferPositive(&pad, 1);
			sceKernelDelayThread(0);
		}

		pspDebugScreenClear();

		return;
	}

	pkgBuf = (char *)sceKernelGetBlockHeadAddr(blockid);

	SceUID blockid2 = sceKernelAllocPartitionMemory(2, "buffer2", PSP_SMEM_Low, 1024 * 1024, NULL); //1 MiB buffer

	if (blockid2 & 0x80000000) {
		printScreenXY(0, 2, "Error allocating memory: 0x%08X", 0xFF, blockid2);
		printScreenXY(0, 3, "Triangle - Go back", 0xFF00);

		while (!padPress(PSP_CTRL_TRIANGLE)) {
			sceCtrlPeekBufferPositive(&pad, 1);
			sceKernelDelayThread(0);
		}

		sceKernelFreePartitionMemory(blockid);

		pspDebugScreenClear();

		return;
	}

	tmpBuf = (char *)sceKernelGetBlockHeadAddr(blockid2);

	char buf[256];
	sprintf(buf, "ms0:/pkg/%s", file);

	SceUID fd = sceIoOpen(buf, PSP_O_RDONLY, 0777);
	SceSize fdsize = sceIoLseek(fd, 0, PSP_SEEK_END);
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	sceIoRead(fd, pkgBuf, 256);

	if (toU32(pkgBuf + 0x1C) != fdsize) {
		sceIoClose(fd);
		printScreenXY(0, 2, "Corrupt PKG detected", 0xFF);
		printScreenXY(0, 3, "detected size: %d bytes", -1, fdsize);
		printScreenXY(0, 4, "expected size: %d bytes", -1, toU32(pkgBuf + 0x1C));
		printScreenXY(0, 5, "Triangle - Go back", 0xFF00);

		while (!padPress(PSP_CTRL_TRIANGLE)) {
			sceCtrlPeekBufferPositive(&pad, 1);
			sceKernelDelayThread(0);
		}

		sceKernelFreePartitionMemory(blockid);
		sceKernelFreePartitionMemory(blockid2);

		pspDebugScreenClear();

		return;
	}

	sceIoLseek(fd, toU32(pkgBuf + 8) + 0x48, PSP_SEEK_SET);
	sceIoRead(fd, gameid, 9);
	gameid[9] = 0;

	memcpy(public_key, pkgBuf + 0x70, 16);
	memcpy(static_public_key, pkgBuf + 0x70, 16);

	u32 enc_start = toU32(pkgBuf + 0x24);
	u32 files = toU32(pkgBuf + 0x14);

	sceIoLseek(fd, enc_start, PSP_SEEK_SET);
	sceIoRead(fd, tmpBuf, files * 32);

#ifdef MAGICGATE
	sceMgrAESInit();
#endif

	int i, j, pspcount = 0;
	u32 file_name[files], file_name_len[files], file_offset[files], file_size[files], is_file[files];

	for (i = 0; i < (int)(files * 2); i++) {
#ifdef MAGICGATE
		sceMgrAESEncrypt(xor_key, public_key, 16, PSPAESKey, NULL);
#else
		AES128_ECB_encrypt(public_key, PSPAESKey, xor_key);
#endif
		xor128((u8 *)(tmpBuf + (i * 16)), (u8 *)(tmpBuf + (i * 16)), xor_key);
		iter128(public_key);
	}

	for (i = 0; i < (int)files; i++) {
		if (((u8 *)tmpBuf)[(i * 32) + 24] == 0x90) {
			file_name[pspcount] = toU32(tmpBuf + i * 32);
			file_name_len[pspcount] = toU32(tmpBuf + i * 32 + 4);
			file_offset[pspcount] = toU32(tmpBuf + i * 32 + 12);
			file_size[pspcount] = toU32(tmpBuf + i * 32 + 20);
			is_file[pspcount] = ((tmpBuf[i * 32 + 27] != 4) && file_size[pspcount]);
			pspcount++;
		}
	}

	int files_extracted = 0, do_extract = 0, do_overwrite = 0;

	for (i = 0; i < pspcount; i++) {
		sceIoLseek(fd, enc_start + file_name[i], PSP_SEEK_SET);
		int namesize = (file_name_len[i] + 15) & -16;
		sceIoRead(fd, tmpBuf, namesize);
		memcpy(public_key, pkgBuf + 0x70, 16);

		setiter128(public_key, file_name[i] >> 4);

		for (j = 0; j < (namesize >> 4); j++) {
#ifdef MAGICGATE
			sceMgrAESEncrypt(xor_key, public_key, 16, PSPAESKey, NULL);
#else
			AES128_ECB_encrypt(public_key, PSPAESKey, xor_key);
#endif
			xor128((u8 *)(tmpBuf + (j * 16)), (u8 *)(tmpBuf + (j * 16)), xor_key);
			iter128(public_key);
		}

		char path[256], tmp[256];
		sprintf(path, "ms0:/PSP/GAME/%s/%s", gameid, tmpBuf + 15);

		char *Path = path;
		int pl = 0;

		while (Path[pl]) {
			if (Path[pl] == '/') {
				memcpy(tmp, path, pl);
				tmp[pl] = 0;

				memset(&stat, 0, sizeof(SceIoStat));

				if (sceIoGetstat(tmp, &stat) < 0)
					sceIoMkdir(tmp, 0777);
			}

			pl++;
		}

		if (is_file[i]) {
			printScreenXY(0, 2, "Currently extracting:\n%s", -1, path);



			memset(&stat, 0, sizeof(SceIoStat));



			files_extracted++;

			sceIoLseek(fd, enc_start + file_offset[i], PSP_SEEK_SET);
			sceIoRead(fd, tmpBuf, 1024 * 1024);

			setiter128(public_key, file_offset[i] >> 4);

			SceUID dstfd = sceIoOpen(path, 0x602, 0777);

			u32 szcheck = 0, mincheck = 0;

			printScreenXY(0, 4, "%d/%d bytes", -1, mincheck, file_size[i]);

			for (j = 0; j < (int)(file_size[i] >> 4); j++) {
				if (szcheck == 1024 * 1024) {
					szcheck = 0;
					mincheck += 1024 * 1024;

					sceIoWrite(dstfd, tmpBuf, 1024 * 1024);
					sceIoRead(fd, tmpBuf, 1024 * 1024);

					printScreenXY(0, 4, "%d/%d bytes", -1, mincheck, file_size[i]);
				}

#ifdef MAGICGATE
				sceMgrAESEncrypt(xor_key, public_key, 16, PSPAESKey, NULL);
#else
				AES128_ECB_encrypt(public_key, PSPAESKey, xor_key);
#endif
				xor128((u8 *)((tmpBuf + (j * 16)) - mincheck), (u8 *)((tmpBuf + (j * 16)) - mincheck), xor_key);
				iter128(public_key);

				szcheck += 16;
			}

			if (mincheck < file_size[i])
				sceIoWrite(dstfd, tmpBuf, file_size[i] - mincheck);

			printScreenXY(0, 3, "\n\n\n\n", 0);

			sceIoClose(dstfd);

			int pathlen = strlen(path);

			if (!strcmp(path + pathlen - 9, "EBOOT.PBP")) {
				dstfd = sceIoOpen(path, PSP_O_RDONLY, 0777);
				sceIoLseek(dstfd, 0x24, PSP_SEEK_SET);
				u32 psar;
				sceIoRead(dstfd, &psar, 4);
				sceIoLseek(dstfd, psar, PSP_SEEK_SET);
				u8 block[16];
				sceIoRead(dstfd, block, 16);

				if (!memcmp(block, "PSTITLE", 7))
					sceIoLseek(dstfd, psar + 0x200, PSP_SEEK_SET);
				else if (!memcmp(block, "PSISO", 5))
					sceIoLseek(dstfd, psar + 0x400, PSP_SEEK_SET);

				sceIoRead(dstfd, block, 4);

				if (!memcmp(block, "\x00PGD", 4)) {
					dumpPS1key(path);
					keysbin = 1;
				}

				sceIoClose(dstfd);
			}
		}
	}

	sceIoClose(fd);

	sceKernelFreePartitionMemory(blockid);
	sceKernelFreePartitionMemory(blockid2);
	printScreenXY(0, 4, "files extracted: %d", -1, files_extracted);
	printScreenXY(0, 5, "Installation complete", -1);
	printScreenXY(0, 6, "Triangle - Go back", 0xFF00);
	pspDebugScreenClear();  
	printScreenCenter(16, "GAME INSTALLED", 0xFFFF00);
	sceIoRemove("ms0:/pkg/game.pkg");
	sceDisplayWaitVblankStart();
	sceKernelDelayThread(1000000);
	sceKernelExitVSHVSH(NULL);

	if (keysbin)
		printScreenXY(0, 7, "PS1 KEYS.BIN dumped", -1);

	while (!padPress(PSP_CTRL_TRIANGLE)) {
		sceCtrlPeekBufferPositive(&pad, 1);
		sceKernelDelayThread(0);
	}

	pspDebugScreenClear();
}


int thread_start(SceSize args __attribute__((unused)), void *argp __attribute__((unused)))
{
	pspDebugScreenInit();
	pspDebugScreenClear();

	printScreenCenter(0, "PSP depackager v%d", 0xFFFF00, homebrew_version);
	printScreenXY(0, 1, "********************************************************************", 0xFF00);
	printScreenXY(0, 32, "********************************************************************", 0xFF00);

	memset(&Dir, 0, sizeof(SceIoDirent));
	memset(&pad, 0, sizeof(SceCtrlData));

	SceUID dd = sceIoDopen("ms0:/pkg");

	SceUID mod = sceKernelLoadModule("flash0:/kd/mgr.prx", 0, NULL);
	sceKernelStartModule(mod, 0, NULL, NULL, NULL);

	if (dd >= 0) {
		while (sceIoDread(dd, &Dir) > 0) {
			if ((Dir.d_stat.st_mode & 0x2000) && is_pkg_supported(Dir.d_name)) {
				memcpy(&DirList[dircount], &Dir, sizeof(SceIoDirent));
				dircount++;
			}
		}

		sceIoDclose(dd);

		int i, j = 0, k = 0, max = 16;

		while (1) {
			sceCtrlPeekBufferPositive(&pad, 1);

			if (padPress(PSP_CTRL_UP)) {
				pspDebugScreenClear();
				printScreenXY(0, (j - k) + 2, "  ", 0);
				j--;

				if (j < 0) {
					j = 0;
					k = 0;
				}

				if (j < k)
					k = j;
			}

			if (padPress(PSP_CTRL_DOWN)) {
				pspDebugScreenClear();
				printScreenXY(0, (j - k) + 2, "  ", 0);
				j++;

				if (j >= dircount) {
					j = dircount - 1;

					if (dircount >= max)
						k = dircount - max;
					else
						k = 0;
				}

				if (j >= k + (max - 1))
					k = j - (max - 1);
			}

			if (padPress(PSP_CTRL_LEFT)) {
				pspDebugScreenClear();
				printScreenXY(0, (j - k) + 2, "  ", 0);
				j -= 5;

				if (j < 0) {
					j = 0;
					k = 0;
				}

				if (j < k)
					k = j;
			}

			if (padPress(PSP_CTRL_RIGHT)) {
				pspDebugScreenClear();
				printScreenXY(0, (j - k) + 2, "  ", 0);
				j += 5;

				if (j >= dircount) {
					j = dircount - 1;

					if (dircount >= max)
						k = dircount - max;
					else
						k = 0;
				}

				if (j >= k + (max - 1))
					k = (j - (max - 1)) >= 0 ? (j - (max - 1)) : j;
			}

			if (padPress(PSP_CTRL_SQUARE))
				view_pkg_info(DirList[j].d_name);

			
				install_pkg(DirList[j].d_name);

			if (padPress(PSP_CTRL_HOME)) {
				pspDebugScreenClear();
				printScreenCenter(16, "Goodbye World", 0xFFFF00);
				sceDisplayWaitVblankStart();
				sceKernelDelayThread(1000000);
				sceKernelExitVSHVSH(NULL);
			}

			printScreenXY(0, (j - k) + 2, "->", 0xFF);

			for (i = 0; i < max; i++) {
				if ((i + k) < dircount) {
					printScreenXY(2, i + 2, "                                                                ", 0);
					printScreenXY(2, i + 2, DirList[i + k].d_name, (DirList[i + k].d_stat.st_mode & 0x1000) ? 0xFFFF00 : (j == (i + k)) ? 0xFF : 0xFFFFFF);
				}
			}

			printScreenCenter(0, "PSP depackager v%d", 0xFFFF00, homebrew_version);
			printScreenXY(1, 1, "******************************************************************", 0xFF00);
			printScreenXY(1, 32, "******************************************************************", 0xFF00);
			printScreenXY(0, 19, "Supported PKG files detected: %d", 0xFF00, dircount);
			printScreenXY(0, 20, "[]         - view PKG info", 0xFF00);
			printScreenXY(0, 21, "X          - install PSP PKG", 0xFF00);
			printScreenXY(0, 22, "Up/Down    - scroll by one", 0xFF00);
			printScreenXY(0, 23, "Left/Right - scroll by five", 0xFF00);
			printScreenXY(0, 24, "Home       - exit", 0xFF00);

			Buttons = pad.Buttons;
			//sceKernelDelayThread(100);
			sceDisplayWaitVblankStart();
			sceKernelDelayThread(0);
		}
	} else {
		printScreenCenter(16, "Error opening directory ms0:/pkg/: 0x%08X\n", 0xFF, dd);
		sceKernelDelayThread(5000000);
		sceKernelExitVSHVSH(NULL);
	}

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("PSP_depackager_thread", thread_start, 32, 0x20000, 0, NULL);

	if (thid >= 0)
		sceKernelStartThread(thid, args, argp);

	return 0;
}

int module_stop(SceSize args __attribute__((unused)), void *argp __attribute__((unused)))
{
	return 0;
}
