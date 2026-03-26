#include <ultra64.h>
#include "file.h"

static __inline HuFILE *fsearchfree(void) {
    s32 i;

    for(i = 0; i < 4; i++) {
        if (D_800EEAC8[i].unk10 == 0) {
            return &D_800EEAC8[i];
        }
    }
    return NULL;
}

// FUNCTIONS

static __inline HuFILE *fsearchfree(void);
static __inline u32 filegetstat();
static __inline void _memcpy(u8 *dst, u8 *src, u32 size);
static __inline void filesetstat();
static void filereadfat(s32 saved_reg_s0); // NONMATCHING
static __inline void filefatflush(void);
static __inline void filewritecurrent(s32 arg0);

void finit(s32 arg0) {
    D_8009E07C = 0;
    D_800EEAD8 = 0;
    D_800EEAEC = 0;
    D_800EEB00 = 0;
    D_800EEB14 = 0;
    dmaRead(&D_800EE2C0, 8, arg0);
    D_8009E070 = arg0 + 8;
    D_800EE2C0[0] += arg0;
    D_8008C780 = 0;
    D_8008C784 = 0;
    D_8008C788 = 0;
    D_8008C78C = 0;
    D_8008C790 = 0;
    D_8009E080 = -1;
    D_8009E074 = 0;
}

int fsize(HuFILE *stream) {
    return stream->unkC;
}

HuFILE *fopen(u32 arg0, const char *mode) {
    HuFILE* file = NULL;
    HuFILE* var_v1;
    s32 temp_t9;
    int i;

    while (D_8008C788 != 0) {
        osYieldThread();
    }
    D_8008C788 = 1;
    filereadfat(arg0 >> 0x8);
    arg0 &= 0xFF;
    file = fsearchfree();


    if (file != NULL) {
        if (D_800EE2C8[((arg0))].unk0 == -1) {
            return NULL;
        }
        file->unk0 = arg0;
        file->unk4 = D_800EE2C0[0] + D_800EE2C8[(arg0)].unk0;
        file->unkC = D_800EE2C8[(arg0)].unk4;
        file->pos = 0;
        file->unk10 = mode;
    }

    D_8008C788 = 0;
    return file;
}

u32 fcreate(void) {
    u32 v1;
    u32 i;
    s32 sp50[2];
    u32 s3;
    u32 prevI;
    u32 sp44;

    // empty call to get right inline
    filegetstat();

    while (D_8008C78C != 0) {
        osYieldThread();
    }

    D_8008C78C = 1;
    for (i = 0; i < D_800EE2C0[1]; i++) {
        filereadfat(i >> 0x8);
        if (D_800EE2C8[((i & 0xFF))].unk0 == -1) {
            if (i == 0) {
                D_800EE2C8[((i & 0xFF))].unk0 = (D_800EE2C0[1] * 8) + 8;
                return i;
            }
            sp44 = -1;
            while (D_8008C780 != 0) {
                osYieldThread();
            }
            D_8008C780 = 1;
            // @fake / @hack
            prevI = D_8009E080;
            if (D_8009E080 != ((i - 1) >> 8)) {
                sp44 = D_8009E080;
                filereadfat(((i - 1) >> 8));
            }
            sp50[0] = D_800EE2C0[0] + D_800EE2C8[(i - 1) & 0xFF].unk0;
            sp50[1] = D_800EE2C8[(i - 1) & 0xFF].unk4;
            if (sp44 != -1) {
                filereadfat(sp44);
            }
            D_8008C780 = 0;
            D_800EE2C8[((i & 0xFF))].unk0 = sp50[1] + sp50[0];
            return i;
        }
    }

    D_8008C78C = 0;
    return 0U;
}

// unidentified inline. We dont need to define this, as it matches with a stub call.
static __inline u32 filegetstat(HuFILE *stream) {

}

void fclose(HuFILE* stream) {
    if (stream == D_8009E074) {
        D_8009E074 = 0;
    }
    stream->unk10 = 0;
}

/**
 * Gets the next character from the specified stream and advances the position
 * indicator for the stream.
 */
int fgetc(HuFILE* stream) {
    int c;

    // are we at or past EOF? Return -1 for an invalid fgetc.
    if (stream->pos >= stream->unkC) {
        c = -1;
    } else {
        // otherwise, load in the necessary chunk and get the character.
        readromtobuff(stream, stream->pos);
        c = D_800EEB18[stream->pos & 0x7FF];

        // increment position.
        stream->pos++;
    }
    return c;
}

size_t fread(u8 *buffer, size_t size, size_t count, HuFILE *stream) {
    s32 total_size;
    s32 var_s0;
    s32 var_s2;
    u32 temp_s5;
    u32 var_s1;
    s32 i;
    u32 pos;

    while (D_8008C790 != 0) {
        osYieldThread();
    }
    D_8008C790 = 1;
    if (!(stream->unk10 & 1)) {
        return 0;
    }
    total_size = size * count;
    if (stream->unkC < (stream->pos + total_size)) {
        total_size = ((total_size - stream->pos) - total_size) + stream->unkC;
    }
    var_s1 = stream->pos;
    pos = stream->pos + total_size;
    temp_s5 = (((u32) (pos - 1) >> 0xB) - (stream->pos >> 0xB)) + 1;
    var_s2 = total_size;
    for (i = 0; i < temp_s5; i++) {
        readromtobuff(stream, var_s1);
        if ((u32) ((var_s1 & 0x7FF) + var_s2) < 0x800U) {
            var_s0 = var_s2;
        } else {
            var_s0 = 0x800;
            var_s0 -= (var_s1 & 0x7FF);
        }
        memcpy(((u8*)buffer), (u8*)&D_800EEB18 + (var_s1 & 0x7FF), var_s0);
        osWritebackDCache(((u8*)buffer), var_s0);
        var_s1 += var_s0;
        buffer += var_s0;
        var_s2 -= var_s0;
    }
    pos = stream->pos + total_size;
    stream->pos = pos;
    D_8008C790 = 0;
    return total_size;
}

/**
 * Given a loaded file, load the 0x800 chunk determined from the pos into the current file buffer.
 */
void readromtobuff(HuFILE* arg0, s32 arg1) {
    s32 temp;

    if (arg0 != D_8009E074) {
        D_8009E074 = arg0;
        D_8009E078 = arg1 / 2048;
    } else {
        if (D_8009E078 == (arg1 / 2048)) {
            return;
        }
        D_8009E078 = arg1 / 2048;
    }

    if ((u32)arg0->unkC <= 0x800 || (u32)arg0->unkC >= (arg1 + 0x800)) {
        temp = 0x800;
    } else {
        temp = (u32)arg0->unkC - arg1;
        temp += temp & 1;
    }

    _memcpy(&D_800EEB18, temp, arg0->unk4 + (D_8009E078 << 0xB));
}

static __inline void _memcpy(u8 *dst, u8 *src, u32 size) {
    dmaRead(dst, src, size);
}

size_t fwrite( const void* buffer, size_t size, size_t count, HuFILE* stream ) {
    s32 pad0;
    s32 sp44[2];
    s32 pad1;
    s32 sp2C;
    u32 sp38;
    s32 sp34;

    // empty inline call to reserve stub
    filesetstat();

    if (!(stream->unk10 & 2)) {
        return 0;
    }

    D_8009E07C = 1;
    sp2C = size * count;
    dmaWrite(stream->unk4 + stream->pos, sp2C, buffer);
    stream->unkC = (stream->pos += sp2C);

    sp44[0] = stream->unk4 - D_800EE2C0[0];
    sp44[1] = stream->unkC;

    sp38 = stream->unk0;
    sp34 = -1;

    while (D_8008C784 != 0) {
        osYieldThread();
    }
    D_8008C784 = 1;
    if (D_8009E080 != (sp38 >> 8)) {
        sp34 = D_8009E080;
        filereadfat((sp38 >> 8));
    }
    D_800EE2C8[sp38 & 0xFF].unk0 = sp44[0] - D_800EE2C0[0];
    D_800EE2C8[sp38 & 0xFF].unk4 = sp44[1];
    dmaWrite(((u32)D_8009E080 * 8) + D_8009E070, 0x800, (u32)&D_800EE2C8);
    if (sp34 != -1) {
        filereadfat(sp34);
    }
    D_8008C784 = 0;
    return sp2C;
}

static __inline void filesetstat() {

}

static void filereadfat(s32 saved_reg_s0) {
    if (saved_reg_s0 != D_8009E080) {
        if (D_8009E07C != 0) {
            filefatflush();
        }
        D_8009E07C = 0;
        D_8009E080 = saved_reg_s0;
        filewritecurrent(saved_reg_s0);
    }
}

static __inline void filefatflush(void) {
    dmaWrite((D_8009E080 * 8) + D_8009E070, 0x800, &D_800EE2C8[0]);
}

static __inline void filewritecurrent(s32 arg0) {
    dmaRead(D_800EE2C8, 0x800, D_8009E070 + (arg0 << 0xB));
}

// It returns zero if successful, or else it returns a non-zero value.
int fseek(HuFILE *pointer, long int offset, int position) {
    switch (position) {
        default:
            break;
        case SEEK_SET:
            if (offset < 0) {
                return -1;
            }
            pointer->pos = offset;
            break;
        case SEEK_CUR:
            if ((offset < 0) && (pointer->pos < (u32) -offset)) {
                return -1;
            }
            pointer->pos = (u32) (pointer->pos + offset);
            break;
        case SEEK_END:
            if ((offset < 0) && (pointer->unkC < (u32) -offset)) {
                return -1;
            }
            pointer->pos = (pointer->unkC + (u32) offset);
            break;
    }
    if (pointer->unkC < (u32) pointer->pos) {
        return -1;
    }
    return 0;
}

long ftell(HuFILE* stream) {
    return stream->pos;
}
