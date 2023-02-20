/*
 *  Copyright (C) 2014 qwikrazor87
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <string.h>
#include "lib.h"

void ClearCaches(void)
{
	sceKernelDcacheWritebackInvalidateAll();
	sceKernelIcacheInvalidateAll();
}

u32 FindTextAddrByName(const char *module)
{
	u32 kaddr;
	for (kaddr = 0x88000000; kaddr < 0x88400000; kaddr += 4) {
		if (strcmp((const char *)kaddr, module) == 0) {
			if ((*(u32*)(kaddr + 0x64) == *(u32*)(kaddr + 0x78)) && \
				(*(u32*)(kaddr + 0x68) == *(u32*)(kaddr + 0x88))) {
				if (*(u32*)(kaddr + 0x64) && *(u32*)(kaddr + 0x68))
					return *(u32*)(kaddr + 0x64);
			}
		}
	}
	return 0;
}

u32 FindExport(const char *module, const char *library, u32 nid)
{
	u32 addr = FindTextAddrByName(module);

	if (addr) {
		u32 maxaddr = 0x88400000;

		if (addr >= 0x08800000 && addr < 0x0A000000)
			maxaddr = 0x0A000000;
		else if (addr >= 0x08400000 && addr < 0x08800000)
			maxaddr = 0x08800000;

		for (; addr < maxaddr; addr += 4) {
			if (strcmp(library, (const char *)addr) == 0) {
				u32 libaddr = addr;

				while (*(u32*)(addr -= 4) != libaddr);

				u32 exports = (u32)(*(u16*)(addr + 10) + *(u8*)(addr + 9));
				u32 jump = exports * 4;

				addr = *(u32*)(addr + 12);

				while (exports--) {
					if (*(u32*)addr == nid)
						return *(u32*)(addr + jump);

					addr += 4;
				}

				return 0;
			}
		}
	}

	return 0;
}

void fillvram(u32 color)
{
	u32 vram;

	for (vram = 0x44000000; vram < 0x44088000; vram += 4)
		_sw(color, vram);
}

int ValidUserAddress(void *addr, int mode)
{
	if (mode)
		return ((u32)addr >= 0x08400000 && (u32)addr < 0x08800000);

	return ((u32)addr >= 0x08800000 && (u32)addr < 0x0A000000);
}

u32 FindImport(char *libname, u32 nid, int mode)
{
	u32 lower = 0x08400000;
	u32 higher = 0x08800000;

	if (!mode) {
		lower = 0x08800000;
		higher = 0x0A000000;
	}

	u32 i;

	for (i = lower; i < higher; i += 4) {
		SceLibraryStubTable *stub = (SceLibraryStubTable *)i;

		if ((stub->libname != libname) && ValidUserAddress((void *)stub->libname, mode) \
			&& ValidUserAddress(stub->nidtable, mode) && ValidUserAddress(stub->stubtable, mode)) {
			if (strcmp(libname, stub->libname) == 0) {
				u32 *table = stub->nidtable;

				int j;

				for (j = 0; j < stub->stubcount; j++) {
					if (table[j] == nid) {
						return ((u32)stub->stubtable + (j * 8));
					}
				}
			}
		}
	}

	return 0;
}

void logstr(const char *str)
{
	SceUID fd = sceIoOpen("ms0:/log.txt", 0x302, 0777);
	sceIoWrite(fd, str, strlen(str));
	sceIoClose(fd);
}

static u8 kirk_buf[0x0814];

static int kirk7(u8 *buf, int size, int type)
{
	int retv;
	u32 *header = (u32 *)buf;

	header[0] = 5;
	header[1] = 0;
	header[2] = 0;
	header[3] = type;
	header[4] = size;

	retv = sceUtilsBufferCopyWithRange(buf, size + 0x14, buf, size, 7);

	if (retv)
		return 0x80510311;

	return 0;
}

int bbmac_getkey(MAC_KEY *mkey, u8 *bbmac, u8 *vkey)
{
	int i, retv, type, code;
	u8 *kbuf, tmp[16], tmp1[16];

	type = mkey->type;
	retv = sceDrmBBMacFinal(mkey, tmp, NULL);

	if (retv)
		return retv;

	kbuf = kirk_buf + 0x14;

	if (type == 3) {
		memcpy(kbuf, bbmac, 0x10);
		kirk7(kirk_buf, 0x10, 0x63);
	} else
		memcpy(kirk_buf, bbmac, 0x10);

	memcpy(tmp1, kirk_buf, 16);
	memcpy(kbuf, tmp1, 16);

	code = (type == 2) ? 0x3A : 0x38;
	kirk7(kirk_buf, 0x10, code);

	for (i = 0; i < 0x10; i++)
		vkey[i] = tmp[i] ^ kirk_buf[i];

	return 0;
}

void dumpPS1key(const char *path)
{
	int flag = 2;
	PGD_HEADER PGD;
	memset(&PGD, 0, sizeof(PGD_HEADER));
	MAC_KEY mkey;
	u8 buf[1024];

	SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
	sceIoLseek(fd, 0x24, PSP_SEEK_SET);
	u32 psar, pgdoff = 0;

	sceIoRead(fd, &psar, 4);
	sceIoLseek(fd, psar, PSP_SEEK_SET);
	sceIoRead(fd, buf, 16);

	if (!memcmp(buf, "PSTITLE", 7))
		pgdoff = psar + 0x200;
	else if (!memcmp(buf, "PSISO", 5))
		pgdoff = psar + 0x400;
	else {
		sceIoClose(fd);
		return;
	}

	sceIoLseek(fd, pgdoff, PSP_SEEK_SET);
	sceIoRead(fd, buf, sizeof(buf));
	sceIoClose(fd);

	PGD.buf = buf;
	PGD.key_index = *(u32*)(buf + 4);
	PGD.drm_type = *(u32*)(buf + 8);

	// Set the hashing, crypto and open modes.
	if (PGD.drm_type == 1) {
		PGD.mac_type = 1;
		flag |= 4;

		if (PGD.key_index > 1) {
			PGD.mac_type = 3;
			flag |= 8;
		}

		PGD.cipher_type = 1;
	} else {
		PGD.mac_type = 2;
		PGD.cipher_type = 2;
	}

	PGD.open_flag = flag;

	sceDrmBBMacInit(&mkey, PGD.mac_type);
	sceDrmBBMacUpdate(&mkey, buf, 0x70);

	bbmac_getkey(&mkey, buf + 0x70, PGD.vkey);

	char Path[256];
	strcpy(Path, path);
	int len = strlen(Path);

	while (Path[len] != '/')
		len--;

	Path[len + 1] = 0;
	strcat(Path, "KEYS.BIN");
	fd = sceIoOpen(Path, 0x602, 0777);
	sceIoWrite(fd, PGD.vkey, 16);
	sceIoClose(fd);
}
