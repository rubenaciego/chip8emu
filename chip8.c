#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "chip8.h"

uint8_t fontset[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80 
};

void init(struct Chip8* chip8)
{
	chip8->pc = 0x200;
	chip8->opcode = 0;
	chip8->I = 0;
	chip8->sp = 0;
	chip8->instruction_num = 0;

	for (int i = 0; i < 2048; i++)
		chip8->graphics[i] = 0;
		
	for (int i = 0; i < 16; i++)
		chip8->stack[i] = 0;

	for (int i = 0; i < 16; i++)
		chip8->key[i] = chip8->V[i] = 0;
		
	for (int i = 0; i < 4096; i++)
		chip8->memory[i] = 0;
	
	for (int i = 0; i < 80; i++)
		chip8->memory[i] = fontset[i];		

	chip8->delay_timer = 0;
	chip8->sound_timer = 0;

	chip8->draw = 1;

	srand(time(NULL));
}

void emulate_cycle(struct Chip8* chip8)
{
	chip8->opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];
	
	switch (chip8->opcode & 0xF000)
	{		
		case 0x0000:
			switch (chip8->opcode & 0x000F)
			{
			case 0x0000:
				for (int i = 0; i < 2048; i++)
					chip8->graphics[i] = 0x0;

				chip8->draw = 1;
				chip8->pc += 2;
			break;

			case 0x000E:
				chip8->sp--;
				chip8->pc = chip8->stack[chip8->sp];				
				chip8->pc += 2;
			break;
			}
		break;

		case 0x1000:
			chip8->pc = chip8->opcode & 0x0FFF;
		break;

		case 0x2000:
			chip8->stack[chip8->sp] = chip8->pc;
			chip8->sp++;
			chip8->pc = chip8->opcode & 0x0FFF;
		break;
		
		case 0x3000:
			if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x00FF))
				chip8->pc += 4;
			else
				chip8->pc += 2;
		break;
		
		case 0x4000:
			if (chip8->V[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x00FF))
				chip8->pc += 4;
			else
				chip8->pc += 2;
		break;
		
		case 0x5000:
			if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4])
				chip8->pc += 4;
			else
				chip8->pc += 2;
		break;
		
		case 0x6000:
			chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
			chip8->pc += 2;
		break;
		
		case 0x7000:
			chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
			chip8->pc += 2;
		break;
		
		case 0x8000:
			switch (chip8->opcode & 0x000F)
			{
			case 0x0000:
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4];
				chip8->pc += 2;
			break;

			case 0x0001:
				chip8->V[(chip8->opcode & 0x0F00) >> 8] |= chip8->V[(chip8->opcode & 0x00F0) >> 4];
				chip8->pc += 2;
			break;

			case 0x0002:
				chip8->V[(chip8->opcode & 0x0F00) >> 8] &= chip8->V[(chip8->opcode & 0x00F0) >> 4];
				chip8->pc += 2;
			break;

			case 0x0003:
				chip8->V[(chip8->opcode & 0x0F00) >> 8] ^= chip8->V[(chip8->opcode & 0x00F0) >> 4];
				chip8->pc += 2;
			break;

			case 0x0004:					
				if (chip8->V[(chip8->opcode & 0x00F0) >> 4] > (0xFF - chip8->V[(chip8->opcode & 0x0F00) >> 8])) 
					chip8->V[0xF] = 1;
				else 
					chip8->V[0xF] = 0;
						
				chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->V[(chip8->opcode & 0x00F0) >> 4];
				chip8->pc += 2;					
			break;

			case 0x0005:
				if (chip8->V[(chip8->opcode & 0x00F0) >> 4] > chip8->V[(chip8->opcode & 0x0F00) >> 8]) 
					chip8->V[0xF] = 0;
				else 
					chip8->V[0xF] = 1;
				
				chip8->V[(chip8->opcode & 0x0F00) >> 8] -= chip8->V[(chip8->opcode & 0x00F0) >> 4];
				chip8->pc += 2;
			break;

			case 0x0006:
				chip8->V[0xF] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0x1;
				chip8->V[(chip8->opcode & 0x0F00) >> 8] >>= 1;
				chip8->pc += 2;
			break;

			case 0x0007:
				if (chip8->V[(chip8->opcode & 0x0F00) >> 8] > chip8->V[(chip8->opcode & 0x00F0) >> 4])	// chip8->VY-chip8->VX
					chip8->V[0xF] = 0;
				else
					chip8->V[0xF] = 1;
				
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];				
				chip8->pc += 2;
			break;

			case 0x000E:
				chip8->V[0xF] = chip8->V[(chip8->opcode & 0x0F00) >> 8] >> 7;
				chip8->V[(chip8->opcode & 0x0F00) >> 8] <<= 1;
				chip8->pc += 2;
			break;
			}
		break;
		
		case 0x9000:
			if (chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->V[(chip8->opcode & 0x00F0) >> 4])
				chip8->pc += 4;
			else
				chip8->pc += 2;
		break;

		case 0xA000:
			chip8->I = chip8->opcode & 0x0FFF;
			chip8->pc += 2;
		break;
		
		case 0xB000:
			chip8->pc = (chip8->opcode & 0x0FFF) + chip8->V[0];
		break;
		
		case 0xC000:
			chip8->V[(chip8->opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (chip8->opcode & 0x00FF);
			chip8->pc += 2;
		break;
	
		case 0xD000:
		{
			uint16_t x = chip8->V[(chip8->opcode & 0x0F00) >> 8];
			uint16_t y = chip8->V[(chip8->opcode & 0x00F0) >> 4];
			uint16_t height = chip8->opcode & 0x000F;
			uint16_t pixel;

			chip8->V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = chip8->memory[chip8->I + yline];
				for (int xline = 0; xline < 8; xline++)
				{
					if ((pixel & (0x80 >> xline)) != 0)
					{
						if(chip8->graphics[(x + xline + ((y + yline) * 64))] == 1)
						{
							chip8->V[0xF] = 1;                                    
						}
						chip8->graphics[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
						
			chip8->draw = 1;			
			chip8->pc += 2;
		}
		break;
			
		case 0xE000:
			switch (chip8->opcode & 0x00FF)
			{
			case 0x009E:
				if (chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] != 0)
					chip8->pc += 4;
				else
					chip8->pc += 2;
			break;
			
			case 0x00A1:
				if (chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 0)
					chip8->pc += 4;
				else
					chip8->pc += 2;
			break;
			}
		break;
		
		case 0xF000:
			switch (chip8->opcode & 0x00FF)
			{
			case 0x0007:
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->delay_timer;
				chip8->pc += 2;
			break;
							
			case 0x000A:		
			{
				uint8_t keyPress = 0;

				for (int i = 0; i < 16; i++)
				{
					if(chip8->key[i] != 0)
					{
						chip8->V[(chip8->opcode & 0x0F00) >> 8] = i;
						keyPress = 1;
					}
				}

				if (!keyPress)						
					return;

				chip8->pc += 2;			
			}
			break;
			
			case 0x0015:
				chip8->delay_timer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
				chip8->pc += 2;
			break;

			case 0x0018:
				chip8->sound_timer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
				chip8->pc += 2;
			break;

			case 0x001E:
				if (chip8->I + chip8->V[(chip8->opcode & 0x0F00) >> 8] > 0xFFF)	// chip8->VF is set to 1 when range overflow (chip8->I+chip8->VX>0xFFF), and 0 when there isn't.
					chip8->V[0xF] = 1;
				else
					chip8->V[0xF] = 0;

				chip8->I += chip8->V[(chip8->opcode & 0x0F00) >> 8];
				chip8->pc += 2;
			break;

			case 0x0029:
				chip8->I = chip8->V[(chip8->opcode & 0x0F00) >> 8] * 0x5;
				chip8->pc += 2;
			break;

			case 0x0033:
				chip8->memory[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;
				chip8->memory[chip8->I + 1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] / 10) % 10;
				chip8->memory[chip8->I + 2] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) % 10;					
				chip8->pc += 2;
			break;

			case 0x0055:					
				for (int i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++)
					chip8->memory[chip8->I + i] = chip8->V[i];	

				chip8->I += ((chip8->opcode & 0x0F00) >> 8) + 1;
				chip8->pc += 2;
			break;

			case 0x0065:					
				for (int i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++)
					chip8->V[i] = chip8->memory[chip8->I + i];			

				chip8->I += ((chip8->opcode & 0x0F00) >> 8) + 1;
				chip8->pc += 2;
			break;
			}
		
		break;
	}

	chip8->instruction_num++;

	if (chip8->instruction_num == 9)
	{
		if (chip8->delay_timer > 0)
			chip8->delay_timer--;

		if (chip8->sound_timer > 0)
		{
			if (chip8->sound_timer == 1)
				Beep(750, 160);
			chip8->sound_timer--;
		}

		chip8->instruction_num = 0;
	}
}

void debug_render(struct Chip8* chip8)
{
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (chip8->graphics[(y * 64) + x] == 0) 
				printf("O");
			else 
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

void load_rom(struct Chip8* chip8, const char * filename)
{
	init(chip8);
	printf("Loading: %s\n", filename);
	
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		fputs("File error", stderr);
		getchar();
		exit(-1);
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	rewind(file);
	printf("ROM size: %dB\n", (int)fsize);

	uint8_t* buffer = (uint8_t*)malloc(fsize);
	if (buffer == NULL) 
	{
		fputs("Memory error", stderr); 
		getchar();
		exit(-1);
	}

	size_t result = fread(buffer, 1, fsize, file);
	if (result != fsize) 
	{
		fputs("Reading error", stderr);
		getchar();
		exit(-1);
	}

	if ((4096 - 512) > fsize)
	{
		for(int i = 0; i < fsize; i++)
			chip8->memory[i + 512] = buffer[i];
	}
	else
	{
		printf("Error: ROM too big for memory\n");
		getchar();
		exit(-1);
	}
	
	fclose(file);
	free(buffer);
}
