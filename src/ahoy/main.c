#include <sys/stat.h>

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <builder.h>
#include <SDL2/SDL.h>

#include "cerr.h"
#include "emulator.h"
#include "gui.h"
#include "macros.h"
#include "SDL_timer.h"

#define FPS           60
#define INSTS_PER_SEC 700

[[noreturn]] static void usage(void);
static void run(int, const char *);

static const char *argv0;

void
usage(void)
{
	fprintf(stderr, "Usage: %s [file]\n", argv0);
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	int opt;
	const struct option longopts[] = {
		{"help",  no_argument, nullptr, 'h'},
		{nullptr, no_argument, nullptr, 0  },
	};

	argv0 = argv[0];
	cerrinit(*argv);

	while ((opt = getopt_long(argc, argv, "h", longopts, nullptr)) != -1) {
		switch (opt) {
		case 'h':
			execlp("man", "man", "1", argv[0], nullptr);
			die("execlp: man 1 %s", argv[0]);
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

static void
update_timers(void)
{
	if (c8.DT > 0)
		c8.DT--;

	if (c8.ST > 0) {
		c8.ST--;
		auplay(false);
	} else
		auplay(true);
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
	emuinit(u8strtou8(sb));

	while (gs != GUI_STOP) {
		double dt;
		uint64_t st, et;

		readkb();
		if (gs == GUI_PAUSED)
			continue;

		st = SDL_GetPerformanceCounter();
		for (int i = 0; i < INSTS_PER_SEC / FPS; i++)
			emutick();
		et = SDL_GetPerformanceCounter();
		dt = (double)((et - st) * 1000) / SDL_GetPerformanceFrequency();
		SDL_Delay(16.67f > dt ? 16.67f - dt : 0);

		// Update window with changes every 60hz
		if (c8.needs_redraw)
			windrw();

		// Update delay & sound timers every 60hz
		update_timers();

		emutick();
	}

	u8strfree(sb);
	winfree();
}
