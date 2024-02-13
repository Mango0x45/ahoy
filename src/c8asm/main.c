#include <sys/stat.h>

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <builder.h>
#include <da.h>

#include "assembler.h"
#include "cerr.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"

static void asmfile(int, const char *);

size_t filesize;
const char *filename;
const char8_t *baseptr;

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
		asmfile(STDIN_FILENO, "-");
	for (int i = 0; i < argc; i++) {
		if (streq("-", argv[i]))
			asmfile(STDIN_FILENO, "-");
		else {
			int fd;
			if ((fd = open(argv[i], O_RDONLY)) == -1)
				die("open: %s", argv[i]);
			asmfile(fd, argv[i]);
			close(fd);
		}
	}

	return EXIT_SUCCESS;
}

void
asmfile(int fd, const char *fn)
{
	char *buf;
	size_t blksize;
	ssize_t nr;
	struct ast ast;
	struct stat st;
	struct u8str sb;
	struct tokens toks;

	filename = fn;

	if (fstat(fd, &st) == -1)
		die("fstat: %s", filename);
	blksize = MAX(st.st_blksize, BUFSIZ);
	if (!(buf = malloc(blksize)))
		die("malloc");

	/* Load the contents of the file into sb */
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
		die("read: %s", filename);

	free(buf);
	filesize = sb.len;
	baseptr = u8strfit(&sb)->p;
	assemble(stdout, ast = parsefile(toks = lexfile(u8strtou8(sb))));

	free(toks.buf);
	free(ast.buf);
	u8strfree(sb);
}
