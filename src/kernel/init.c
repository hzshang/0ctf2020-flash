#include "libc.h"
#include "bios.h"

typedef struct {
    int16_t regs[2];
    uint32_t mul;
    uint16_t* pc;
    uint16_t* base;
    uint16_t* top;
    uint16_t* sp;
    uint16_t* in;
} VM;

extern VM vm;
uint16_t vm_pop(){
    if(vm.sp == vm.base)
        return 0;
    vm.sp--;
    return *vm.sp;
}

void vm_push(uint16_t a){
    if(vm.sp <= vm.top){ // off by one
        *vm.sp = a;
        vm.sp++;
    }
}

void vm_run(){

    int stop = 0;
    while(!stop){
        uint16_t op = *vm.pc;
        vm.pc++;
        switch(op){
            case 0:// +
            {
#ifdef DEBUG
                printf("add\n");
#endif
                uint16_t a = vm_pop();
                uint16_t b = vm_pop();
                vm_push(a+b);
            }
                break;
            case 1:// -
            {
#ifdef DEBUG
                printf("sub\n");
#endif
                uint16_t a = vm_pop();
                uint16_t b = vm_pop();
                vm_push(a-b);
            }
                break;
            case 2:// *
            {
#ifdef DEBUG
                printf("mul\n");
#endif
                uint16_t a = vm_pop();
                uint16_t b = vm_pop();
                vm.mul = a*b;
            }
                break;
            case 3:// %
            {
#ifdef DEBUG
                printf("mod\n");
#endif
                uint16_t a = vm_pop();
                vm_push(vm.mul%a);
            }
                break;
            case 4:// cmp less
            {
#ifdef DEBUG
                printf("cmp less\n");
#endif
                uint16_t a = vm_pop();
                uint16_t b = vm_pop();
                vm_push(a<b);
            }
                break;
            case 5:// cmp equal
            {
#ifdef DEBUG
                printf("cmp equal\n");
#endif
                uint16_t a = vm_pop();
                uint16_t b = vm_pop();
                vm_push(a==b);
            }
                break;
            case 6:// jmp if true
            {
#ifdef DEBUG
                printf("jmp if true\n");
#endif
                int jmp_less = (int)(*(int16_t*)vm.pc);
                vm.pc ++;
                int m = vm_pop();
                if(m)
                    vm.pc += jmp_less/2;
            }
                break;
            case 7:// jmp if false
            {
#ifdef DEBUG
                printf("jmp if false\n");
#endif
                int jmp_less = (int)(*(int16_t*)vm.pc);
                vm.pc ++;
                int m = vm_pop();
                if(!m)
                    vm.pc += jmp_less/2;
            }
                break;
            case 8:// push from in
            {
#ifdef DEBUG
                printf("push in\n");
#endif
                vm_push(*vm.in);
                vm.in++;
            }
                break;
            case 9:// push from pc
            {
                uint16_t a = *(uint16_t*)vm.pc;
                vm.pc ++;
#ifdef DEBUG
                printf("push %d\n",a);
#endif
                vm_push(a);
            }
                break;
            case 10:// push from reg
            {
#ifdef DEBUG
                printf("push regs[0]\n");
#endif
                vm_push(vm.regs[0]);
            }
                break;
            case 11:// pop to reg
            {
#ifdef DEBUG
                printf("pop regs[0]\n");
#endif
                vm.regs[0]=vm_pop();
            }
                break;
            case 12:// drop
            {
#ifdef DEBUG
                printf("pop\n");
#endif
                vm_pop();
            }
                break;
            case 13:// stop
            {
#ifdef DEBUG
                printf("hlt\n");
#endif
                stop = 1;
            }
                break;
            default:
            {
#ifdef DEBUG
                printf("unknown opcode!\n");
                while(1);
#endif
            }
            break;
        }
    }
}
extern uint16_t vm_code;
void vm_init(char* in,int length){
    vm.in = (uint16_t*)in;
    vm.regs[0] = length;
    vm.pc = &vm_code;
}

void kernel_main(){
    printf("Welcome to Flag Machine\n");
    printf("Give me flag: ");
    char* input = malloc(0x800);
    int length = readline(input,0x800);
    input[--length] == '\x00';
    if(!strncmp(input,"flag{",5) && input[length-1] == '}'){
        length -= 6;
        memmove(input,&input[5],length);
        input[length] = '\x00';
        vm_init(input,(length+1)/2);
        vm_run();
        if(vm.regs[0] == 0){
            printf("Correct!\n");
            // while(1);
            return;
        }else{
            goto wrong;
        }
    }
wrong:
    printf("Try again!\n");
#ifdef DEBUG
    // while(1);
#endif
    return;
    // return start.......
}