#include <stdio.h>
#include <string.h>
#include "registers.h"
#include "machine_types.h"
#include "utilities.h"
#include "instruction.h"
#include "disasm.h"
#include "bof.h"
// a size for the memory (2^16 bytes = 64K)
#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define MEMORY_SIZE_IN_WORDS (MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD)

static int vm_tracing_enabled = 1;

// function to execute instructions and print grp 
void executeInstructions(BOFFILE bf, BOFHeader bh);

static union mem_u {
    byte_type bytes[MEMORY_SIZE_IN_BYTES];
    word_type words[MEMORY_SIZE_IN_WORDS];
    bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory;

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [-p] <program.bof>\n", argv[0]);
        return 1;
    }

    // Check if the -p option is provided
    int print_mode = 0;
    int file_index = 1;
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        print_mode = 1;
        file_index = 2;
    }

    // Open the binary object file
    BOFFILE bf = bof_read_open(argv[file_index]);
    BOFHeader bh = bof_read_header(bf);
    if (bf.fileptr == NULL) {
        fprintf(stderr, "Error: Cannot open binary object file '%s'\n", argv[file_index]);
        return 1;
    }
    
    // Load the instructions into memory
    for (int i = 0; i < bh.text_length / 4; i++) {
        memory.instrs[i] = instruction_read(bf);
    }

    // Load the data into memory
    for (int i = 0; i < bh.data_length / 4; i++) {
        memory.words[i + (bh.data_start_address / 4)] = bof_read_word(bf); 
    }
    
    // Prints loaded instructions and data in assembly file format
    if (print_mode == 1) {
        fprintf(stdout,"Addr Instruction\n");
        for (int i = 0; i < bh.text_length; i+= 4) {
            fprintf(stdout,"\t%d %s", i, instruction_assembly_form(memory.instrs[i / 4]));
            newline(stdout);
        }
        
        // for (int i = 0; i < bh.data_length; i+= 4) {
        //     fprintf(stdout,"     %d:  %d\t", i + bh.data_start_address, memory.words[(i + (bh.data_start_address) / 4);
        // }
        bof_close(bf);
        return 0;
    }
    else {
        executeInstructions(bf, bh);
    }
    //disasmInstrs(stdout, bf, bh.text_length / BYTES_PER_WORD);

    // Close the binary object file
    bof_close(bf);

    return 0;
}

void executeInstructions(BOFFILE bf, BOFHeader bh) {
    for (int i = 0; i < bh.text_length / 4; i++) {
        // Fetch current instruction
        bin_instr_t instruction = memory.instrs[i];

        // Perform current instruction on gpr
        doInstruction(stdout, instruction, i / 4);

        if (vm_tracing_enabled == 1) {
            printRegister(stdout, instruction, i * 4);
        }
    }
}
