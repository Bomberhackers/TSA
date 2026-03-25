#ifndef _FILE_H_
#define _FILE_H_

typedef struct HuFILE {
    /* 0x00 */ long unk0;
    /* 0x04 */ u8 *unk4;
    /* 0x08 */ unsigned long pos;
    /* 0x0C */ long unkC;
    /* 0x10 */ long unk10;
} HuFILE;

struct UnkStruct800EE2C8 {
    /* 0x00 */ long unk0;
    /* 0x00 */ long unk4;
};

extern struct UnkStruct800EE2C8 D_800EE2C8[];
extern HuFILE D_800EEAC8[4];

#define SEEK_SET 0 // It denotes the end of the file.
#define SEEK_CUR 1 // It denotes starting of the file.
#define SEEK_END 2 // It denotes the file pointer's current position.

void finit(s32 arg0);
int fsize(HuFILE *stream);
HuFILE *fopen(u32 arg0, const char *mode);
u32 fcreate(void);
void fclose(HuFILE* stream);
int fgetc(HuFILE* stream);
size_t fread(u8 *buffer, size_t size, size_t count, HuFILE *stream);
void readromtobuff(HuFILE* stream, s32 pos);
int fseek(HuFILE *pointer, long int offset, int position);
long ftell(HuFILE* stream);

#endif // _FILE_H_
