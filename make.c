#define _GNU_SOURCE
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

static void build_ahoy(void);
static void build_c8asm(void);
static void build_c8dump(void);
static void build_common(void);
static void build_librune(void);
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
		       "-name", "ahoy", "-or", "-name", "c8asm", "-or", "-path",
		       "./src/c8asm/autogen-lookup.h", ")", "-delete");
		CMDPRC(c);
	} else {
		warnx("invalid subcommand -- '%s'", *argv);
		usage();
	}

	return EXIT_SUCCESS;
}

void
build_common(void)
{
	glob_t g;
	char **objs;
	cmd_t c = {0};
	struct strv sv = {0};

	if (glob("src/common/*.c", 0, globerr, &g))
		die("glob");

	objs = bufalloc(nullptr, g.gl_pathc, sizeof(*objs));
	for (size_t i = 0; i < g.gl_pathc; i++) {
		char *s = strdup(g.gl_pathv[i]);
		if (!s)
			die("strdup");
		s[strlen(s) - 1] = 'o';
		objs[i] = s;
	}

	env_or_default(&sv, "CC", CC);
	if (FLAGSET('r'))
		env_or_default(&sv, "CFLAGS", CFLAGS_RLS);
	else
		env_or_default(&sv, "CFLAGS", CFLAGS_DBG);

	for (size_t i = 0; i < g.gl_pathc; i++) {
		if (!FLAGSET('f') && !foutdated(objs[i], g.gl_pathv[i]))
			continue;

		c.dst = objs[i];
		cmdaddv(&c, sv.buf, sv.len);
		cmdadd(&c, "-c", g.gl_pathv[i], "-o", objs[i]);
		CMDPRC2(c);
	}

	globfree(&g);
	strvfree(&sv);
	for (size_t i = 0; i < g.gl_pathc; i++)
		free(objs[i]);
}

void
build_ahoy(void)
{
	glob_t g;
	char **objs;
	cmd_t c = {0};
	struct strv sv = {0}, pc = {0};

	build_librune();

	if (glob("src/ahoy/*.c", 0, globerr, &g))
		die("glob");

	objs = bufalloc(nullptr, g.gl_pathc, sizeof(*objs));
	for (size_t i = 0; i < g.gl_pathc; i++) {
		char *s = strdup(g.gl_pathv[i]);
		if (!s)
			die("strdup");
		s[strlen(s) - 1] = 'o';
		objs[i] = s;
	}

	env_or_default(&sv, "CC", CC);
	if (FLAGSET('r'))
		env_or_default(&sv, "CFLAGS", CFLAGS_RLS);
	else
		env_or_default(&sv, "CFLAGS", CFLAGS_DBG);

	(void)pcquery(&pc, "sdl2", PKGC_CFLAGS);

	for (size_t i = 0; i < g.gl_pathc; i++) {
		if (!FLAGSET('f') && !foutdated(objs[i], g.gl_pathv[i]))
			continue;

		c.dst = objs[i];
		cmdaddv(&c, sv.buf, sv.len);
		cmdadd(&c, "-Isrc/common", "-Ivendor/da", "-Ivendor/librune/include");
		cmdaddv(&c, pc.buf, pc.len);
		cmdadd(&c, "-c", g.gl_pathv[i], "-o", objs[i]);
		CMDPRC2(c);
	}

	if (!FLAGSET('f') && !foutdatedv("ahoy", (const char **)objs, g.gl_pathc))
		goto out;

	strvfree(&sv);
	strvfree(&pc);
	env_or_default(&sv, "CC", CC);
	env_or_default(&sv, "LDFLAGS", nullptr);

	c.dst = "ahoy";
	cmdaddv(&c, sv.buf, sv.len);
	if (pcquery(&pc, "sdl2", PKGC_LIBS))
		cmdaddv(&c, pc.buf, pc.len);
	else
		cmdadd(&c, "-lSDL2");
	cmdadd(&c, "-o", c.dst);
	cmdaddv(&c, objs, g.gl_pathc);
	cmdadd(&c, "src/common/cerr.o", "vendor/librune/librune.a");
	CMDPRC2(c);

out:
	globfree(&g);
	strvfree(&sv);
	strvfree(&pc);
	for (size_t i = 0; i < g.gl_pathc; i++)
		free(objs[i]);
}

void
build_c8asm(void)
{
	glob_t g;
	char **objs;
	cmd_t c = {0};
	struct strv sv = {0};

	if (!binexists("gperf"))
		diex("gperf is required to build c8asm");

	build_librune();

	if (FLAGSET('f')
	    || foutdated("src/c8asm/autogen-lookup.h", "src/c8asm/instr.gperf"))
	{
		c.dst = "src/c8asm/autogen-lookup.h";
		cmdadd(&c, "gperf", "src/c8asm/instr.gperf", "--output-file", c.dst);
		CMDPRC2(c);
	}

	if (glob("src/c8asm/*.c", 0, globerr, &g))
		die("glob");

	objs = bufalloc(nullptr, g.gl_pathc, sizeof(*objs));
	for (size_t i = 0; i < g.gl_pathc; i++) {
		char *s = strdup(g.gl_pathv[i]);
		if (!s)
			die("strdup");
		s[strlen(s) - 1] = 'o';
		objs[i] = s;
	}

	env_or_default(&sv, "CC", CC);
	if (FLAGSET('r'))
		env_or_default(&sv, "CFLAGS", CFLAGS_RLS);
	else
		env_or_default(&sv, "CFLAGS", CFLAGS_DBG);

	for (size_t i = 0; i < g.gl_pathc; i++) {
		if (!FLAGSET('f') && !foutdated(objs[i], g.gl_pathv[i]))
			continue;

		c.dst = objs[i];
		cmdaddv(&c, sv.buf, sv.len);
		cmdadd(&c, "-Isrc/common", "-Ivendor/da", "-Ivendor/librune/include");
		cmdadd(&c, "-c", g.gl_pathv[i], "-o", objs[i]);
		CMDPRC2(c);
	}

	if (!FLAGSET('f') && !foutdatedv("c8asm", (const char **)objs, g.gl_pathc))
		goto out;

	strvfree(&sv);
	env_or_default(&sv, "CC", CC);
	env_or_default(&sv, "LDFLAGS", nullptr);

	c.dst = "c8asm";
	cmdaddv(&c, sv.buf, sv.len);
	cmdadd(&c, "-o", c.dst);
	cmdaddv(&c, objs, g.gl_pathc);
	cmdadd(&c, "src/common/cerr.o", "vendor/librune/librune.a");
	CMDPRC2(c);

out:
	globfree(&g);
	strvfree(&sv);
	for (size_t i = 0; i < g.gl_pathc; i++)
		free(objs[i]);
}

void
build_c8dump(void)
{
	glob_t g;
	char **objs;
	cmd_t c = {0};
	struct strv sv = {0};

	build_librune();

	if (glob("src/c8dump/*.c", 0, globerr, &g))
		die("glob");

	objs = bufalloc(nullptr, g.gl_pathc, sizeof(*objs));
	for (size_t i = 0; i < g.gl_pathc; i++) {
		char *s = strdup(g.gl_pathv[i]);
		if (!s)
			die("strdup");
		s[strlen(s) - 1] = 'o';
		objs[i] = s;
	}

	env_or_default(&sv, "CC", CC);
	if (FLAGSET('r'))
		env_or_default(&sv, "CFLAGS", CFLAGS_RLS);
	else
		env_or_default(&sv, "CFLAGS", CFLAGS_DBG);

	for (size_t i = 0; i < g.gl_pathc; i++) {
		if (!FLAGSET('f') && !foutdated(objs[i], g.gl_pathv[i]))
			continue;

		c.dst = objs[i];
		cmdaddv(&c, sv.buf, sv.len);
		cmdadd(&c, "-Isrc/common", "-Ivendor/da", "-Ivendor/librune/include");
		cmdadd(&c, "-c", g.gl_pathv[i], "-o", objs[i]);
		CMDPRC2(c);
	}

	if (!FLAGSET('f') && !foutdatedv("c8dump", (const char **)objs, g.gl_pathc))
		goto out;

	strvfree(&sv);
	env_or_default(&sv, "CC", CC);
	env_or_default(&sv, "LDFLAGS", nullptr);

	c.dst = "c8dump";
	cmdaddv(&c, sv.buf, sv.len);
	cmdadd(&c, "-o", c.dst);
	cmdaddv(&c, objs, g.gl_pathc);
	cmdadd(&c, "src/common/cerr.o", "vendor/librune/librune.a");
	CMDPRC2(c);

out:
	globfree(&g);
	strvfree(&sv);
	for (size_t i = 0; i < g.gl_pathc; i++)
		free(objs[i]);
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
