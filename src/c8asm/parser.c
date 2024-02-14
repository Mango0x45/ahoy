#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <da.h>

#include "cerr.h"
#include "common.h"
#include "lexer.h"
#include "macros.h"
#include "parser.h"

#define E_BADLABEL  "identifier ‘%.*s’ cannot be used as a label"
#define E_EARLY     "expected %s but input ended prematurely"
#define E_EXPECTED2 "expected %s but got %s"
#define E_EXPECTED  "expected %s but got %s ‘%.*s’"
#define E_INSTR     "got unknown instruction ‘%.*s’"
#define E_TOOLARGE  "expected %s but got out-of-range integer ‘%.*s’"

#define die_with_off(P, ...) die_with_off(filename, (P)-baseptr, __VA_ARGS__)

enum numsize {
	NS_NIBBLE = 0xF,
	NS_BYTE = 0xFF,
	NS_ADDR = 0xFFF,
};

enum regtype {
	RT_NONE,
	RT_DT,
	RT_I,
	RT_K,
	RT_ST,
	RT_VX,
};

static bool parselabel(void);
static void parseline(void);
static void parseop(void);
static struct raw_addr parseaddr(struct token);
static struct token reqnext(const char *, tokkind);

static uint16_t hexval(char);
static uint16_t parsenum(struct token, enum numsize);
static enum regtype regtype(struct u8view);

static void parseop_add(void), parseop_and(void), parseop_bcd(void),
	parseop_call(void), parseop_cls(void), parseop_db(void), parseop_drw(void),
	parseop_hex(void), parseop_jp(void), parseop_ld(void), parseop_or(void),
	parseop_ret(void), parseop_rnd(void), parseop_rstr(void), parseop_se(void),
	parseop_shl(void), parseop_shr(void), parseop_sknp(void), parseop_skp(void),
	parseop_sne(void), parseop_stor(void), parseop_sub(void),
	parseop_subn(void), parseop_sys(void), parseop_xor(void);
#include "autogen-lookup.h"

static size_t i;
static struct ast ast;
static struct tokens *tokens;

struct ast
parsefile(struct tokens toks)
{
	ast.len = i = 0;
	tokens = &toks;

	while (i < toks.len)
		parseline();

	/* We can safely not do this, but GCC disagrees.  This gets GCC to not
	   complain about dangling pointers. */
	tokens = nullptr;

	return ast;
}

void
parseline(void)
{
	while (parselabel())
		;
	parseop();
	reqnext("end of line", T_EOL);
}

bool
parselabel(void)
{
	if (tokens->len - i >= 2 && tokens->buf[i].kind == T_IDENT
	    && tokens->buf[i + 1].kind == T_COLON)
	{
		struct dir lbl = {
			.kind = D_LABEL,
			.name = tokens->buf[i].sv,
		};
		if (regtype(lbl.name) != RT_NONE)
			die_with_off(lbl.name.p, E_BADLABEL, U8_PRI_ARGS(lbl.name));
		dapush(&ast, lbl);
		i += 2;
		return true;
	}

	return false;
}

void
parseop(void)
{
	const struct opf_pair *op;
	struct token tok = reqnext("instruction or end of line", T_IDENT | T_EOL);

	if (tok.kind == T_EOL) {
		i--;
		return;
	}

	if (!(op = oplookup(tok.sv.p, tok.sv.len)))
		die_with_off(tok.sv.p, E_INSTR, U8_PRI_ARGS(tok.sv));
	op->pfn();
}

struct raw_addr
parseaddr(struct token tok)
{
	if (tok.kind == T_NUMBER)
		return (struct raw_addr){.val = parsenum(tok, NS_ADDR)};
	if (tok.kind == T_IDENT) {
		if (regtype(tok.sv) != RT_NONE)
			die_with_off(tok.sv.p, E_BADLABEL, U8_PRI_ARGS(tok.sv));
		return (struct raw_addr){.label = true, .sv = tok.sv};
	}
	unreachable();
}

enum regtype
regtype(struct u8view v)
{
	if (v.len == 0 || v.len > 2)
		return RT_NONE;
	if (v.len == 1)
		return v.p[0] == 'i' ? RT_I : v.p[0] == 'k' ? RT_K : RT_NONE;
	if (memeq(v.p, "dt", 2))
		return RT_DT;
	if (memeq(v.p, "st", 2))
		return RT_ST;
	return v.p[0] == 'v'
	            && ((v.p[1] >= '0' && v.p[1] <= '9')
	                || (v.p[1] >= 'a' && v.p[1] <= 'f'))
	         ? RT_VX
	         : RT_NONE;
}

uint16_t
hexval(char ch)
{
	return ch >= '0' && ch <= '9' ? ch - '0'
	     : ch >= 'a' && ch <= 'f' ? ch - 'a' + 10
	                              : (unreachable(), 0);
}

uint16_t
parsenum(struct token tok, enum numsize size)
{
	char ch;
	uint16_t acc, cutoff, cutlim;
	struct u8view v = tok.sv;

	acc = 0;
	cutoff = size;
	cutlim = cutoff % tok.base;
	cutoff /= tok.base;

	if (v.len >= 2 && v.p[0] == '0' && v.p[1] > '9') {
		v.p += 2;
		v.len -= 2;
	}

	for (ch = *v.p; v.len; v.p++, v.len--, ch = *v.p) {
		if (ch == '\'')
			continue;
		else if (ch >= '0' && ch <= '9')
			ch -= '0';
		else if (ch >= 'a' && ch <= 'f')
			ch -= 'a' - 10;
		else if (ch >= 'A' && ch <= 'F')
			ch -= 'A' - 10;
		else
			unreachable();

		if (acc > cutoff || (acc == cutoff && ch > cutlim)) {
			const char *s = size == NS_NIBBLE ? "nibble"
			              : size == NS_BYTE   ? "byte"
			              : size == NS_ADDR   ? "address"
			                                  : (unreachable(), nullptr);
			die_with_off(tok.sv.p, E_TOOLARGE, s, U8_PRI_ARGS(tok.sv));
		}

		acc *= tok.base;
		acc += ch;
	}

	return acc;
}

struct token
reqnext(const char *want, tokkind msk)
{
	struct token t;
	if (i >= tokens->len)
		die_with_off(baseptr + filesize - 1, E_EARLY, want);

	if ((t = tokens->buf[i++]).kind & msk)
		return t;
	if (t.kind == T_EOL)
		die_with_off(t.sv.p, E_EXPECTED2, want, tokrepr(t.kind));
	die_with_off(t.sv.p, E_EXPECTED, want, tokrepr(t.kind), U8_PRI_ARGS(t.sv));
}

#define I(...) ((struct dir){.kind = D_INSTR, .instr = (__VA_ARGS__)})

/* Common implementations of instructions that always take 1 or 2 v-registers */
#define ONE_VREG(T) \
	do { \
		struct instr ins = {.kind = (T)}; \
		struct token tok = reqnext("v-register", T_IDENT); \
		if (regtype(tok.sv) & ~RT_VX) { \
			die_with_off(tok.sv.p, E_EXPECTED, "v-register", \
			             tokrepr(tok.kind), U8_PRI_ARGS(tok.sv)); \
		} \
		ins.args[ins.len++].val = hexval(tok.sv.p[1]); \
		dapush(&ast, I(ins)); \
	} while (false)
#define TWO_VREG(T) \
	do { \
		struct instr ins = {.kind = (T)}; \
		struct token lhs = reqnext("v-register", T_IDENT); \
		struct token rhs = reqnext("v-register", T_IDENT); \
		if (regtype(lhs.sv) & ~RT_VX) { \
			die_with_off(lhs.sv.p, E_EXPECTED, "v-register", \
			             tokrepr(lhs.kind), U8_PRI_ARGS(lhs.sv)); \
		} \
		if (regtype(rhs.sv) & ~RT_VX) { \
			die_with_off(rhs.sv.p, E_EXPECTED, "v-register", \
			             tokrepr(rhs.kind), U8_PRI_ARGS(rhs.sv)); \
		} \
		ins.args[ins.len++].val = hexval(lhs.sv.p[1]); \
		ins.args[ins.len++].val = hexval(rhs.sv.p[1]); \
		dapush(&ast, I(ins)); \
	} while (false)

void
parseop_add(void)
{
	enum regtype rt;
	struct instr ins = {};
	struct token tok = reqnext("v- or i-register", T_IDENT);

	switch (rt = regtype(tok.sv)) {
	case RT_VX:
		ins.args[ins.len++].val = hexval(tok.sv.p[1]);
		tok = reqnext("byte or v-register", T_IDENT | T_NUMBER);

		if (tok.kind == T_NUMBER) {
			ins.kind = I_ADD_VX_B;
			ins.args[ins.len++].val = parsenum(tok, NS_BYTE);
		} else if (regtype(tok.sv) != RT_VX) {
			die_with_off(tok.sv.p, E_EXPECTED, "v-register", tokrepr(tok.kind),
			             U8_PRI_ARGS(tok.sv));
		} else {
			ins.kind = I_ADD_VX_VY;
			ins.args[ins.len++].val = hexval(tok.sv.p[1]);
		}
		break;
	case RT_I:
		ins.kind = I_ADD_I_VX;
		tok = reqnext("v-register", T_IDENT);
		if (regtype(tok.sv) != RT_VX) {
			die_with_off(tok.sv.p, E_EXPECTED, "v-register", tokrepr(tok.kind),
			             U8_PRI_ARGS(tok.sv));
		}
		ins.args[ins.len++].val = hexval(tok.sv.p[1]);
		break;
	default:
		die_with_off(tok.sv.p, E_EXPECTED, "v- or i-register",
		             tokrepr(tok.kind), U8_PRI_ARGS(tok.sv));
	}

	dapush(&ast, I(ins));
}

void
parseop_and(void)
{
	TWO_VREG(I_AND);
}

void
parseop_bcd(void)
{
	ONE_VREG(I_BCD);
}

void
parseop_call(void)
{
	struct instr ins = {.kind = I_CALL};
	struct token tok = reqnext("address", T_IDENT | T_NUMBER);
	ins.args[ins.len++] = parseaddr(tok);
	dapush(&ast, I(ins));
}

void
parseop_cls(void)
{
	dapush(&ast, I((struct instr){.kind = I_CLS}));
}

void
parseop_db(void)
{
	struct instr ins = {.kind = I_DB};
	do {
		struct token tok = reqnext("byte or string", T_NUMBER | T_STRING);
		switch (tok.kind) {
		case T_NUMBER:
			dapush(&ins, parsenum(tok, NS_BYTE));
			break;
		case T_STRING:
			for (size_t i = 1; i < tok.sv.len - 1; i++)
				dapush(&ins, tok.sv.p[i]);
			break;
		default:
			unreachable();
		}
	} while (i < tokens->len && tokens->buf[i].kind != T_EOL);

	dapush(&ast, I(ins));
}

void
parseop_drw(void)
{
	struct instr ins = {.kind = I_DRW};
	struct token op1, op2, op3;

	op1 = reqnext("v-register", T_IDENT);
	op2 = reqnext("v-register", T_IDENT);
	op3 = reqnext("nibble", T_NUMBER);

	if (regtype(op1.sv) != RT_VX) {
		die_with_off(op1.sv.p, E_EXPECTED, "v-register", tokrepr(op1.kind),
		             U8_PRI_ARGS(op1.sv));
	}
	if (regtype(op2.sv) != RT_VX) {
		die_with_off(op2.sv.p, E_EXPECTED, "v-register", tokrepr(op2.kind),
		             U8_PRI_ARGS(op2.sv));
	}

	ins.args[ins.len++].val = hexval(op1.sv.p[1]);
	ins.args[ins.len++].val = hexval(op2.sv.p[1]);
	ins.args[ins.len++].val = parsenum(op3, NS_NIBBLE);
	dapush(&ast, I(ins));
}

void
parseop_hex(void)
{
	ONE_VREG(I_HEX);
}

void
parseop_jp(void)
{
	enum regtype rt;
	struct instr ins = {};
	struct token op = reqnext("v0-register or address", T_IDENT | T_NUMBER);

	if (op.kind == T_IDENT)
		rt = regtype(op.sv);
	if (op.kind == T_NUMBER || (op.kind == T_IDENT && rt == RT_NONE)) {
		ins.kind = I_JP_A;
		ins.args[ins.len++] = parseaddr(op);
	} else if (op.kind == T_IDENT) {
		ins.kind = I_JP_V0_A;
		if (op.sv.len != 2 || !memeq(op.sv.p, "v0", 2)) {
			die_with_off(op.sv.p, E_EXPECTED, "v0-register or address",
			             tokrepr(op.kind), U8_PRI_ARGS(op.sv));
		}
		ins.args[ins.len++] = parseaddr(reqnext("address", T_NUMBER | T_IDENT));
	} else
		unreachable();

	dapush(&ast, I(ins));
}

void
parseop_ld(void)
{
	enum regtype rt;
	struct instr ins = {};
	struct token op = reqnext("v-, i-, dt-, or st-register", T_IDENT);

	switch (rt = regtype(op.sv)) {
	case RT_DT:
	case RT_ST:
		ins.kind = rt == RT_DT ? I_LD_DT : I_LD_ST;
		op = reqnext("v-register", T_IDENT);
		if (regtype(op.sv) != RT_VX) {
			die_with_off(op.sv.p, E_EXPECTED, "v-register", tokrepr(op.kind),
			             U8_PRI_ARGS(op.sv));
		}
		ins.args[ins.len++].val = hexval(op.sv.p[1]);
		break;

	case RT_I:
		ins.kind = I_LD_I;
		ins.args[ins.len++] = parseaddr(reqnext("address", T_NUMBER | T_IDENT));
		break;

	case RT_VX:
		ins.args[ins.len++].val = hexval(op.sv.p[1]);
		op = reqnext("v-, k-, or dt-register, or byte", T_IDENT | T_NUMBER);

		switch (op.kind) {
		case T_IDENT:
			switch (rt = regtype(op.sv)) {
			case RT_DT:
				ins.kind = I_LD_VX_DT;
				break;
			case RT_K:
				ins.kind = I_LD_VX_K;
				break;
			case RT_VX:
				ins.kind = I_LD_VX_VY;
				ins.args[ins.len++].val = hexval(op.sv.p[1]);
				break;
			default:
				die_with_off(op.sv.p, E_EXPECTED,
				             "v-, k-, or dt-register, or byte",
				             tokrepr(op.kind), U8_PRI_ARGS(op.sv));
			}

			break;
		case T_NUMBER:
			ins.kind = I_LD_VX_B;
			ins.args[ins.len++].val = parsenum(op, NS_BYTE);
			break;
		default:
			unreachable();
		}
		break;

	default:
		die_with_off(op.sv.p, E_EXPECTED, "v-, i-, dt-, or st-register",
		             tokrepr(op.kind), U8_PRI_ARGS(op.sv));
	}

	dapush(&ast, I(ins));
}

void
parseop_or(void)
{
	TWO_VREG(I_OR);
}

void
parseop_ret(void)
{
	dapush(&ast, I((struct instr){.kind = I_RET}));
}

void
parseop_rnd(void)
{
	struct instr ins = {.kind = I_RND};
	struct token op1, op2;

	op1 = reqnext("v-register", T_IDENT);
	op2 = reqnext("byte", T_NUMBER);

	if (regtype(op1.sv) != RT_VX) {
		die_with_off(op1.sv.p, E_EXPECTED, "v-register", tokrepr(op1.kind),
		             U8_PRI_ARGS(op1.sv));
	}

	ins.args[ins.len++].val = hexval(op1.sv.p[1]);
	ins.args[ins.len++].val = parsenum(op2, NS_BYTE);
	dapush(&ast, I(ins));
}

void
parseop_rstr(void)
{
	ONE_VREG(I_RSTR);
}

void
parseop_se(void)
{
	struct instr ins = {};
	struct token op1, op2;

	op1 = reqnext("v-register", T_IDENT);
	op2 = reqnext("byte or v-register", T_IDENT | T_NUMBER);

	if (regtype(op1.sv) != RT_VX) {
		die_with_off(op1.sv.p, E_EXPECTED, "v-register", tokrepr(op1.kind),
		             U8_PRI_ARGS(op1.sv));
	}
	ins.args[ins.len++].val = hexval(op1.sv.p[1]);

	switch (op2.kind) {
	case T_IDENT:
		if (regtype(op2.sv) != RT_VX) {
			die_with_off(op2.sv.p, E_EXPECTED, "v-register", tokrepr(op2.kind),
			             U8_PRI_ARGS(op2.sv));
		}
		ins.kind = I_SE_VX_VY;
		ins.args[ins.len++].val = hexval(op2.sv.p[1]);
		break;
	case T_NUMBER:
		ins.kind = I_SE_VX_B;
		ins.args[ins.len++].val = parsenum(op2, NS_BYTE);
		break;
	default:
		unreachable();
	}

	dapush(&ast, I(ins));
}

void
parseop_shl(void)
{
	ONE_VREG(I_SHL);
}

void
parseop_shr(void)
{
	ONE_VREG(I_SHR);
}

void
parseop_sknp(void)
{
	ONE_VREG(I_SKNP);
}

void
parseop_skp(void)
{
	ONE_VREG(I_SKP);
}

void
parseop_sne(void)
{
	struct instr ins = {};
	struct token op1, op2;

	op1 = reqnext("v-register", T_IDENT);
	op2 = reqnext("byte or v-register", T_IDENT | T_NUMBER);

	if (regtype(op1.sv) != RT_VX) {
		die_with_off(op1.sv.p, E_EXPECTED, "v-register", tokrepr(op1.kind),
		             U8_PRI_ARGS(op1.sv));
	}
	ins.args[ins.len++].val = hexval(op1.sv.p[1]);

	switch (op2.kind) {
	case T_IDENT:
		if (regtype(op2.sv) != RT_VX) {
			die_with_off(op2.sv.p, E_EXPECTED, "v-register", tokrepr(op2.kind),
			             U8_PRI_ARGS(op2.sv));
		}
		ins.kind = I_SNE_VX_VY;
		ins.args[ins.len++].val = hexval(op2.sv.p[1]);
		break;
	case T_NUMBER:
		ins.kind = I_SNE_VX_B;
		ins.args[ins.len++].val = parsenum(op2, NS_BYTE);
		break;
	default:
		unreachable();
	}

	dapush(&ast, I(ins));
}

void
parseop_stor(void)
{
	ONE_VREG(I_STOR);
}

void
parseop_sub(void)
{
	TWO_VREG(I_SUB);
}

void
parseop_subn(void)
{
	TWO_VREG(I_SUBN);
}

void
parseop_sys(void)
{
	struct instr ins = {.kind = I_SYS};
	ins.args[ins.len++] = parseaddr(reqnext("address", T_NUMBER | T_IDENT));
	dapush(&ast, I(ins));
}

void
parseop_xor(void)
{
	TWO_VREG(I_XOR);
}
