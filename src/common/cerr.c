#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cerr.h"

#define SGR_BOLD "\33[1m"
#define SGR_DONE "\33[0m"

static bool color;
static const char *progname;

static const char *_bold, *_done;

void
cerrinit(const char *s)
{
	const char *p = strrchr(s, '/');
	progname = p ? p + 1 : s;

	if (isatty(STDOUT_FILENO)) {
		const char *ev = getenv("NO_COLOR");
		if (!ev || !*ev)
			color = true;
	}

	if (color) {
		_bold = SGR_BOLD;
		_done = SGR_DONE;
	} else {
		_bold = "";
		_done = "";
	}
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
	fprintf(stderr, "%s%s:%s:%zu:%s ", _bold, progname, file, off, _done);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);

	exit(EXIT_FAILURE);
}
