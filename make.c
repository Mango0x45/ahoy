#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <glob.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __has_include(<features.h>)
#	include <features.h>
#endif

#include "cbs.h"

#define WARN \
	"-Wall", "-Wextra", "-Wpedantic", "-Werror", \
		"-Wno-attributes",  /* GNU attributes in headers */ \
		"-Wno-parentheses", /* if (x = foo()) */ \
		"-Wno-pointer-sign" /* char ↔ char8_t */

#define CC "cc"
#ifdef __GLIBC__
#	define CFLAGS WARN, "-std=c2x", "-D_GNU_SOURCE"
#else
#	define CFLAGS WARN, "-std=c2x"
#endif
#define CFLAGS_DBG CFLAGS, "-Og", "-ggdb3", "-DDEBUG=1"
#ifdef __APPLE__
#	define CFLAGS_RLS CFLAGS, "-O3", "-flto"
#else
#	define CFLAGS_RLS CFLAGS, "-O3", "-flto", "-march=native", "-mtune=native"
#endif

#define streq(x, y) (!strcmp(x, y))

#define FLAGMSK(f) ((uint64_t)1 << ((f) - ((f) < 'a' ? 'A' : 'G')))
#define FLAGSET(f) (flags & FLAGMSK(f))

#define _CMDPRC(c, f) \
	do { \
		int ec; \
		f(c); \
		if ((ec = cmdexec(c)) != EXIT_SUCCESS) \
			diex("%s terminated with exit-code %d", *c._argv, ec); \
		cmdclr(&c); \
	} while (0)
#define CMDPRC(c)  _CMDPRC(c, cmdput)
#define CMDPRC2(c) _CMDPRC(c, cmdput2)

enum libs {
	L_CERR = 1 << 0,
	L_LIBRUNE = 1 << 1,
};

static void build_ahoy(void);
static void build_c8asm(void);
static void build_c8dump(void);
static void build_common(void);
static void build_librune(void);
static void mkb(char *, char **, size_t, struct strv, enum libs);
static void mkc(char *, struct strv);
static int globerr(const char *, int);
static void cmdput2(cmd_t);

static uint64_t flags;
static const char *argv0;
static bool librune_built;

static void
usage(void)
{
	fprintf(stderr, "Usage: %s [-fr]\n", argv0);
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	int opt;
	const struct option longopts[] = {
		{"force",   no_argument, nullptr, 'f'},
		{"release", no_argument, nullptr, 'r'},
		{nullptr,   no_argument, nullptr, 0  },
	};

	cbsinit(argc, argv);
	rebuild();
	argv0 = *argv;

	while ((opt = getopt_long(argc, argv, "fr", longopts, nullptr)) != -1) {
		switch (opt) {
		case '?':
			usage();
		default:
			flags |= FLAGMSK(opt);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		build_common();
		build_c8dump();
		build_c8asm();
		build_ahoy();
	} else if (streq(*argv, "clean")) {
		cmd_t c = {0};
		cmdadd(&c, "find", ".", "-type", "f", "(", "-name", "*.[ao]", "-or",
		       "-name", "ahoy", "-or", "-name", "c8asm", "-or", "-name",
		       "c8dump", "-or", "-path", "./src/c8asm/autogen-lookup.h", ")",
		       "-delete");
		CMDPRC(c);
	} else {
		warnx("invalid subcommand -- '%s'", *argv);
		usage();
	}

	return EXIT_SUCCESS;
}

void
mkb(char *dst, char **srcs, size_t srcslen, struct strv extras, enum libs libs)
{
	static cmd_t c;
	static struct strv sv;

	for (size_t i = 0; i < srcslen; i++) {
		size_t n = strlen(srcs[i]);
		assert(srcs[i][n - 1] == 'c');
		srcs[i][n - 1] = 'o';
	}

	if (!sv.buf) {
		env_or_default(&sv, "CC", CC);
		env_or_default(&sv, "LDFLAGS", nullptr);
		if (FLAGSET('r'))
			env_or_default(&sv, "CFLAGS", CFLAGS);
	}

	if (foutdatedv(dst, (const char **)srcs, srcslen) || FLAGSET('f')) {
		c.dst = dst;
		cmdaddv(&c, sv.buf, sv.len);
		cmdaddv(&c, extras.buf, extras.len);
		cmdadd(&c, "-Isrc/common", "-Ivendor/da", "-Ivendor/librune/include");
		cmdadd(&c, "-o", dst);
		cmdaddv(&c, srcs, srcslen);
		if (libs & L_CERR)
			cmdadd(&c, "src/common/cerr.o");
		if (libs & L_LIBRUNE)
			cmdadd(&c, "vendor/librune/librune.a");
		CMDPRC2(c);
	}
}

void
mkc(char *src, struct strv extras)
{
	size_t len;
	char *dst;
	static cmd_t c;
	static struct strv sv;

	len = strlen(src);
	if (!(dst = strdup(src)))
		die("strdup");
	assert(dst[len - 1] == 'c');
	dst[len - 1] = 'o';

	if (!sv.buf) {
		env_or_default(&sv, "CC", CC);
		if (FLAGSET('r'))
			env_or_default(&sv, "CFLAGS", CFLAGS_RLS);
		else
			env_or_default(&sv, "CFLAGS", CFLAGS_DBG);
	}

	if (foutdated(dst, src) || FLAGSET('f')) {
		c.dst = dst;
		cmdaddv(&c, sv.buf, sv.len);
		cmdaddv(&c, extras.buf, extras.len);
		cmdadd(&c, "-Isrc/common", "-Ivendor/da", "-Ivendor/librune/include");
		cmdadd(&c, "-c", src, "-o", dst);
		CMDPRC2(c);
	}

	free(dst);
}

void
build_common(void)
{
	glob_t g;

	build_librune();
	if (glob("src/common/*.c", 0, globerr, &g))
		die("glob");
	for (size_t i = 0; i < g.gl_pathc; i++)
		mkc(g.gl_pathv[i], (struct strv){});
	globfree(&g);
}

void
build_ahoy(void)
{
	glob_t g;
	struct strv sv = {}, pc = {};

	build_librune();
	if (glob("src/ahoy/*.c", 0, globerr, &g))
		die("glob");
	(void)pcquery(&pc, "sdl2", PKGC_CFLAGS);
	for (size_t i = 0; i < g.gl_pathc; i++)
		mkc(g.gl_pathv[i], pc);
	strvfree(&pc);
	if (!pcquery(&pc, "sdl2", PKGC_LIBS))
		pc = (struct strv){.buf = (char *[]){"-SDL2"}, .len = 1};
	mkb("ahoy", g.gl_pathv, g.gl_pathc, pc, L_CERR | L_LIBRUNE);

	globfree(&g);
	strvfree(&pc);
}

void
build_c8asm(void)
{
	glob_t g;
	cmd_t c = {0};

	if (!binexists("gperf"))
		diex("gperf is required to build c8asm");
	build_librune();

	if (foutdated("src/c8asm/autogen-lookup.h", "src/c8asm/instr.gperf")
	    || FLAGSET('f'))
	{
		c.dst = "src/c8asm/autogen-lookup.h";
		cmdadd(&c, "gperf", "src/c8asm/instr.gperf", "--output-file", c.dst);
		CMDPRC2(c);
	}

	if (glob("src/c8asm/*.c", 0, globerr, &g))
		die("glob");
	for (size_t i = 0; i < g.gl_pathc; i++)
		mkc(g.gl_pathv[i], (struct strv){});
	mkb("c8asm", g.gl_pathv, g.gl_pathc, (struct strv){}, L_CERR | L_LIBRUNE);

	globfree(&g);
}

void
build_c8dump(void)
{
	glob_t g;

	build_librune();
	if (glob("src/c8dump/*.c", 0, globerr, &g))
		die("glob");
	for (size_t i = 0; i < g.gl_pathc; i++)
		mkc(g.gl_pathv[i], (struct strv){});
	mkb("c8dump", g.gl_pathv, g.gl_pathc, (struct strv){}, L_CERR | L_LIBRUNE);
	globfree(&g);
}

void
build_librune(void)
{
	cmd_t c = {0};
	struct strv sv = {0};

	if (librune_built)
		return;
	librune_built = true;

	env_or_default(&sv, "CC", CC);

	if (FLAGSET('f')
	    || foutdated("vendor/librune/make", "vendor/librune/make.c"))
	{
		c.dst = "vendor/librune/make";
		cmdaddv(&c, sv.buf, sv.len);
		cmdadd(&c, "-lpthread", "-o", c.dst, "vendor/librune/make.c");
		CMDPRC2(c);
	}

	if (FLAGSET('f') || !fexists("vendor/librune/librune.a")) {
		cmdadd(&c, "vendor/librune/make");
		if (FLAGSET('f'))
			cmdadd(&c, "-f");
		if (FLAGSET('r'))
			cmdadd(&c, "-r");
		if (FLAGSET('l'))
			cmdadd(&c, "-l");
		CMDPRC(c);
	}
}

int
globerr(const char *s, int e)
{
	errno = e;
	die("glob: %s", s);
}

void
cmdput2(cmd_t c)
{
	const char *p;

	flockfile(stderr);
	for (p = *c._argv; *p; p++)
		fputc(toupper(*p), stderr);
	fprintf(stderr, "\t%s\n", c.dst);
	funlockfile(stderr);
}
