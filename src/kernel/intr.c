#include "fram.h"

void goto_bios(trapframe*);
void reload_timer();
void intr_handler(trapframe* trap){
    // intr to bios
    trap->epc ^= 0xd3abc0de;
    goto_bios(trap);
    trap->epc ^= 0xd3abc0de;
    reload_timer();
}
