#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <da.h>
#include <mbstring.h>

#include "assembler.h"
#include "cerr.h"
#include "common.h"
#include "macros.h"
#include "parser.h"

#define E_LEXISTS  "label ‘%.*s’ has already been declared"
#define E_LNEXISTS "label ‘%.*s’ hasn’t been declared"

struct label {
	uint16_t addr;
	struct u8view sv;
};

struct labels {
	struct label *buf;
	size_t len, cap;
};

static void pushlabel(struct labels *, struct label);
static uint16_t getaddr(struct raw_addr);
static struct label *getlabel(struct u8view);

static size_t i;
static struct labels locals, globals;

struct label *
getlabel(struct u8view sv)
{
	struct labels *xs = sv.p[0] == '.' ? &locals : &globals;
	da_foreach (xs, x) {
		if (u8eq(x->sv, sv))
			return x;
	}
	return nullptr;
}

uint16_t
getaddr(struct raw_addr a)
{
	struct label *lbl;
	if (!a.label)
		return a.val;
	/* The first 0x200 is reserved for the interpreter, so we need to offset
	   labels that aren’t integer-literals by that. */
	if (lbl = getlabel(a.sv))
		return lbl->addr + 0x200;
	die_with_off(filename, a.sv.p - baseptr, E_LNEXISTS, U8_PRI_ARGS(a.sv));
}

void
pushlabel(struct labels *dst, struct label lbl)
{
	struct label *found;
	if (found = getlabel(lbl.sv)) {
		die_with_off(filename, lbl.sv.p - baseptr, E_LEXISTS,
		             U8_PRI_ARGS(lbl.sv));
	}
	dapush(dst, lbl);
}

void
assemble(FILE *stream, struct ast ast)
{
#define PUT(X) \
	do { \
		uint16_t __x = htons(X); \
		fwrite(&__x, 1, sizeof(__x), stream); \
	} while (false)

	bool pad = false;

	da_foreach (&ast, node) {
		if (node->kind == D_LABEL) {
			struct label lbl = {
				.addr = i,
				.sv = node->name,
			};
			pushlabel(node->name.p[0] == '.' ? &locals : &globals, lbl);
		} else if (node->kind == D_INSTR)
			i += node->instr.kind == I_DB ? node->instr.len : 2;
		else
			unreachable();
	}

	da_foreach (&ast, node) {
		if (node->kind == D_LABEL)
			continue;
		if (node->kind != D_INSTR)
			unreachable();

		/* Instructions need to be 0-padded so they appear on an even byte
		   boundary. */
		if (node->instr.kind != I_DB && pad)
			putchar(0);

		switch (node->instr.kind) {
		case I_ADD_I_VX:
			PUT(0xF01E | (node->instr.args[0].val << 8));
			break;
		case I_ADD_VX_B:
			PUT(0x7000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val);
			break;
		case I_ADD_VX_VY:
			PUT(0x8004 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_AND:
			PUT(0x8002 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_BCD:
			PUT(0xF033 | (node->instr.args[0].val << 8));
			break;
		case I_CALL:
			PUT(0x2000 | getaddr(node->instr.args[0]));
			break;
		case I_CLS:
			PUT(0x00E0);
			break;
		case I_DB:
			da_foreach (&node->instr, byte)
				fputc(*byte, stream);
			if (node->instr.len & 1)
				pad = !pad;
			break;
		case I_DRW:
			PUT(0xD000 | (node->instr.args[0].val << 8)
			    | (node->instr.args[1].val << 4) | node->instr.args[2].val);
			break;
		case I_HEX:
			PUT(0xF029 | (node->instr.args[0].val << 8));
			break;
		case I_JP_A:
			PUT(0x1000 | getaddr(node->instr.args[0]));
			break;
		case I_JP_V0_A:
			PUT(0xB000 | getaddr(node->instr.args[0]));
			break;
		case I_LD_DT:
			PUT(0xF015 | (node->instr.args[0].val << 8));
			break;
		case I_LD_I:
			PUT(0xA000 | getaddr(node->instr.args[0]));
			break;
		case I_LD_ST:
			PUT(0xF018 | (node->instr.args[0].val << 8));
			break;
		case I_LD_VX_B:
			PUT(0x6000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val);
			break;
		case I_LD_VX_DT:
			PUT(0xF007 | (node->instr.args[0].val << 8));
			break;
		case I_LD_VX_K:
			PUT(0xF00A | (node->instr.args[0].val << 8));
			break;
		case I_LD_VX_VY:
			PUT(0x8000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_OR:
			PUT(0x8001 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_RET:
			PUT(0x00EE);
			break;
		case I_RND:
			PUT(0xC000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val);
			break;
		case I_RSTR:
			PUT(0xF065 | (node->instr.args[0].val << 8));
			break;
		case I_SE_VX_B:
			PUT(0x3000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val);
			break;
		case I_SE_VX_VY:
			PUT(0x5000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_SHL:
			PUT(0x800E | (node->instr.args[0].val << 8));
			break;
		case I_SHR:
			PUT(0x8006 | (node->instr.args[0].val << 8));
			break;
		case I_SKNP:
			PUT(0xE0A1 | (node->instr.args[0].val << 8));
			break;
		case I_SKP:
			PUT(0xE09E | (node->instr.args[0].val << 8));
			break;
		case I_SNE_VX_B:
			PUT(0x4000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val);
			break;
		case I_SNE_VX_VY:
			PUT(0x9000 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_STOR:
			PUT(0xF055 | (node->instr.args[0].val << 8));
			break;
		case I_SUB:
			PUT(0x8005 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_SUBN:
			PUT(0x8007 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		case I_SYS:
			PUT(getaddr(node->instr.args[0]));
			break;
		case I_XOR:
			PUT(0x8003 | (node->instr.args[0].val << 8)
			    | node->instr.args[1].val << 4);
			break;
		default:
			unreachable();
		}
	}

	locals.len = 0;

#undef PUT
}
