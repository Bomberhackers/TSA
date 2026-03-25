#include <ultra64.h>
#include "file.h"
#include "decode.h"
#include "fblock.h"

// fblock.c

extern s32 D_80092AFC;
extern s32 D_80092B24;

extern s32 D_8009E0A0;
extern u32 D_8009E0A4;
extern s32 D_8009E0A8;

extern s32 D_80092AB0;
extern s32 D_80092AD8;

extern u8 D_800EE0B0[][3];
extern u8 D_800ED0B0[][2];

extern s32 (*D_8009E0AC)();

extern u32 D_8008C7A0[];

// FUNCTIONS

static __inline void info_setup(void);
static __inline void fchangeblockaddr(s32 arg0);

void fexecLoad(s32 id) {
    if (id != D_8009E0A8) {
        D_8009E0A8 = id;
        D_8009E0AC = fexecLoadAddress(id, 0x80250000);
    }
}

u32 fexecLoadAddress(s32 id, u32 (*func)()) {
    HuFILE *stream;
    s32 i;
    int evfile_found;
    s32 temp_a2;
    s32 pad[5];
    int size = 0x2A0000;

    temp_a2 = (D_800ED0B0[id][0] << 0x11) + (D_800ED0B0[id][1] << 0xB);
    if (temp_a2 != D_8009E0A4) {
        D_8009E0A4 = temp_a2;
        finit(temp_a2);
    }
    stream = fopen(0, 1);
    i = fsize(stream);

    if (i >= 0x202) {
        errstop(&D_80092AB0, D_800ED0B0[id][0]); // "too long file size : evinfo : block %d\n"
    }

    fread((void*)&D_800EE0B0, 1, i, stream);

    for (i = 0; i < D_800EE0B0[0][0]; i++) {
        evfile_found = (D_800EE0B0[i][1] << 8) + D_800EE0B0[i][2];
        if (evfile_found == id) {
            evfile_found = D_800EE0B0[i + 1][0];
            break;
        }
    }

    if (i >= D_800EE0B0[0][0]) {
        errstop(&D_80092AD8, id); // "Can not find evfile number : %d\n"
    }

    fclose(stream);
    stream = fopen(evfile_found, 1);
    fread((u8*)&i, 4, 1, stream);
    fseek(stream, 4, 1);
    osInvalICache(((u32) ((u32)func + 0xF) >> 4) * 0x10, 0x80000);
    osInvalDCache(((u32) ((u32)func + 0xF) >> 4) * 0x10, 0x80000);
    decode(stream, func, i);
    osWritebackDCache(((u32) ((u32)func + 0xF) >> 4) * 0x10, 0x80000);
    fclose(stream);
    if (D_8009E0A4 != size) {
        D_8009E0A4 = size;
        finit(size);
    }
    return func();
}

s32 fexecGetFileSize(s32 id) {
    HuFILE *stream;
    s32 i;
    s32 sp3C;
    int evfile_found;
    s32 temp_a2;
    s32 pad[4];
    int size = 0x2A0000;

    temp_a2 = (D_800ED0B0[id][0] << 0x11) + (D_800ED0B0[id][1] << 0xB);
    if (temp_a2 != D_8009E0A4) {
        D_8009E0A4 = temp_a2;
        finit(temp_a2);
    }
    stream = fopen(0, 1);
    i = fsize(stream);

    if (i >= 0x202) {
        errstop(&D_80092AFC, D_800ED0B0[id][0]);
    }

    fread((void*)&D_800EE0B0, 1, i, stream);

    for (i = 0; i < D_800EE0B0[0][0]; i++) {
        evfile_found = (D_800EE0B0[i][1] << 8) + D_800EE0B0[i][2];
        if (evfile_found == id) {
            evfile_found = D_800EE0B0[i + 1][0];
            break;
        }
    }

    if (i >= D_800EE0B0[0][0]) {
        errstop(&D_80092B24, id);
    }

    fclose(stream);
    stream = fopen(evfile_found, 1);
    fread((u8*)&i, 4, 1, stream);
    fread(&sp3C, 4, 1, stream);
    fclose(stream);

    if (D_8009E0A4 != size) {
        D_8009E0A4 = size;
        finit(size);
    }
    return i + sp3C;
}

void fexecCall(void) {
    D_8009E0AC();
}

void fexecInit(void) {
    int size = 0x2A0000;

    zjSetVec(4, &D_8008C7A0);
    D_8009E0A0 = -1;
    D_8009E0A4 = -1;
    D_8009E0A8 = -1;
    if (D_8009E0A0 == -1) {
        info_setup();
        D_8009E0A0 = 0;
    }
    if (D_8009E0A4 != size) {
        D_8009E0A4 = size;
        finit(0x2A0000);
    }
}

static __inline void info_setup(void) {
    dmaRead(&D_800ED0B0, 0x1000, 0xFF000);
}

void fexecChangeBlock(s32 arg0) {
    u32 temp = arg0 << 0x11;

    if (D_8009E0A4 != temp) {
        fchangeblockaddr(temp);
    }
}

static __inline void fchangeblockaddr(s32 arg0) {
    D_8009E0A4 = arg0;
    finit(arg0);
}
