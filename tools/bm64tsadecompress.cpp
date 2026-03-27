#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

// usage:
// bm64tsadecompress <input_file> <output_file>

struct FileEntry {
    const char *folder;
    const char *filename;
    int isHeader;
};

struct FileEntry gFiles[] = {
    // actor
    { "actor", "info",       true },
    { "actor", "main.bin",   false },
    // item
    { "item", "info",        true },
    { "item", "main.bin",    false },
    // coll
    { "coll", "info",        true },
    { "coll", "main.bin",    false },
    // mobj
    { "mobj", "info",        true },
    { "mobj", "main.bin",    false },
    // blast
    { "blast", "info",       true },
    { "blast", "flame.bin",  false },
    { "blast", "ice.bin",    false },
    { "blast", "wind.bin",   false },
    { "blast", "ground.bin", false },
    { "blast", "elec.bin",   false },
    { "blast", "light.bin",  false },
    { "blast", "dark.bin",   false },
    // enemy
    { "enemy", "info",       true },
    { "enemy", "world1.bin", false },
    { "enemy", "world2.bin", false },
    { "enemy", "world3.bin", false },
    { "enemy", "world4.bin", false },
    { "enemy", "world5.bin", false },
    { "enemy", "world6.bin", false },
    { "enemy", "world7.bin", false },
    { "enemy", "world8.bin", false },
    { "enemy", "battle.bin", false },
    // boss
    { "boss", "info",        true },
    { "boss", "demon.bin",   false },
    { "boss", "devil.bin",   false },
    { "boss", "angel.bin",   false },
    // menu
    { "menu", "info",        true },
    { "menu", "card.bin",    false },
    { "menu", "title.bin",   false },
    { "menu", "file.bin",    false },
    { "menu", "battle.bin",  false },
    { "menu", "custom.bin",  false },
    { "menu", "stage.bin",   false },
    // demo
    { "demo", "info",        true },
    { "demo", "story.bin",   false },
    { "demo", "guide.bin",   false },
    // stage
    { "stage", "info",       true },
    { "stage", "world1.bin", false },
    { "stage", "world2.bin", false },
    { "stage", "world3.bin", false },
    { "stage", "world4.bin", false },
    { "stage", "world5.bin", false },
    { "stage", "world6.bin", false },
    { "stage", "world7.bin", false },
    { "stage", "world8.bin", false },
    {},
};

static uint8_t *rom_data;

#define ALIGN16(x)  ((((x) + 15) >> 4) << 4)

int append_file_entry_to_file(FILE *out, struct FileEntry *entry, char *assets_path) {
    char *full_path = (char*)malloc(0x30);
    int unalign_size;
    int align_size;

    // buffer the file path to the string.
    sprintf(full_path, "%s/%s/%s", assets_path, entry->folder, entry->filename);

    FILE *in = fopen(full_path, "rb");
    if (in == NULL) {
        printf("ERROR: Failed to open asset file at %s\n", full_path);
        exit(1);
    }

    // get size of asset
    fseek(in, 0L, SEEK_END);
    unalign_size = ftell(in);
    fseek(in, 0L, SEEK_SET);

    // if we not handling a header, seek 4 bytes into the file to skip the size.
    if (!entry->isHeader) {
        fseek(in, 4, SEEK_SET);
        unalign_size -= 0x4; // we need to subtract 4 as we were counting the size of the bin besides the size byte at the start.
    }

    align_size = ALIGN16(unalign_size);

    // by aligning the size, we can just buffer the unaligned file into the contents and it will already be padded.
    uint8_t *out_buf = (uint8_t*)malloc(align_size);

    if (out_buf == NULL) {
        printf("ERROR: Unable to allocate malloc buffer for asset file.\n");
        exit(1);
    }

    // clear the buffer.
    memset(out_buf, 0, align_size);

    // read the asset file in
    fread(out_buf, 1, unalign_size, in);
    fwrite(out_buf, 1, align_size, out);

    return 0;
}

int main(int argc, char *argv[]) {
    int sz;

    if (argc != 4) {
        printf("usage: bm64tsadecompress <input_file> <output_file> <assets_path>\n");
        return 1;
    }

    char *filepath = argv[1];
    char *filepath_out = argv[2];
    char *assets_path = argv[3];

    // open the ROM input file.
    FILE *rom_in_f = fopen(filepath, "rb");

    // get size of ROM
    fseek(rom_in_f, 0L, SEEK_END);
    sz = ftell(rom_in_f);
    fseek(rom_in_f, 0L, SEEK_SET);

    rom_data = (uint8_t*)malloc(sz);

    // first copy all ROM bytes over to seek the cursor
    int read;
    read = fread(rom_data, 1, sz, rom_in_f);
    if (read != sz) {
        printf("Error reading file (0x%08X != 0x%08X)\n", read, sz);
        return 1;
    }

    // now write rom_in to rom_out.
    FILE *rom_out_f = fopen(filepath_out, "wb");
    if (!rom_in_f) {
        printf("Output file not valid or not found!\n");
        return 1;
    }
    read = fwrite(rom_data, 1, sz, rom_out_f);

    // next detect the country code.
    switch(rom_data[0x3E]) {
        case 0x4A: // 'J'
            printf("J ROM detected\n");
            break;
        case 0x45: // 'E' (US ROM)
            printf("U ROM detected\n");
            break;
        default:
            printf("ERROR: Invalid ROM detected. Check your ROM\n");
            return 1;
    }

    printf("Generating decompressed ROM...\n");

    // append the list of files to the decompressed ROM.
    int i = 0;

    // hack
    while (gFiles[i].folder) {
        append_file_entry_to_file(rom_out_f, &gFiles[i], assets_path);
        i++;
    }

    printf("File written!\n");
    return 0;
}
