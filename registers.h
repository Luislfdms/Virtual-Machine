#ifndef  _REGISTERS
#define  _REGISTERS

#include <stdio.h>
#include "machine_types.h"
#include "bof.h"
#include "regname.h"
#include "instruction.h"

//Register tags to appropriate register spot
typedef enum {
    $0 = 0, $at = 1, $v0 = 2, $v1 = 3, $a0 = 4, $a1 = 5, $a2 = 6, $a3 = 7,
    $t0 = 8, $t1 = 9, $t2 = 10, $t3 = 11, $t4 = 12, $t5 = 13, $t6 = 14, $t7 = 15,
    $s0 = 16, $s1 = 17, $s2 = 18, $s3 = 19, $s4 = 20, $s5 = 21, $s6 =22, $s7 =23,
    $t8 = 24, $t9 =25, $k0 =26, $k1 = 27, $gp = 28, $sp = 29, $fp = 30, $ra = 31
} gpr_tag;

typedef enum {
    PC = 0, HI = 1, LO = 2
} spr_tag;

//prints the General Purpose Register at the current instruction
extern void printRegister();

//prints the GPR at all instructions
extern void printRegisters(FILE *out, BOFFILE bf, int length);
#endif

//Does the instruction
extern void doInstruction(FILE * out, bin_instr_t bi, unsigned int i);