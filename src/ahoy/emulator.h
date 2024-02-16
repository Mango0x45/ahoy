#ifndef AHOY_AHOY_EMULATOR_H
#define AHOY_AHOY_EMULATOR_H

#include <stdint.h>

#include <mbstring.h>

/* Uppercase variables represent registers.  The following registers exist:

   Vx — 16 general-purpose registers
   DT — delay timer
   ST — sound timer
   SP — stack pointer
   PC — program counter
   I  — register to hold addresses
   */
struct chip8 {
	bool kbd[16];
	uint8_t V[16];
	uint8_t DT, ST, SP;
	uint16_t PC, I;
	uint16_t callstack[16];
	uint64_t screen[32];
};

void emuinit(struct u8view, const char *);
void emutick(void);

extern struct chip8 c8;

#endif /* !AHOY_AHOY_EMULATOR_H */
