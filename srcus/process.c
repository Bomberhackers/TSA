#include <ultra64.h>

// memory externs
extern s32 GetMemoryAllocSize(s32);
extern void InitMemory(void *, s32);
extern void *AllocMemory(void *, s32);
extern void *malloc(s32);
extern s32 getsp();

// process.c

#define NULL (void*)0

typedef struct jump_buf
{
    u32* sp;
    void *func;
    u32 regs[21];
} jmp_buf;

typedef struct HeapNode {
    /* 0x00 */ s32 size;
    /* 0x04 */ u8 heap_constant;
    /* 0x05 */ u8 active;
    /* 0x08 */ struct HeapNode* prev;
    /* 0x0C */ struct HeapNode* next;
} HeapNode;

typedef void (*process_func)();

#define EXEC_PROCESS_DEFAULT 0
#define EXEC_PROCESS_SLEEPING 1
#define EXEC_PROCESS_WATCH 2
#define EXEC_PROCESS_DEAD 3
#define EXEC_PROCESS_UNK4 4 // paused?

typedef struct Process {
    /*0x00*/ struct Process *next;
    /*0x04*/ struct Process *youngest_child;
    /*0x08*/ struct Process *oldest_child;
    /*0x0C*/ struct Process *relative;
    /*0x10*/ struct Process *parent_oldest_child;
    /*0x14*/ struct Process *new_process;
    /*0x18*/ void *heap;
    /*0x1C*/ u32 stat;
    /*0x20*/ process_func func;
    /*0x24*/ u32 exec_mode;
    /*0x28*/ u16 priority;
    char filler2A[0x2];
    /*0x2C*/ s32 sleep_time;
    /*0x30*/ void *base_sp;
    /*0x34*/ jmp_buf prc_jump;
    /*0x90*/ process_func destructor;
} Process; // size:0x94

extern void *process_jmp_buf;
extern Process *top_process;
extern Process *current_process;
extern u16 process_count;

extern void *D_800A59C8;

// FUNCTIONS

void InitProcess(void);
Process *CreateChildProcess(process_func func, u16 priority, s32 stack_size, s32 extra_data_size, Process *root);
Process* CreateProcess(process_func func, u16 priority, u32 stack_size, s32 extra_data_size);
static void LinkProcess(Process** root, Process* process); // INLINE
void LinkChildProcess(Process* root, Process *child);
void WatchChildProcess(void);
void EndProcess(void);
static void TerminateProcess(Process *process); // INLINE
static void UnlinkProcess(Process *process); // INLINE
void SleepVProcess(void);
void SleepProcess(s32 time);
void SetProcessDestruct(void *destructor_func);
void CallProcess(s32 time);
void AllocProcessMemory(s32 size);
Process* GetCurrentProcess(void);
void FreeProcessMemory(void *ptr);
void SleepPrioProcess(u16 priority_min, u16 priority_max, s32 time);
void SleepProcessP(Process* process, s32 time);
void KillPrioProcess(u16 arg0, u16 arg1); // REQUIRES O3 ABI
void KillProcess(Process *process);
void KillChildProcess(Process* process); // this cannot be static, there is a table that links against this
void UnlinkChildProcess(Process* process);
static s32 SetKillStatusProcess(Process *process);
void WakeupPrioProcess(u16 priority_min, u16 priority_max);
void WakeupProcess(Process* process);
void SetProcessCheck(void);
void CheckProcessStruct(void);
void CheckProcessStackBroken(void);
s32 CheckProcessStack(void);
s32 GetProcessStackR(void);

void InitProcess(void) {
    process_count = 0;
    top_process = 0;
}

// INLINE
static void LinkProcess(Process** root, Process* process) {
    Process* src_process = *root;
    if (src_process != NULL && (src_process->priority >= process->priority))
    {
        while (src_process->next != NULL && src_process->next->priority >= process->priority)
        {
            src_process = src_process->next;
        }

        process->next = src_process->next;
        process->youngest_child = src_process;
        src_process->next = process;
        if (process->next)
        {
            process->next->youngest_child = process;
        }
    }
    else
    {
        process->next = (*root);
        process->youngest_child = NULL;
        *root = process;
        if (src_process != NULL)
        {
            src_process->youngest_child = process;
        }
    }
    process->oldest_child = NULL;
    process->relative = NULL;
}

static void UnlinkProcess(Process *process) {
    if (process->next) {
        process->next->youngest_child = process->youngest_child;
    }

    if (process->youngest_child) {
        process->youngest_child->next = process->next;
    } else {
        top_process = process->next;
    }
}

Process* CreateProcess(process_func func, u16 priority, u32 stack_size, s32 extra_data_size) {
    s32 alloc_size;
    HeapNode *process_heap;
    Process* process;

    if (stack_size == 0) {
        stack_size = 0x1000;
    } else if (stack_size < 0x2000) {
        stack_size = 0x2000;
    }

    alloc_size = GetMemoryAllocSize(sizeof(Process)) + GetMemoryAllocSize(stack_size) + extra_data_size;

    process_heap = (HeapNode *)malloc(alloc_size);
    if (process_heap == NULL) {
        errstop("process : create malloc error\n");
    }
    InitMemory(process_heap, alloc_size);

    process = (Process*)AllocMemory(process_heap, sizeof(Process));
    process->heap = process_heap;
    process->stat = 0;
    process->exec_mode = EXEC_PROCESS_DEFAULT;
    process->priority = priority;
    process->sleep_time = 0;
    process->base_sp = AllocMemory(process_heap, stack_size);
    ((s32*)process->base_sp)[0] = 0xDBDB7272;
    process->prc_jump.func = func;
    process->func = func;
    process->prc_jump.sp = (u32)process->base_sp + stack_size - 8;
    process->destructor = NULL;
    LinkProcess(&top_process, process);
    process_count++;
    return process;
}

void LinkChildProcess(Process* root, Process *child) {
    UnlinkChildProcess(child);

    if (root->oldest_child != NULL) {
        root->oldest_child->new_process = child;
    }
    child->parent_oldest_child = root->oldest_child;
    child->new_process = NULL;
    root->oldest_child = child;
    child->relative = root;
}

void UnlinkChildProcess(Process* process) {
    if (process->relative != NULL) {
        if (process->parent_oldest_child != NULL) {
            process->parent_oldest_child->new_process = process->new_process;
        }
        if (process->new_process != NULL) {
            process->new_process->parent_oldest_child = process->parent_oldest_child;
        } else {
            process->relative->oldest_child = process->parent_oldest_child;
        }
        process->relative = NULL;
    }
}

Process *CreateChildProcess(process_func func, u16 priority, s32 stack_size, s32 extra_data_size, Process *root) {
    Process *proc = CreateProcess(func, priority, stack_size, extra_data_size);

    LinkChildProcess(root, proc);
    return proc;
}

void WatchChildProcess(void) {
    Process* process = GetCurrentProcess();
    if (process->oldest_child) {
        process->exec_mode = EXEC_PROCESS_WATCH;
        if (!setjmp(&process->prc_jump)) {
            longjmp(&process_jmp_buf, 1);
        }
    }
}

Process* GetCurrentProcess(void) {
    return current_process;
}

void KillChildProcess(Process* process) {
    Process* child_process = process->oldest_child;

    for(child_process = process->oldest_child; child_process; child_process = child_process->parent_oldest_child) {
        if (child_process->oldest_child != 0) {
            KillChildProcess(child_process);
        }
        SetKillStatusProcess(child_process);
    }
    process->oldest_child = NULL;
}

static void TerminateProcess(Process *process) {
    if (process->destructor) {
        process->destructor();
    }
    UnlinkProcess(process);
    process_count--;
    longjmp(&process_jmp_buf, 2);
}

void EndProcess(void) {
    Process *process = GetCurrentProcess();

    KillChildProcess(process);
    UnlinkChildProcess(process);
    TerminateProcess(process);
}

void SleepProcess(s32 time) {
    Process* process = GetCurrentProcess();
    int res;
    jmp_buf *jmp;

    if (time != 0 && process->exec_mode != EXEC_PROCESS_DEAD) {
        process->exec_mode = EXEC_PROCESS_SLEEPING;
        process->sleep_time = time;
    }

    jmp = &process->prc_jump;
    res = setjmp(jmp);

    if (!((u32)process->heap & 0x80000000) || !((u32)process->base_sp & 0x80000000) || !((u32)jmp & 0x80000000)) {
        errstop("Process : Structure broken : P 0x%08x F 0x%08x ST 0x%08x HP 0x%08x JP 0x%08x\n", process, process->func, process->base_sp, process->heap, jmp);
    }

    if ((u32*)process->base_sp >= process->prc_jump.sp || ((u32*)process->base_sp)[0] != 0xDBDB7272 || ((u8*)process->heap)[4] != 0xA5) {
        errstop("Process : Heap or stack broken : P 0x%08x F 0x%08x ST 0x%08x SP 0x%08x HP 0x%08x SMG 0x%08x HMG 0x%02x\n", process, process->func, process->base_sp, process->prc_jump.sp, process->heap, ((u32*)process->base_sp)[0], ((u8*)process->heap)[4]);
    }

    if (!res) {
        longjmp(&process_jmp_buf, 1);
    }
}

void SleepVProcess(void) {
    SleepProcess(0);
}

void SetProcessDestruct(void *destructor_func) {
    Process *process = GetCurrentProcess();
    process->destructor = destructor_func;
}

void CallProcess(s32 time) {
    Process* cur_proc_local;
    s32 ret;

    current_process = top_process;
    ret = setjmp(&process_jmp_buf);

    while (1)
    {
        switch (ret)
        {
            case 2:
                free(current_process->heap);
            case 1:
                if (((u8*)current_process->heap)[4] != 0xA5) {
                    errstop("stack overlap error.(process pointer %x)\n", current_process);
                    cur_proc_local = current_process;
                }
                current_process = current_process->next;
                break;
        }

        cur_proc_local = current_process;
        if (cur_proc_local == 0) {
            break;
        }

        switch (cur_proc_local->exec_mode){
            case EXEC_PROCESS_UNK4:
                ret = 1;
                break;
            case EXEC_PROCESS_SLEEPING:
                if (cur_proc_local->sleep_time > 0 && (cur_proc_local->sleep_time -= time) <= 0) {
                    cur_proc_local->sleep_time = 0;
                    cur_proc_local->exec_mode = EXEC_PROCESS_DEFAULT;
                }
                ret = 1;
                break;

            case EXEC_PROCESS_WATCH:
                if (cur_proc_local->oldest_child != 0) {
                    ret = 1;
                }
                else {
                    cur_proc_local->exec_mode = EXEC_PROCESS_DEFAULT;
                    ret = 0;
                }
                break;

            case EXEC_PROCESS_DEAD:
                cur_proc_local->prc_jump.func = EndProcess;

            case EXEC_PROCESS_DEFAULT:
                longjmp(&cur_proc_local->prc_jump, 1);
                break;
        }
    }
}

void AllocProcessMemory(s32 size) {
    Process *process = GetCurrentProcess();

    AllocMemory(process->heap, size);
}

void FreeProcessMemory(void *ptr) {
    FreeMemory(ptr);
}

void SleepPrioProcess(u16 priority_min, u16 priority_max, s32 time) {
    Process *process;

    for(process = top_process; process; process = process->next) {
        if (process->priority >= priority_min && priority_max >= process->priority && time && process->exec_mode != EXEC_PROCESS_DEAD) {
            if (time == -1) {
                process->exec_mode = EXEC_PROCESS_UNK4;
            } else {
                process->exec_mode = EXEC_PROCESS_SLEEPING;
                process->sleep_time = time;
            }
        }
    }
}

void SleepProcessP(Process* process, s32 time) {
    if ((time != 0) && (process->exec_mode != 3)) {
        if (time == -1) {
            process->exec_mode = 4;
            return;
        }
        process->exec_mode = 1;
        process->sleep_time = time;
    }
}

void KillPrioProcess(u16 arg0, u16 arg1) {
    Process* process = top_process;

    for(process = top_process; process; process = process->next) {
        if (process->priority >= arg0 && arg1 >= process->priority) {
            KillChildProcess(process);
            UnlinkChildProcess(process);
            SetKillStatusProcess(process);
        }
    }
}

void KillProcess(Process *process) {
    KillChildProcess(process);
    UnlinkChildProcess(process);
    SetKillStatusProcess(process);
}

static s32 SetKillStatusProcess(Process *process) {
    if (process->exec_mode != 3) {
        WakeupProcess(process);
        process->exec_mode = 3;
        return 0;
    }
    return -1;
}

void WakeupPrioProcess(u16 priority_min, u16 priority_max) {
    Process* process = top_process;

    for(process = top_process; process; process = process->next) {
        if (process->priority >= priority_min && priority_max >= process->priority) {
            WakeupProcess(process);
        }
    }
}

void WakeupProcess(Process* process) {
    if (process->exec_mode == 1) {
        process->sleep_time = 0;
        return;
    }
    if (process->exec_mode == 4) {
        if (process->sleep_time != 0) {
            process->exec_mode = 1;
            return;
        }
        process->exec_mode = 0;
    }
}

void SetProcessCheck(void) {
    memcpy(&D_800A59C8, current_process, sizeof(Process));
}

void CheckProcessStruct(void) {
    u8 *var_s0 = &D_800A59C8;
    u8 *cur_process = (u8*)current_process; // didnt use GetCurrentProcess....
    int i;

    for(i = 0; i < sizeof(Process); i++) {
        if ((*var_s0++) != (*cur_process++)) {
            errstop("Process : Structure compare failed\n");
        }
    }
}

void CheckProcessStackBroken(void) {
    if ((getsp() - (s32)current_process->base_sp) < 5) {
        errstop("Process : Stack broken : P 0x%08x F 0x%08x ST 0x%08x SP 0x%08x HP 0x%08x\n", current_process, current_process->func, current_process->base_sp, current_process->prc_jump.sp, current_process->heap);
    }
}

s32 CheckProcessStack(void) {
    if ((getsp() - (s32)current_process->base_sp) < 5) {
        return 1;
    }
    return 0;
}

s32 GetProcessStackR(void) {
    return getsp() - (s32)current_process->base_sp;
}
