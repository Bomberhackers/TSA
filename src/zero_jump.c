#include <ultra64.h>
#include "zerojump_init.h"

extern u32 zerojump_ROM_START;
extern u32 zerojump_ROM_END;

extern u32 boot_code_TEXT_START;
extern u32 boot_code_TEXT_END;

extern u32 boot_code_RODATA_START;
extern u32 boot_code_RODATA_END;

extern u32 zerojump_VRAM[];
extern u32 D_8008C690[];
extern u32 gSecureCallArr[];

extern void exec_zerojmp_func(void);

void zerojumpinit(void) {
    dmaRead((u32)&exec_zerojmp_func, (u32)&zerojump_ROM_END - (u32)&zerojump_ROM_START, (u32)&zerojump_ROM_START);
    // weird if statements needed to consume registers to match.
    if (boot_code_TEXT_END == 0 || boot_code_TEXT_END == 0) {

    }
    if (boot_code_RODATA_START == 0 || boot_code_RODATA_START == 0) {

    }
    if (boot_code_TEXT_END == 0 || boot_code_TEXT_END == 0) {

    }
    osMapTLB(0, 0, 0x10000000, (u32) ((((u32)&exec_zerojmp_func)) - 0x80000000), -1, -1);
    gSecureCallArr[0] = &D_8008C690; // map the secure call manually.
}
