#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bios.h"
#include "8086.h"
#include <stdbool.h>

uint16_t memory[65536];
uint16_t pc;
uint8_t sc;

static enum {
	AX, AH, AL, BX, BH, BL, CX, CH, CL, DX, DH, DL, SR, R14, R15, R16 
} rs;

short int sr[256];

bool ZF = false;
bool SF = false;
bool CF = false;
bool OF = false;

#define ui uint16_t


#define NOP 0x0000
#define LOAD 0x0001
#define ADD 0x0002
#define SUB 0x0003
#define MUL 0x004
#define DIV 0x0005
#define CMP 0x0006
#define AND 0x0007
#define OR 0x0008
#define NOT 0x0009
#define XOR 0x0010
#define INC 0x0011
#define DEC 0x0012
#define SHL 0x0013
#define SHR 0x0014
#define INT 0x0015
#define PUSH 0x0016
#define POP 0x0017
#define RSR 0x0018
#define _8086_ 0xfffe
#define HALT 0xffff



void init() {
    memory[0] = _8086_;
    for(int I = 0; I < 8; I++) reg[I] = 0;
    for(int I = 0; I < 8; I++) sr[I] = 0;
    pc = 0;
    sc = 0;
}

void error(int code) {
    switch(code) {
        case 1: printf("_8086_ not in start of memory\n"); exit(1); break;
        case 2: printf("Unknown instruction on %d\n", pc); exit(2); break;
        case 3: printf("Memory is full\n"); exit(3); break;
        case 4: printf("Warning: memory is half-full"); break;
    }
}

void check() {
    if(memory[0] != _8086_) error(1);
    if(memory[65535] != 0) error(3);
    if(memory[32700] != 0) error(4);
}

void exec() {
	while(1) {
		int code = memory[pc++];
		switch(code) {
			case LOAD: { int r = memory[pc++]; reg[r] = memory[pc++]; break; }
			case ADD: { int r1 = memory[pc++]; int r2 = memory[pc++]; reg[AX] = reg[r1] + reg[r2]; break; }
			case SUB: { int r1 = memory[pc++]; int r2 = memory[pc++]; reg[AX] = reg[r1] - reg[r2]; break; }
			case MUL: { int r1 = memory[pc++]; int r2 = memory[pc++]; reg[AX] = reg[r1] * reg[r2]; break; }
			case DIV: { int r1 = memory[pc++]; int r2 = memory[pc++]; reg[AX] = reg[r1] / reg[r2]; break; }
			case CMP: {
				int r1 = memory[pc++]; 
				int r2 = memory[pc++];
				int res = reg[r1] - reg[r2];
				ZF = (res == 0);
				SF = (res != 0);
				CF = (reg[r1] < reg[r2]);
				OF = ((reg[r1] ^ reg[r2]) && (reg[r1] ^ res));
				break;
			}
			case OR: { 
				int r1 = memory[pc++]; 
				int r2 = memory[pc++]; 
				reg[AX] = reg[r1] | reg[r2]; break; 
			}
			case AND: {
                                int r1 = memory[pc++];
                                int r2 = memory[pc++];
                                reg[AX] = reg[r1] & reg[r2]; break;
                        }
			case NOT: {
                                int r1 = memory[pc++];
                                reg[AX] = ~reg[r1]; break;
                        }
			case XOR: {
                                int r1 = memory[pc++];
                                int r2 = memory[pc++];
                                reg[AX] = reg[r1] ^ reg[r2]; break;
                        }
			case SHR: {
                                int r1 = memory[pc++];
                                int r2 = memory[pc++];
                                reg[AX] = reg[r1] >> reg[r2]; break;
                        }
			case SHL: {
                                int r1 = memory[pc++];
                                int r2 = memory[pc++];
                                reg[AX] = reg[r1] << reg[r2]; break;
                        }
			case INC: {
                                int r1 = memory[pc++];
                                reg[r1]++; 
				break;
                        }
			case DEC: {
                                int r1 = memory[pc++];
                                reg[r1]--; 
				break;
                        }
			case PUSH: {
				int r = memory[pc++];
				(r != SR) ? error(1) : 0;
				int val = memory[pc++];
				(sc >= 256) ? sr[sc++] = val : error(6);
				break;
			}
			case POP: {
                                int r = memory[pc++];
                                (r != SR) ? error(1) : 0;
                                (sc != 0) ? sr[sc--] = 0 : error(7);
                                break;
                        }
			case RSR: {
				for(int i = 0; i < 256; i++) sr[i] = 0;
				sc = 0;
				break;
			}
			case INT: { int code = memory[pc++]; bios(code); break; }
			case HALT: return;
		}
	}
}



int main()
{
    init();
    check();
    reg[AL] = 92;
    printf("Hello world: %d\n", memory[0]);
    uint16_t prog[] = { LOAD, AL, 1, LOAD, AH, 1, ADD, AH, AL, INT, 0xFF,PUSH, SR, 2, HALT };
    int sprog = sizeof(prog) / sizeof(uint16_t);
    check();
    for(int i = 0; i < sprog; i++) memory[i] = prog[i];
    exec();
    printf("%d\n", sr[1]);
    return 0;
}
