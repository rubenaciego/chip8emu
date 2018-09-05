#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

struct Chip8 
{
	uint8_t draw;
	uint8_t graphics[64 * 32];
	uint8_t  key[16];

	uint16_t pc;			
	uint16_t opcode;		
	uint16_t I;
	uint16_t sp;			
	
	uint8_t V[16];
	uint16_t stack[16];
	uint8_t memory[4096];
			
	uint8_t delay_timer;
	uint8_t sound_timer;
};

void emulate_cycle(struct Chip8* chip8);
void debug_render(struct Chip8* chip8);
void load_rom(struct Chip8* chip8, const char* filename);
void init(struct Chip8* chip8);

#endif
