#include <da.h>
#include <mbstring.h>
#include <rtype.h>

#include "cerr.h"
#include "common.h"
#include "lexer.h"
#include "macros.h"

#define ISDIGIT(n)   ((n) >= '0' && (n) <= '9')
#define U8MOV(sv, n) ((sv)->p += (n), (sv)->len -= (n))

#define E_BASE         "integer with invalid base specifier"
#define E_EXTRA        "unknown extraneous character"
#define E_IDENTCCHAR   "illegal character in identifier"
#define E_IDENTLOST    "local label missing identifier"
#define E_IDENTSCHAR   "illegal first character in identifier"
#define E_UNTERMINATED "unterminated string literal"
#define E_UTF8         "invalid UTF-8 byte near ‘%02X’"

#define EOLS     U"\n\v\f\r\x85\u2028\u2029"
#define NUMCHARS U"'0123456789abcdefABCDEF"

static void lexline(struct tokens *, struct u8view *);
static bool skipws(struct u8view *);

const char *
tokrepr(tokkind k)
{
	return (const char *[]){
		[T_COLON] = "colon",      [T_EOL] = "end of line",
		[T_IDENT] = "identifier", [T_NUMBER] = "number",
		[T_STRING] = "string",
	}[k];
}

struct tokens
lexfile(void)
{
	const char8_t *s;
	struct tokens toks;
	struct u8view sv = filebuf;

	if (s = u8chk(sv.p, sv.len))
		die_with_off(filename, s - sv.p, E_UTF8, *s);

	dainit(&toks, 256);

	while (sv.len) {
		size_t len = u8cbspn(sv.p, sv.len, EOLS, lengthof(EOLS) - 1);
		struct u8view line = {
			.p = sv.p,
			.len = len,
		};

		lexline(&toks, &line);

		/* Skip trailing EOL */
		if (sv.len > len)
			len += u8rlen(sv.p + len);

		U8MOV(&sv, len);
	}

	return toks;
}

#define DIE_AT_POS_WITH_CODE2(HL, ...)                                         \
	DIE_AT_POS_WITH_CODE((HL), sv->p - w, __VA_ARGS__)

void
lexline(struct tokens *toks, struct u8view *sv)
{
	struct token tok;

	for (;;) {
		int w;
		rune ch;

		if (!skipws(sv))
			goto end;

		tok.sv.p = sv->p;
		tok.sv.len = w = u8next(&ch, &sv->p, &sv->len);

		if (ISDIGIT(ch)) {
			size_t off, m = 10;

			tok.kind = T_NUMBER;
			tok.base = 10;

			if (ch == '0') {
				w = u8next(&ch, &sv->p, &sv->len);
				if (!w || rprop_is_pat_ws(ch)) {
					U8MOV(sv, -w);
					goto out;
				}
				tok.sv.len++;

				switch (ch) {
				case 'b':
					tok.base = m = 2;
					break;
				case 'o':
					tok.base = m = 8;
					break;
				case 'd':
					/* Implicitly base-10 already */
					break;
				case 'x':
					/* m = 22 because A–F can be both upper- or lowercase */
					tok.base = 16;
					m = 22;
					break;
				default:
					if (!ISDIGIT(ch))
						DIE_AT_POS_WITH_CODE2(tok.sv, E_BASE);
				}
			}

out:
			/* +1 to support the digit separator */
			tok.sv.len += off = u8bspn(sv->p, sv->len, NUMCHARS, m + 1);
			U8MOV(sv, off);
		} else if (ch == '.' || ch == '_' || rprop_is_xids(ch)) {
			tok.kind = T_IDENT;
			if (ch == '.') {
				tok.sv.len += w = u8next(&ch, &sv->p, &sv->len);
				if (!w || rprop_is_pat_ws(ch))
					DIE_AT_POS_WITH_CODE2(tok.sv, E_IDENTLOST);
				if (ch != '_' && !rprop_is_xids(ch)) {
					U8MOV(&tok.sv, 1);
					DIE_AT_POS_WITH_CODE2(tok.sv, E_IDENTSCHAR);
				}
			}

			while (w = u8next(&ch, &sv->p, &sv->len)) {
				if (ch == ':' || rprop_is_pat_ws(ch)) {
					U8MOV(sv, -w);
					break;
				}
				if (!rprop_is_xidc(ch)) {
					struct u8view hl = {
						.p = sv->p - w,
						.len = w,
					};
					DIE_AT_POS_WITH_CODE2(hl, E_IDENTCCHAR);
				}

				tok.sv.len += w;
			}
		} else if (ch == '"') {
			tok.kind = T_STRING;
			while (w = u8next(&ch, &sv->p, &sv->len)) {
				tok.sv.len += w;
				if (ch == '"')
					goto found;
			}
			DIE_AT_POS_WITH_CODE2(tok.sv, E_UNTERMINATED);
found:
		} else if (ch == ':') {
			tok.kind = T_COLON;
		} else if (ch == ';') {
			goto end;
		} else {
			struct u8view hl = {.p = sv->p - w, .len = w};
			DIE_AT_POS_WITH_CODE2(hl, E_EXTRA);
		}

		/* The colon is the only token that isn’t whitespace separated */
		if (ch != ':' && sv->len) {
			w = u8next(&ch, &sv->p, &sv->len);
			if (!w || !rprop_is_pat_ws(ch)) {
				struct u8view hl = {.p = sv->p - w, .len = w};
				DIE_AT_POS_WITH_CODE2(hl, E_EXTRA);
			}
		}

		dapush(toks, tok);
	}

end:
	tok = (struct token){
		.kind = T_EOL,
		.sv.p = sv->p,
		.sv.len = 0,
	};
	dapush(toks, tok);
}

#undef DIE_AT_POS_WITH_CODE2

bool
skipws(struct u8view *sv)
{
	rune ch;

	if (!sv->len)
		return false;

	for (int w = u8tor_uc(&ch, sv->p); rprop_is_pat_ws(ch);
	     w = u8tor_uc(&ch, sv->p))
	{
		U8MOV(sv, w);
		if (!sv->len)
			return false;
	}

	return true;
}
