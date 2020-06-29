#include "malloc.h"
#include "bios.h"
#include "stdint.h"

void* malloc(int size){
    void* iter = (void*)BIOS_HEAP;
    if(!*(uint32_t*)iter || *(uint32_t*)iter + size >= 0x8000)
        *(uint32_t*)iter = 4;
    void* ret = *(uint32_t*)iter + iter;
    *(uint32_t*)iter += size;
    return ret;
}