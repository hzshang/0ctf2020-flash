#ifndef TRAPS_H
#define TRAPS_H
#include "stdint.h"
typedef struct {
  // general registers
  uint at;
  uint v0;
  uint v1;
  uint a0;
  uint a1;
  uint a2;
  uint a3;
  uint t0;
  uint t1;
  uint t2;
  uint t3;
  uint t4;
  uint t5;
  uint t6;
  uint t7;
  uint s0;
  uint s1;
  uint s2;
  uint s3;
  uint s4;
  uint s5;
  uint s6;
  uint s7;
  uint t8;
  uint t9;
  uint k0;
  uint k1;
  uint gp;
  uint sp;
  uint fp;
  uint ra;

  uint epc;
  uint error_epc;
  uint status;
} trapframe;
#endif