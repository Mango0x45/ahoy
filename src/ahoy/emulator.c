#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <mbstring.h>

#include "cerr.h"
#include "emulator.h"
#include "macros.h"

#define MEM_RESERVED 0x200
#define MEM_TOTAL    0xFFF
#define MEM_FREE     (MEM_TOTAL - MEM_RESERVED)

static void opexec(uint16_t);
[[noreturn]] static void badins(uint16_t);

/* Uppercase variables in this file will be used to represent registers.  The
   following registers exist:

   Vx — 16 general-purpose registers
   DT — delay timer
   ST — sound timer
   SP — stack pointer
   PC — program counter
   I  — register to hold addresses
   */
static uint8_t V[16];
static uint8_t DT, ST, SP;
static uint16_t PC, I;

static bool kbd[16];
static uint16_t callstack[16];
static uint64_t screen[32];

/* Preload font into memory */
static uint8_t mem[MEM_TOTAL] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
};

static void
scrdrw(void)
{
	fputs("\33[2J", stdout);
	for (size_t i = 0; i < lengthof(screen); i++) {
		for (size_t j = UINT64_WIDTH; j-- > 0;) {
			char buf[U8_LEN_MAX];
			bool bitset = (screen[i] & ((uint64_t)1 << j)) != 0;
			int w = rtou8(buf, bitset ? U'█' : U' ', sizeof(buf));
			fwrite(buf, 1, w, stdout);
		}
		putchar('\n');
	}
}

void
emulate(struct u8view prog)
{
	struct timespec tp;

	if (prog.len > MEM_FREE)
		diex("%s: binary of size %zu KiB too large to fit in RAM", "TODO", 0ul);

	PC = MEM_RESERVED;
	memcpy(mem + PC, prog.p, prog.len);

	clock_gettime(CLOCK_REALTIME, &tp);
	srand(tp.tv_sec ^ tp.tv_nsec);

	for (;; PC += 2) {
		uint16_t op = (mem[PC] << 8) | mem[PC + 1];
		opexec(op);
		scrdrw();
	}
}

void
opexec(uint16_t op)
{
	switch (op >> 12) {
	case 0x0:
		switch (op) {
		case 0x00E0:
			memset(screen, 0, sizeof(screen));
			break;
		case 0x00EE:
			if (SP == 0)
				diex("%s: stack pointer underflow", "TODO");
			PC = callstack[--SP];
			break;
		default:
			/* sys instruction is ignored */
		}
		break;

	case 0x1:
		/* -2 because we +2 each iteration */
		PC = (op & 0xFFF) - 2;
		break;

	case 0x2:
		if (SP == lengthof(callstack))
			diex("%s: stack pointer overflow", "TODO");
		callstack[SP++] = PC;
		PC = (op & 0xFFF) - 2;
		break;

	case 0x3: {
		unsigned x = (op & 0x0F00) >> 8;
		if (V[x] == (op & 0xFF))
			PC += 2;
		break;
	}

	case 0x4: {
		unsigned x = (op & 0x0F00) >> 8;
		if (V[x] != (op & 0xFF))
			PC += 2;
		break;
	}

	case 0x5: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;
		if (V[x] == V[y])
			PC += 2;
		break;
	}

	case 0x6: {
		unsigned x = (op & 0x0F00) >> 8;
		V[x] = op & 0xFF;
		break;
	}

	case 0x7: {
		unsigned x = (op & 0x0F00) >> 8;
		V[x] += op & 0xFF;
		break;
	}

	case 0x8: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;

		switch (op & 0xF) {
		case 0x0:
			V[x] = V[y];
			break;
		case 0x1:
			V[x] |= V[y];
			break;
		case 0x2:
			V[x] &= V[y];
			break;
		case 0x3:
			V[x] ^= V[y];
			break;
		case 0x4: {
			unsigned n = V[x] + V[y];
			V[x] = n;
			V[0xF] = n > UINT8_MAX;
			break;
		}
		case 0x5:
			V[0xF] = V[x] > V[y];
			V[x] -= V[y];
			break;
		case 0x6:
			V[0xF] = V[x] & 1;
			V[x] >>= 1;
			break;
		case 0x7:
			V[0xF] = V[y] > V[x];
			V[x] = V[y] - V[x];
			break;
		case 0xE:
			V[0xF] = V[x] & 0x80;
			V[x] <<= 1;
			break;
		default:
			badins(op);
		}

		break;
	}

	case 0x9: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;
		if (V[x] != V[y])
			PC += 2;
		break;
	}

	case 0xA:
		I = op & 0xFFF;
		break;

	case 0xB:
		PC = (op & 0xFFF) + V[0] - 2;
		break;

	case 0xC: {
		unsigned x = (op & 0x0F00) >> 8;
		V[x] = rand() & (op & 0xFF);
		break;
	}

	case 0xD: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;
		unsigned n = (op & 0x000F) >> 0;

		for (unsigned i = 0; i < n; i++) {
			/* TODO: bounds check? */
			uint8_t spr_row = mem[I + i];
			uint8_t scr_row = V[y] + i;
			uint64_t msk;

			if (scr_row >= lengthof(screen))
				break;

			msk = ((uint64_t)spr_row << (64 - 8)) >> V[x];
			V[0xF] = screen[scr_row] & msk;
			screen[scr_row] ^= msk;
		}
		break;
	}

	case 0xE: {
		unsigned x = (op & 0x0F00) >> 8;

		switch (op & 0xFF) {
		case 0x9E:
			if (V[x] < lengthof(kbd) && kbd[V[x]])
				PC += 2;
			break;
		case 0xA1:
			if (V[x] >= lengthof(kbd) || !kbd[V[x]])
				PC += 2;
			break;
		default:
			badins(op);
		}
		break;
	}

	case 0xF: {
		unsigned x = (op & 0x0F00) >> 8;

		switch (op & 0xFF) {
		case 0x07:
			V[x] = DT;
			break;
		case 0x0A:
			badins(op);
			break;
		case 0x15:
			DT = V[x];
			break;
		case 0x18:
			ST = V[x];
			break;
		case 0x1E:
			I += V[x];
			break;
		case 0x29:
			/* Each character sprite is 5 bytes */
			I = V[x] * 5;
			break;
		case 0x33:
			mem[I + 0] = V[x] / 100 % 10;
			mem[I + 1] = V[x] / 10 % 10;
			mem[I + 2] = V[x] / 1 % 10;
			break;
		case 0x55:
			memcpy(mem + I, V, x);
			break;
		case 0x65:
			memcpy(V, mem + I, x);
			break;
		default:
			badins(op);
		}

		break;
	}

	default:
		unreachable();
	}
}

void
badins(uint16_t op)
{
	diex("%s: invalid opcode: %04X", "TODO", op);
}
