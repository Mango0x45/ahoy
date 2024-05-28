#ifndef AHOY_C8ASM_PARSER_H
#define AHOY_C8ASM_PARSER_H

#include <stddef.h>
#include <stdint.h>

#include <mbstring.h>

struct tokens;

typedef enum {
	I_ADD_I_VX,
	I_ADD_VX_B,
	I_ADD_VX_VY,
	I_AND,
	I_BCD,
	I_CALL,
	I_CLS,
	I_DB,
	I_DRW,
	I_HEX,
	I_JP_A,
	I_JP_V0_A,
	I_LD_DT,
	I_LD_I,
	I_LD_ST,
	I_LD_VX_B,
	I_LD_VX_DT,
	I_LD_VX_K,
	I_LD_VX_VY,
	I_OR,
	I_RET,
	I_RND,
	I_RSTR,
	I_SE_VX_B,
	I_SE_VX_VY,
	I_SHL,
	I_SHR,
	I_SKNP,
	I_SKP,
	I_SNE_VX_B,
	I_SNE_VX_VY,
	I_STOR,
	I_SUB,
	I_SUBN,
	I_SYS,
	I_XOR,
} instrkind;

typedef enum {
	R_V0,
	R_V1,
	R_V2,
	R_V3,
	R_V4,
	R_V5,
	R_V6,
	R_V7,
	R_V8,
	R_V9,
	R_VA,
	R_VB,
	R_VC,
	R_VD,
	R_VE,
	R_VF,
	R_I,
	R_K,
	R_DT,
	R_ST,
} reg;

typedef enum {
	D_INSTR,
	D_LABEL,
} dirkind;

/* Arguments can always be represented by a uint16_t, however the parser is not
   responsible for assigning addresses to labels.  As a result an arg at this
   stage can be either a uint16_t or the name of a label. */
struct raw_addr {
	bool label;
	union {
		uint16_t val;
		struct u8view sv;
	};
};

struct instr {
	instrkind kind;

	/* The most arguments any instruction can take is 3, so it’s more efficient
	   to just store the arguments in a fixed-size array.  The only exception is
	   the ‘db’ instruction which takes a variable-number of arguments, so in
	   that case we use a dynamic array. */
	union {
		struct raw_addr args[3];
		struct {
			uint8_t *buf;
			size_t cap;
		};
	};

	size_t len;
};

struct dir {
	dirkind kind;
	union {
		struct u8view name;
		struct instr instr;
	};
};

struct ast {
	struct dir *buf;
	size_t len, cap;
};

struct ast parsefile(struct tokens);

#endif /* !AHOY_C8ASM_PARSER_H */
