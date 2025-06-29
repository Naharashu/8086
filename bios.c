#include <stdio.h>
#include "bios.h"
#include "8086.h"
#include <stdlib.h>

unsigned int btime = 0;
uint16_t reg[16];

static void bios_timer() {
    btime++;
    bios_timer();
}

int bios(int code) {
    switch(code) {
        case 0x80: printf("%c\n", reg[1]); break;
        case 0x50: printf("%d\n", btime); break;
	case 0x81: {
		int ch;
		scanf("%c", &ch);
		reg[1] = ch; 
		break;
	} 
	case 0xFF: {
		exit(1); break; 
	}
	case 0x55: {
		printf("BIOS 1.0 (8086)\n");
	}
    }
    return 0;
}
