#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <stdckdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <builder.h>
#include <mbstring.h>
#include <SDL.h>

#include "cerr.h"
#include "config.h"
#include "emulator.h"
#include "gui.h"
#include "macros.h"

#define STRTOX(T, SUF)                                                         \
	static T strto##SUF(const char8_t *s, rune *ch)                            \
	{                                                                          \
		T n = 0;                                                               \
		size_t len = strlen(s);                                                \
		while (u8next(ch, &s, &len)) {                                         \
			if (!(*ch >= '0' && *ch <= '9'))                                   \
				return -1;                                                     \
			if (ckd_mul(&n, n, 10) || ckd_add(&n, n, *ch - '0'))               \
				return -1;                                                     \
		}                                                                      \
		*ch = '\0';                                                            \
		return n;                                                              \
	}

STRTOX(int, i)
STRTOX(uint16_t, u16)

[[noreturn]] static void usage(void);
static void run(int, const char *);

struct config cfg = {
	.cpu_hz = 700,
	.vol = 3000,
};
static const char *argv0;

void
usage(void)
{
	fprintf(stderr,
	        "Usage: %s [-S] [-c clock speed] [-s seed] [-v volume] [file]\n"
	        "       %s -h\n",
	        argv0, argv0);
	exit(EXIT_FAILURE);
}

#define NUMERIC_ARG(T, M, FMT, F, DST, N)                                      \
	do {                                                                       \
		rune ch;                                                               \
		T n = F(optarg, &ch);                                                  \
		_Pragma("GCC diagnostic push");                                        \
		_Pragma("GCC diagnostic ignored \"-Wtype-limits\"");                   \
		if (n > M || (ch >= '0' && ch <= '9')) {                               \
			_Pragma("GCC diagnostic pop");                                     \
			warnx(N " too high; may not exceed %" FMT, M);                     \
			usage();                                                           \
		} else if (ch) {                                                       \
			char8_t buf[U8_LEN_MAX];                                           \
			int w = rtou8(buf, ch, sizeof(buf));                               \
			warnx("invalid character ‘%.*s’; " N " must be numeric", w, buf);  \
			usage();                                                           \
		}                                                                      \
		cfg.DST = n;                                                           \
	} while (false)

int
main(int argc, char **argv)
{
	int opt;
	const struct option longopts[] = {
		{"clock-speed", required_argument, nullptr, 'c'},
		{"help",        no_argument,       nullptr, 'h'},
		{"seed",        required_argument, nullptr, 's'},
		{"scanlines",   no_argument,       nullptr, 'S'},
		{"volume",      required_argument, nullptr, 'v'},
		{nullptr,       no_argument,       nullptr, 0  },
	};

	argv0 = argv[0];
	cerrinit(*argv);

	while ((opt = getopt_long(argc, argv, "c:hs:Sv:", longopts, nullptr)) != -1)
	{
		switch (opt) {
		case 'h':
			execlp("man", "man", "1", argv[0], nullptr);
			die("execlp: man 1 %s", argv[0]);
		case 'c':
			NUMERIC_ARG(int, CPUHZMAX, "d", strtoi, cpu_hz, "cpu clock speed");
			if (cfg.cpu_hz < FPS) {
				warnx("cpu clock speed may not be lower than the framerate (%d "
				      "FPS)",
				      FPS);
				usage();
			}
			break;
		case 's':
			NUMERIC_ARG(uint16_t, UINT16_MAX, PRIu16, strtou16, seed, "seed");
			cfg.seeded = true;
			break;
		case 'S':
			cfg.scanls = true;
			break;
		case 'v':
			NUMERIC_ARG(int, VOLMAX, "d", strtoi, vol, "volume");
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	switch (argc) {
	case 0:
		run(STDIN_FILENO, "-");
		break;
	case 1:
		if (streq("-", argv[0]))
			run(STDIN_FILENO, "-");
		else {
			int fd;
			if ((fd = open(argv[0], O_RDONLY)) == -1)
				die("open: %s", argv[0]);
			run(fd, argv[0]);
			close(fd);
		}
		break;
	default:
		usage();
	}

	return EXIT_SUCCESS;
}

void
run(int fd, const char *fn)
{
	char *buf;
	size_t blksize;
	ssize_t nr;
	struct stat st;
	struct u8str sb;

	if (fstat(fd, &st) == -1)
		die("fstat: %s", fn);
	blksize = MAX(st.st_blksize, BUFSIZ);
	if (!(buf = malloc(blksize)))
		die("malloc");

	u8strinit(&sb, S_ISREG(st.st_mode) ? (size_t)st.st_size : blksize);
	while ((nr = read(fd, buf, blksize)) > 0) {
		struct u8view v = {
			.p = buf,
			.len = nr,
		};
		if (!u8strpush(&sb, v))
			die("u8strpush");
	}
	if (nr == -1)
		die("read: %s", fn);

	free(buf);
	wininit();
reset:
	emuinit(u8strtou8(sb), fn);
	windrw();
	auplay(true);

	while (estate != ES_STOP) {
		double dt;
		uint64_t st, et;

		readevnt();
		if (estate == ES_PAUSED)
			continue;
		if (estate == ES_RESET) {
			estate = ES_RUNNING;
			goto reset;
		}

		st = SDL_GetPerformanceCounter();
		for (int i = 0; i < cfg.cpu_hz / FPS; i++)
			emutick();
		et = SDL_GetPerformanceCounter();
		dt = (double)((et - st) * 1000) / SDL_GetPerformanceFrequency();
		SDL_Delay(16.67f > dt ? 16.67f - dt : 0);

		if (c8.needs_redraw)
			windrw();

		if (c8.DT > 0)
			c8.DT--;

		if (c8.ST > 0) {
			c8.ST--;
			auplay(false);
		} else
			auplay(true);
	}

	u8strfree(sb);
	winfree();
}
