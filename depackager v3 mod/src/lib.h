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

#ifndef LIB_H_
#define LIB_H_

#include <psptypes.h>

void fillvram(u32 color);

void ClearCaches(void);

u32 FindImport(char *libname, u32 nid, int mode);
u32 FindTextAddrByName(const char *module);
u32 FindExport(const char *module, const char *library, u32 nid);
u32 pspSdkSetK1(u32 k1);
void logstr(const char *str);

#define U_EXTRACT_IMPORT(x) ((((u32)_lw((u32)x)) & ~0x08000000) << 2)
#define K_EXTRACT_IMPORT(x) (((((u32)_lw((u32)x)) & ~0x08000000) << 2) | 0x80000000)
#define U_EXTRACT_CALL(x) ((((u32)_lw((u32)x)) & ~0x0C000000) << 2)
#define K_EXTRACT_CALL(x) (((((u32)_lw((u32)x)) & ~0x0C000000) << 2) | 0x80000000)

#define MAKE_JUMP(f) (0x08000000 | (((u32)(f) >> 2)  & 0x03FFFFFF))
#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2)  & 0x03FFFFFF))

#define MAKE_JUMP_PATCH(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);

#define HIJACK_FUNCTION(a, f, ptr) \
{ \
	u32 func = a; \
	static u32 patch_buffer[3]; \
	_sw(_lw(func), (u32)patch_buffer); \
	_sw(_lw(func + 4), (u32)patch_buffer + 8);\
	MAKE_JUMP_PATCH((u32)patch_buffer + 4, func + 8); \
	_sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), func); \
	_sw(0, func + 4); \
	ptr = (void *)patch_buffer; \
}

typedef struct {
	unsigned char vkey[16];

	int open_flag;
	int key_index;
	int drm_type;
	int mac_type;
	int cipher_type;

	int data_size;
	int align_size;
	int block_size;
	int block_nr;
	int data_offset;
	int table_offset;

	unsigned char *buf;
} PGD_HEADER;

typedef struct {
	int type;
	u8 key[16];
	u8 pad[16];
	int pad_size;
} MAC_KEY;

int sceDrmBBMacInit(MAC_KEY *mkey, int type);
int sceDrmBBMacUpdate(MAC_KEY *mkey, u8 *buf, int size);
int sceDrmBBMacFinal(MAC_KEY *mkey, u8 *buf, u8 *vkey);
int sceDrmBBMacFinal2(MAC_KEY *mkey, u8 *out, u8 *vkey);
int sceUtilsBufferCopyWithRange(void *,  int, void *, int, int);
void dumpPS1key(const char *path);

#endif /* LIB_H_ */
