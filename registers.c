/*
General Purpose Register concept
instructions mostly work with registers
need to figure out how to integrate memory with registers
*/

#include <stdio.h>
#include <stdlib.h>
#include "machine_types.h"
#include "bof.h"
#include "regname.h"
#include "utilities.h"
#include "registers.h"
#include "instruction.h"

static int gpr[NUM_REGISTERS] = {0};
static int spr[3] = {0, 0, 0};

//prints the General Purpose Register at the current instruction
void printRegister(FILE * out, bin_instr_t bi, int i) {
    spr[PC] = i;
    fprintf(out, "      PC: %d\n", spr[PC]);
    fprintf(out, "GPR[$0 ]: %d GPR[$at]: %d GPR[$v0]: %d GPR[$v1]: %d GPR[$a0]: %d GPR[$a1]: %d\n"
    ,gpr[$0], gpr[$at], gpr[$v0], gpr[$v1], gpr[$a0], gpr[$a1]);
    fprintf(out, "GPR[$a2]: %d GPR[$a3]: %d GPR[$t0]: %d GPR[$t1]: %d GPR[$t2]: %d GPR[$t3]: %d\n"
    ,gpr[$a2], gpr[$a3], gpr[$t0], gpr[$t1], gpr[$t2], gpr[$t3]);
    fprintf(out, "GPR[$t4]: %d GPR[$t5]: %d GPR[$t6]: %d GPR[$t7]: %d GPR[$s0]: %d GPR[$s1]: %d\n"
    ,gpr[$t4], gpr[$t5], gpr[$t6], gpr[$t7], gpr[$s0], gpr[$s1]);
    fprintf(out, "GPR[$s2]: %d GPR[$s3]: %d GPR[$s4]: %d GPR[$s5]: %d GPR[$s6]: %d GPR[$s7]: %d\n"
    ,gpr[$s2], gpr[$s3], gpr[$s4], gpr[$s5], gpr[$s6], gpr[$s7]);
    fprintf(out, "GPR[$t8]: %d GPR[$t9]: %d GPR[$k0]: %d GPR[$k1]: %d GPR[$gp]: %d GPR[$sp]: %d\n"
    ,gpr[$t8], gpr[$t9], gpr[$k0], gpr[$k1], gpr[$gp], gpr[$sp]);
    fprintf(out, "GPR[$fp]: %d GPR[$ra]: %d\n", gpr[$fp], gpr[$ra]);
    fprintf(out, "    1024: 0 ...\n");
    fprintf(out, "    4096: 0 ...\n");
    fprintf(out, "==> addr: %i %s\n", spr[PC], instruction_assembly_form(bi));
}

//prints the GPR at all instructions
void printRegisters(FILE *out, BOFFILE bf, int length)
{
    for (int i = 0; i < length; i++) {
        bin_instr_t bi = instruction_read(bf);
	    printRegister(out, bi, i * BYTES_PER_WORD);
        doInstruction(out, bi, i / 4);
    }
}

void doInstruction(FILE * out, bin_instr_t instr, unsigned int vm_tracing_enabled) {
    fprintf(out, "%s\n", instruction_assembly_form(instr));

    instr_type it = instruction_type(instr);
	// Debugging
	//printf("Syscall code: %d\n", instr.syscall.code);
	//printf("Instruction type: %d\n", it);

    switch (it) {    
    	case reg_instr_type:
			switch (instr.reg.func) {
				case ADD_F: 
					gpr[instr.reg.rs] = instr.reg.rs;
			        gpr[instr.reg.rt] = instr.reg.rt;
			        gpr[instr.reg.rd] = instr.reg.rs + instr.reg.rt;
		    		break;
	    		case SUB_F: 
	        		gpr[instr.reg.rs] = instr.reg.rs;
	        		gpr[instr.reg.rt] = instr.reg.rt;
	        		gpr[instr.reg.rd] = instr.reg.rs - instr.reg.rt;
		    		break;
	    		case AND_F: 
	        		gpr[instr.reg.rs] = instr.reg.rs;
				    gpr[instr.reg.rt] = instr.reg.rt;
			        gpr[instr.reg.rd] = instr.reg.rs & instr.reg.rt;
					break;
				case BOR_F: 
			        gpr[instr.reg.rs] = instr.reg.rs;
			        gpr[instr.reg.rt] = instr.reg.rt;
			        gpr[instr.reg.rd] = instr.reg.rs | instr.reg.rt;
			        break;
			    case NOR_F: 
			        gpr[instr.reg.rs] = instr.reg.rs;
			        gpr[instr.reg.rt] = instr.reg.rt;
			        gpr[instr.reg.rd] = !(instr.reg.rs | instr.reg.rt);
				    break;
			    case XOR_F:
			        gpr[instr.reg.rs] = instr.reg.rs;
			        gpr[instr.reg.rt] = instr.reg.rt;
			        gpr[instr.reg.rd] = instr.reg.rs ^ instr.reg.rt;
				    break;
				case MUL_F: 
			        gpr[instr.reg.rs] = instr.reg.rs;
			        gpr[instr.reg.rt] = instr.reg.rt;
			        spr[HI] = instr.reg.rs * instr.reg.rt;
			        spr[LO] = instr.reg.rs * instr.reg.rt;
					break;
			    case DIV_F:
			        gpr[instr.reg.rs] = instr.reg.rs;
			        gpr[instr.reg.rt] = instr.reg.rt;
			        spr[HI] = instr.reg.rs % instr.reg.rt;
			        spr[LO] = instr.reg.rs / instr.reg.rt;
				    break;
				case MFHI_F: 
			        gpr[instr.reg.rd] = spr[HI];
			        break;
			    case MFLO_F:
			        gpr[instr.reg.rd] = spr[LO];
				    break;
				case SLL_F: case SRL_F:
				    break;
				case JR_F:
				    break;
				default:
					bail_with_error("Unknown register instruction function (%d)!",
							instr.reg.func);
					break;
			}
		case syscall_instr_type:
			switch(instr.syscall.code) {
				case exit_sc:
					exit(EXIT_SUCCESS);
					break;
				case print_str_sc:
					break;
				case print_char_sc:
					break;
				case read_char_sc:
					break;
				case start_tracing_sc:
					vm_tracing_enabled = 1;
					break;
				case stop_tracing_sc:
					vm_tracing_enabled = 0;
					break;
				default:
					bail_with_error("Unknown system call code (%d)!",
							instr.syscall.code);
					break;
				}
			break;
    	case immed_instr_type:
			switch (instr.immed.op) {
				case ADDI_O:
				    gpr[instr.immed.rs] = instr.immed.rs;
			        gpr[instr.immed.rt] = instr.immed.rs + (short int) instr.immed.immed;
				    break;
				case ANDI_O: 
			        gpr[instr.immed.rs] = instr.immed.rs;
			        gpr[instr.immed.rt] = instr.immed.rs & instr.immed.immed;
			        break;
			    case BORI_O: 
			        gpr[instr.immed.rs] = instr.immed.rs;
			        gpr[instr.immed.rt] = instr.immed.rs | instr.immed.immed;
			        break;
			    case XORI_O:
			        gpr[instr.immed.rs] = instr.immed.rs;
			        gpr[instr.immed.rt] = instr.immed.rs ^ instr.immed.immed;
			        break;
				case BEQ_O: case BNE_O:
				    break;
				case BGEZ_O: case BGTZ_O: case BLEZ_O: case BLTZ_O:
				    break;
				case LBU_O: case LW_O: case SB_O: case SW_O:
				    break;
				default:
				    bail_with_error("Unknown immediate instruction opcode (%d)!",
						    instr.immed.op);
				    break;
			}
			break;
    	case jump_instr_type:
			switch (instr.jump.op) {
				case JMP_O: 
			        //spr[PC] =
			        break;
			    case JAL_O:
			        //spr[PC] =
				    break;
				default:
				    bail_with_error("Unknown jump instruction opcode (%d)!",
						    instr.jump.op);
				    break;
				}
			break;
	    default:
			//printf("here1"); 
			bail_with_error("Unknown instruction type (%d) in instruction_assembly_form!",
					it);
		break;
	}
    
    // switch (it) {
    //     case immed_instr_type:
    //         printf("GPR[%s ]: %d, GPR[%s ]: %d, : %hx\n",
    //         regname_get(instr.immed.rs), instr.immed.rs, regname_get(instr.immed.rt), instr.immed.rt, instr.immed.immed);
    //         break;
    //     case reg_instr_type:
    //         printf("GPR[%s ]: %d, GPR[%s ]: %d, GPR[%s ]: %d\n",
    //         regname_get(instr.reg.rs), instr.reg.rs, regname_get(instr.reg.rt), instr.reg.rt, regname_get(instr.reg.rd), instr.reg.rd);
    //         break;
    //     case jump_instr_type:
    //         break;
    //     case syscall_instr_type:
    //         break;
    //     default:
	// 		//printf("here2");
    //         bail_with_error("Unknown instruction type (%d) in instruction_assembly_form!",
	// 		it);
    // }
    
}
