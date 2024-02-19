#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mbstring.h>

#include "cerr.h"
#include "macros.h"
#include "mbstring.h"

#define TAB_AS_SPC "        "

#define EOLS8  u8"\n\v\f\r\x85\u2028\u2029"
#define EOLS32 U"\n\v\f\r\x85\u2028\u2029"

#define SGR_BOLD "\33[1m"
#define SGR_DONE "\33[0m"
#define SGR_ERR  "\33[1;31m"

int sizelen(size_t);

static bool color;
static const char *progname;

static const char *_bold, *_done, *_err;

void
cerrinit(const char *s)
{
	const char *p = strrchr(s, '/');
	progname = p ? p + 1 : s;

	if (isatty(STDOUT_FILENO)) {
		const char *ev = getenv("NO_COLOR");
		color = !ev || !*ev;
	}

	if (color) {
		_bold = SGR_BOLD;
		_done = SGR_DONE;
		_err = SGR_ERR;
	} else
		_bold = _done = _err = "";
}

void
die(const char *fmt, ...)
{
	va_list ap;
	int e = errno;

	va_start(ap, fmt);
	fprintf(stderr, "%s%s:%s ", _bold, progname, _done);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ": %s\n", strerror(e));
	va_end(ap);

	exit(EXIT_FAILURE);
}

void
diex(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s%s:%s ", _bold, progname, _done);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);

	exit(EXIT_FAILURE);
}

void
die_with_off(const char *file, size_t off, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s%s: %s:%zu:%s %serror:%s ", _bold, progname, file, off,
	        _done, _err, _done);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);

	exit(EXIT_FAILURE);
}

void
die_at_pos_with_code(const char *file, struct u8view sv, struct u8view hl,
                     size_t off, const char *fmt, ...)
{
	int w;
	rune _;
	size_t lb;
	va_list ap;
	const char8_t *prv, *end;

	va_start(ap, fmt);
	fprintf(stderr, "%s%s: %s:%zu:%s %serror:%s ", _bold, progname, file, off,
	        _done, _err, _done);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);

	for (lb = 0, prv = end = sv.p; end <= sv.p + off; lb++) {
		prv = end;
		end += u8cbspn(end, sv.p + sv.len - end, EOLS32, lengthof(EOLS32) - 1);
		end += u8tor_uc(&_, end);
	}

	u8prev(&_, &end, prv);
	w = sizelen(lb);
	w = MAX(w, 4);

	fprintf(stderr, " %*zu │ ", w, lb);

	/* The following is really ugly, but it works! */
	if (hl.p) {
		ptrdiff_t w2 = hl.p - prv;
		struct u8view pfx = {prv, w2};

		for (ptrdiff_t i = 0; i < w2; i++) {
			if (prv[i] == '\t')
				fputs(TAB_AS_SPC, stderr);
			else
				fputc(prv[i], stderr);
		}
		fprintf(stderr, "%s%.*s%s%.*s\n", _err, U8_PRI_ARGS(hl), _done,
		        (int)(end - (prv + w2 + hl.len)), prv + w2 + hl.len);
		fprintf(stderr, " %*c │ ", w, ' ');

		while (u8next(&_, &pfx.p, &pfx.len)) {
			if (_ == '\t')
				fputs(TAB_AS_SPC, stderr);
			else
				fputc(' ', stderr);
		}

		fprintf(stderr, "%s^", _err);
		for (u8next(&_, &hl.p, &hl.len); u8next(&_, &hl.p, &hl.len);
		     fputc('~', stderr))
			;
		fprintf(stderr, "%s\n", _done);
	} else {
		fprintf(stderr, "%.*s\n", (int)(end - prv), prv);
		fprintf(stderr, " %*c │\n", w, ' ');
	}

	exit(EXIT_FAILURE);
}

int
sizelen(size_t x)
{
	int n;
	for (n = 0; x; x /= 10, n++)
		;
	return n;
}
