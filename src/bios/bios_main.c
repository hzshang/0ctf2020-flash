#include "bios.h"
#include "stdint.h"
#include "rbtree.h"
#include "fram.h"

extern BR_Tree tree;
void intr_rebase_init(){
    
    init_tree(&tree);
    uint32_t* iter = (uint32_t*)BIOS_OBF_DATA;
    while(*iter){
        insert_key(&tree,iter);
        iter += 2;
    }
}

void intr_handler_from_kernel(trapframe* tf){
    if(!tree.root)
        intr_rebase_init();
    uint32_t a = tf->epc;
    uint32_t* ptr = find_key(&tree,&a);
    if(!ptr)
        return;
    tf->epc = ptr[1];
}

void bios_main(){
    uint32_t* src = (uint32_t*)KERNEL_SRC_BASE;
    uint32_t* dst = (uint32_t*)KERNEL_BASE;
    for(int i=0;i<KERNEL_SIZE/4;i++){
        *dst++ = *src++;
    }
    ((void (*)())KERNEL_ENTRY)();
}
