#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <da.h>
#include <mbstring.h>

#include "assembler.h"
#include "cerr.h"
#include "common.h"
#include "parser.h"

/* TODO: Remove */
#ifndef unreachable
#	define unreachable() __builtin_unreachable()
#endif

#define E_LEXISTS "label ‘%.*s’ has already been declared"

struct label {
	uint16_t addr;
	struct u8view sv;
};

struct labels {
	struct label *buf;
	size_t len, cap;
};

static bool u8eq(struct u8view, struct u8view);
static void pushlabel(struct labels *, struct label);

static size_t i;

bool
u8eq(struct u8view x, struct u8view y)
{
	return x.len == y.len && memcmp(x.p, y.p, x.len) == 0;
}

void
pushlabel(struct labels *dst, struct label lbl)
{
	da_foreach (dst, stored) {
		if (u8eq(stored->sv, lbl.sv)) {
			die_with_off(filename, lbl.sv.p - baseptr, E_LEXISTS,
			             U8_PRI_ARGS(lbl.sv));
		}
	}

	dapush(dst, lbl);
}

void
assemble([[maybe_unused]] FILE *stream, struct ast ast)
{
	static struct labels locals, globals;

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

	locals.len = 0;
}
