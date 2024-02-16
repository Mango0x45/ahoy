#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <mbstring.h>

#include "cerr.h"
#include "emulator.h"
#include "macros.h"

#define MEM_RESERVED 0x200
#define MEM_TOTAL    0xFFF
#define MEM_FREE     (MEM_TOTAL - MEM_RESERVED)

static void opexec(uint16_t);
[[noreturn]] static void badins(uint16_t);

struct chip8 c8;
static const char *filename;

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

void
emuinit(struct u8view prog, const char *fn)
{
	struct timespec tp;

	filename = fn;

	if (prog.len > MEM_FREE) {
		diex("%s: binary of size %.1f KiB exceeds %d B maximum", filename,
		     (double)prog.len / 1024, MEM_FREE);
	}

	c8.PC = MEM_RESERVED;
	memcpy(mem + c8.PC, prog.p, prog.len);

	if (clock_gettime(CLOCK_REALTIME, &tp) == -1)
		die("clock_gettime");
	srand(tp.tv_sec ^ tp.tv_nsec);
}

void
emutick(void)
{
	opexec((mem[c8.PC] << 8) | mem[c8.PC + 1]);
	c8.PC += 2;
}

void
opexec(uint16_t op)
{
	switch (op >> 12) {
	case 0x0:
		switch (op) {
		case 0x00E0:
			memset(c8.screen, 0, sizeof(c8.screen));
			c8.needs_redraw = true;
			break;
		case 0x00EE:
			if (c8.SP == 0)
				diex("%s: stack pointer underflow", filename);
			c8.PC = c8.callstack[--c8.SP];
			break;
		default:
			/* sys instruction is ignored */
		}
		break;

	case 0x1:
		/* -2 because we +2 each iteration */
		c8.PC = (op & 0xFFF) - 2;
		break;

	case 0x2:
		if (c8.SP == lengthof(c8.callstack))
			diex("%s: stack pointer overflow", filename);
		c8.callstack[c8.SP++] = c8.PC;
		c8.PC = (op & 0xFFF) - 2;
		break;

	case 0x3: {
		unsigned x = (op & 0x0F00) >> 8;
		if (c8.V[x] == (op & 0xFF))
			c8.PC += 2;
		break;
	}

	case 0x4: {
		unsigned x = (op & 0x0F00) >> 8;
		if (c8.V[x] != (op & 0xFF))
			c8.PC += 2;
		break;
	}

	case 0x5: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;
		if (c8.V[x] == c8.V[y])
			c8.PC += 2;
		break;
	}

	case 0x6: {
		unsigned x = (op & 0x0F00) >> 8;
		c8.V[x] = op & 0xFF;
		break;
	}

	case 0x7: {
		unsigned x = (op & 0x0F00) >> 8;
		c8.V[x] += op & 0xFF;
		break;
	}

	case 0x8: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;

		switch (op & 0xF) {
		case 0x0:
			c8.V[x] = c8.V[y];
			break;
		case 0x1:
			c8.V[x] |= c8.V[y];
			break;
		case 0x2:
			c8.V[x] &= c8.V[y];
			break;
		case 0x3:
			c8.V[x] ^= c8.V[y];
			break;
		case 0x4: {
			unsigned n = c8.V[x] + c8.V[y];
			c8.V[x] = n;
			c8.V[0xF] = n > UINT8_MAX;
			break;
		}
		case 0x5:
			c8.V[0xF] = c8.V[x] > c8.V[y];
			c8.V[x] -= c8.V[y];
			break;
		case 0x6:
			c8.V[0xF] = c8.V[x] & 1;
			c8.V[x] >>= 1;
			break;
		case 0x7:
			c8.V[0xF] = c8.V[y] > c8.V[x];
			c8.V[x] = c8.V[y] - c8.V[x];
			break;
		case 0xE:
			c8.V[0xF] = c8.V[x] & 0x80;
			c8.V[x] <<= 1;
			break;
		default:
			badins(op);
		}

		break;
	}

	case 0x9: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;
		if (c8.V[x] != c8.V[y])
			c8.PC += 2;
		break;
	}

	case 0xA:
		c8.I = op & 0xFFF;
		break;

	case 0xB:
		c8.PC = (op & 0xFFF) + c8.V[0] - 2;
		break;

	case 0xC: {
		unsigned x = (op & 0x0F00) >> 8;
		c8.V[x] = rand() & (op & 0xFF);
		break;
	}

	case 0xD: {
		unsigned x = (op & 0x0F00) >> 8;
		unsigned y = (op & 0x00F0) >> 4;
		unsigned n = (op & 0x000F) >> 0;

		for (unsigned i = 0; i < n; i++) {
			/* TODO: bounds check? */
			uint8_t spr_row = mem[c8.I + i];
			uint8_t scr_row = c8.V[y] + i;
			uint64_t msk;

			if (scr_row >= lengthof(c8.screen))
				break;

			msk = ((uint64_t)spr_row << (UINT64_WIDTH - 8)) >> c8.V[x];
			c8.V[0xF] = (bool)(c8.screen[scr_row] & msk);
			c8.screen[scr_row] ^= msk;
		}

		c8.needs_redraw = true;
		break;
	}

	case 0xE: {
		unsigned x = (op & 0x0F00) >> 8;

		switch (op & 0xFF) {
		case 0x9E:
			if (c8.V[x] < lengthof(c8.kbd) && c8.kbd[c8.V[x]])
				c8.PC += 2;
			break;
		case 0xA1:
			if (c8.V[x] >= lengthof(c8.kbd) || !c8.kbd[c8.V[x]])
				c8.PC += 2;
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
			c8.V[x] = c8.DT;
			break;
		case 0x0A: {
			static bool any_key_pressed = false;
			static uint8_t key = 0xFF;

			for (uint8_t i = 0; key == 0xFF && i < lengthof(c8.kbd); i++) {
				if (c8.kbd[i]) {
					key = i;
					any_key_pressed = true;
					break;
				}
			}

			if (!any_key_pressed)
				c8.PC -= 2;
			else {
				if (c8.kbd[key])
					c8.PC -= 2;
				else {
					c8.V[x] = key;
					key = 0xFF;
					any_key_pressed = false;
				}
			}
			break;
		}
		case 0x15:
			c8.DT = c8.V[x];
			break;
		case 0x18:
			c8.ST = c8.V[x];
			break;
		case 0x1E:
			c8.I += c8.V[x];
			break;
		case 0x29:
			/* Each character sprite is 5 bytes */
			c8.I = c8.V[x] * 5;
			break;
		case 0x33:
			mem[c8.I + 0] = c8.V[x] / 100 % 10;
			mem[c8.I + 1] = c8.V[x] / 10 % 10;
			mem[c8.I + 2] = c8.V[x] / 1 % 10;
			break;
		case 0x55:
			memcpy(mem + c8.I, c8.V, x);
			c8.I += x;
			break;
		case 0x65:
			memcpy(c8.V, mem + c8.I, x);
			c8.I += x;
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
	diex("%s: invalid opcode: %04X", filename, op);
}
