#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cerr.h"
#include "macros.h"

static void run(int, const char *);

int
main(int argc, char **argv)
{
	int opt;
	const struct option longopts[] = {
		{"help",  no_argument, nullptr, 'h'},
		{nullptr, no_argument, nullptr, 0  },
	};

	cerrinit(*argv);
	while ((opt = getopt_long(argc, argv, "h", longopts, nullptr)) != -1) {
		switch (opt) {
		case 'h':
			execlp("man", "man", "1", argv[0], nullptr);
			die("execlp: man 1 %s", argv[0]);
		default:
			fprintf(stderr, "Usage: %s [file ...]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc)
		run(STDIN_FILENO, "-");
	for (int i = 0; i < argc; i++) {
		if (streq("-", argv[i]))
			run(STDIN_FILENO, "-");
		else {
			int fd;
			if ((fd = open(argv[i], O_RDONLY)) == -1)
				die("open: %s", argv[i]);
			run(fd, argv[i]);
			close(fd);
		}
	}

	return EXIT_SUCCESS;
}

void
run([[maybe_unused]] int fd, [[maybe_unused]] const char *fn)
{
}
