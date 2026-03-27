#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

// usage:
// bm64tsadecompress <input_file> <output_file>

enum OverlayGroup {
    OVL_INVALID = -1,
    OVL_GROUP_ACTOR,   /* 0x40000000 */
    OVL_GROUP_ITEM,    /* 0x41000000 */
    OVL_GROUP_BLAST,   /* 0x42000000 */
    OVL_GROUP_ENEMY,   /* 0x43000000 */
    OVL_GROUP_COLL,    /* 0x44000000 */
    OVL_GROUP_MOBJ,    /* 0x45000000 */
    OVL_GROUP_GAMEMODE /* 0x60000000 */
};

struct FileEntry {
    enum OverlayGroup group;
    const char *folder;
    const char *filename;
    int isHeader;
};

struct FileEntry gFiles[] = {
    // actor
    { OVL_GROUP_ACTOR, "actor", "info",       true },
    { OVL_GROUP_ACTOR, "actor", "main.bin",   false },
    // item
    { OVL_GROUP_ITEM, "item", "info",        true },
    { OVL_GROUP_ITEM, "item", "main.bin",    false },
    // coll
    { OVL_GROUP_COLL, "coll", "info",        true },
    { OVL_GROUP_COLL, "coll", "main.bin",    false },
    // mobj
    { OVL_GROUP_MOBJ, "mobj", "info",        true },
    { OVL_GROUP_MOBJ, "mobj", "main.bin",    false },
    // blast
    { OVL_GROUP_BLAST, "blast", "info",       true },
    { OVL_GROUP_BLAST, "blast", "flame.bin",  false },
    { OVL_GROUP_BLAST, "blast", "ice.bin",    false },
    { OVL_GROUP_BLAST, "blast", "wind.bin",   false },
    { OVL_GROUP_BLAST, "blast", "ground.bin", false },
    { OVL_GROUP_BLAST, "blast", "elec.bin",   false },
    { OVL_GROUP_BLAST, "blast", "light.bin",  false },
    { OVL_GROUP_BLAST, "blast", "dark.bin",   false },
    // enemy
    { OVL_GROUP_ENEMY, "enemy", "info",       true },
    { OVL_GROUP_ENEMY, "enemy", "world1.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world2.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world3.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world4.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world5.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world6.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world7.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "world8.bin", false },
    { OVL_GROUP_ENEMY, "enemy", "battle.bin", false },
    // boss
    { OVL_GROUP_ENEMY, "boss", "info",        true },
    { OVL_GROUP_ENEMY, "boss", "demon.bin",   false },
    { OVL_GROUP_ENEMY, "boss", "devil.bin",   false },
    { OVL_GROUP_ENEMY, "boss", "angel.bin",   false },
    // menu
    { OVL_GROUP_GAMEMODE, "menu", "info",        true },
    { OVL_GROUP_GAMEMODE, "menu", "card.bin",    false },
    { OVL_GROUP_GAMEMODE, "menu", "title.bin",   false },
    { OVL_GROUP_GAMEMODE, "menu", "file.bin",    false },
    { OVL_GROUP_GAMEMODE, "menu", "battle.bin",  false },
    { OVL_GROUP_GAMEMODE, "menu", "custom.bin",  false },
    { OVL_GROUP_GAMEMODE, "menu", "stage.bin",   false },
    // demo
    { OVL_GROUP_GAMEMODE, "demo", "info",        true },
    { OVL_GROUP_GAMEMODE, "demo", "story.bin",   false },
    { OVL_GROUP_GAMEMODE, "demo", "guide.bin",   false },
    // stage
    { OVL_GROUP_GAMEMODE, "stage", "info",       true },
    { OVL_GROUP_GAMEMODE, "stage", "world1.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world2.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world3.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world4.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world5.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world6.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world7.bin", false },
    { OVL_GROUP_GAMEMODE, "stage", "world8.bin", false },
    {},
};

#define ALIGN16(x)  ((((x) + 15) >> 4) << 4)

// start address of the overlay entries
static uint32_t start_addr = 0x1000000;

const char *get_overlay_ram_id(enum OverlayGroup group) {
    switch(group) {
        default:
        case OVL_INVALID:
            printf("ERROR: Invalid overlay group.\n");
            exit(1);
            return NULL; // shuts up warning
        case OVL_GROUP_ACTOR:
            return "actor";
        case OVL_GROUP_ITEM:
            return "item";
        case OVL_GROUP_BLAST:
            return "blast";
        case OVL_GROUP_ENEMY:
            return "enemy";
        case OVL_GROUP_COLL:
            return "coll";
        case OVL_GROUP_MOBJ:
            return "mobj";
        case OVL_GROUP_GAMEMODE:
            return "gamemode";
    }
}

uint32_t get_overlay_vram(enum OverlayGroup group) {
    switch(group) {
        default:
        case OVL_INVALID:
            printf("ERROR: Invalid overlay group.\n");
            exit(1);
            return 0;
        case OVL_GROUP_ACTOR:
            return 0x40000000;
        case OVL_GROUP_ITEM:
            return 0x41000000;
        case OVL_GROUP_BLAST:
            return 0x42000000;
        case OVL_GROUP_ENEMY:
            return 0x43000000;
        case OVL_GROUP_COLL:
            return 0x44000000;
        case OVL_GROUP_MOBJ:
            return 0x45000000;
        case OVL_GROUP_GAMEMODE:
            return 0x60000000;
    }
}

#define ALIGN16(x)  ((((x) + 15) >> 4) << 4)

size_t get_asset_size(const char *filepath) {
    int unalign_size;

    FILE *in = fopen(filepath, "rb");
    if (in == NULL) {
        printf("ERROR: Failed to open asset file at %s\n", filepath);
        exit(1);
    }

    // get size of asset
    fseek(in, 0L, SEEK_END);
    unalign_size = ftell(in);
    fseek(in, 0L, SEEK_SET);

    return unalign_size;
}

int generate_overlay_entry(struct FileEntry *entry, char *assets_path) {
    // print header.
    if (entry->isHeader) {
        printf("  - name: ovl_%s_header\n", entry->folder);
        printf("    exclusive_ram_id: overlay_header\n");
        printf("    ld_use_symbolic_vram_addresses: false\n");
        printf("    type: bin\n");
        printf("    bss_size: 0x0 # unknown\n");

        // now we need the aligned size of the asset, for we are incrementing the addr by this after we are done.
        char *filepath = (char*)malloc(0x30);
        sprintf(filepath, "%s/%s/%s", assets_path, entry->folder, entry->filename);
        int unaligned_size = get_asset_size(filepath);
        int aligned_size = ALIGN16(unaligned_size);

        printf("    start: 0x%7X\n", start_addr);
        printf("    vram: 0x%08X\n", 0x80008000);
        printf("    symbol_name_format: ovl_%s_header_$VRAM\n", entry->folder);
        printf("    subsegments:\n");
        printf("    - [0x%7X, bin]\n", start_addr);

        // inc ptr
        start_addr += aligned_size;
    }
    else {
        // we are not a string for a header. Remove ".bin" from the string before using it.
        int len = strlen(entry->filename);
        int i;
        char *newstr = (char *)malloc(len-4);
        for(i = 0; i < len-4; i++) {
            newstr[i] = entry->filename[i];
        }
        newstr[i] = '\0';

        printf("  - name: ovl_%s_%s\n", entry->folder, newstr);
        printf("    exclusive_ram_id: %s\n", get_overlay_ram_id(entry->group));
        printf("    ld_use_symbolic_vram_addresses: false\n");
        printf("    type: code\n");
        printf("    dir: overlays/%s/%s\n", entry->folder, newstr);
        printf("    bss_size: 0x0 # unknown\n");

        // now we need the aligned size of the asset, for we are incrementing the addr by this after we are done.
        char *filepath = (char*)malloc(0x30);
        sprintf(filepath, "%s/%s/%s", assets_path, entry->folder, entry->filename);
        int unaligned_size = get_asset_size(filepath);

        unaligned_size -= 0x4; // account for removed size field

        int aligned_size = ALIGN16(unaligned_size);

        printf("    start: 0x%7X\n", start_addr);
        printf("    vram: 0x%08X\n", get_overlay_vram(entry->group)); // set VRAM
        printf("    symbol_name_format: ovl_%s_%s_$VRAM\n", entry->folder, newstr);
        printf("    subsegments:\n");
        printf("    - [0x%7X, asm, exec]\n",  start_addr);
        printf("    - [0x%7X, asm, exvec]\n", start_addr + 0x10);
        printf("    - [0x%7X, asm]\n",        start_addr + 0xF50);

        // inc ptr
        start_addr += aligned_size;
    }

    printf("\n");

    return 0;
}

int main(int argc, char *argv[]) {
    int sz;

    if (argc != 2) {
        printf("usage: bm64tsa_gen_vaddrs <assets_path>\n");
        return 1;
    }

    char *assets_path = argv[1];

    printf("Generating overlay entries...\n");

    int i = 0;

    // hack
    while (gFiles[i].folder) {
        generate_overlay_entry(&gFiles[i], assets_path);
        i++;
    }

    printf("Done!\n");
    return 0;
}
