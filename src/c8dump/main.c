#include <sys/stat.h>

#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <builder.h>

#include "cerr.h"
#include "macros.h"

#define MEM_RESERVED 0x200
#define MEM_TOTAL    0xFFF
#define LABEL_BUFSIZ (sizeof("label_4095"))

static void dumpfile(int, const char *);
static const char *getlabel(uint16_t);

static uint16_t labelsfxs[MEM_TOTAL - MEM_RESERVED + 1];

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
			fprintf(stderr,
			        "Usage: %s [file ...]\n"
			        "       %s -h\n",
			        argv[0], argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc)
		dumpfile(STDIN_FILENO, "-");
	for (int i = 0; i < argc; i++) {
		if (streq("-", argv[i]))
			dumpfile(STDIN_FILENO, "-");
		else {
			int fd;
			if ((fd = open(argv[i], O_RDONLY)) == -1)
				die("open: %s", argv[i]);
			dumpfile(fd, argv[i]);
			close(fd);
		}
	}

	return EXIT_SUCCESS;
}

void
dumpfile(int fd, const char *filename)
{
	char *buf;
	size_t blksize;
	ssize_t nr;
	struct stat st;
	struct u8str sb;

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

	if (sb.len & 1) {
		diex("%s: binary size (%zu B) is odd; missing padding bytes?", filename,
		     sb.len);
	}

	for (size_t i = 0, j = 0; i < sb.len; i += 2) {
		uint16_t op = (sb.p[i] << 8) | sb.p[i + 1];
		switch (op >> 12) {
		case 0x0:
		case 0x1:
		case 0x2:
		case 0xA:
		case 0xB:;
			uint16_t addr = op & 0xFFF;
			if (op != 0x00E0 && op != 0x00EE && addr >= MEM_RESERVED
			    && !labelsfxs[addr - MEM_RESERVED])
			{
				labelsfxs[addr - MEM_RESERVED] = ++j;
			}
		}
	}

	for (size_t i = 0; i < sb.len; i += 2) {
		uint16_t op = (sb.p[i] << 8) | sb.p[i + 1];
		static bool indent;

		if (i < lengthof(labelsfxs) && labelsfxs[i]) {
			printf("%s:\n", getlabel(i + MEM_RESERVED));
			indent = true;
		}
		if (indent)
			putchar('\t');

		switch (op >> 12) {
		case 0x0:
		case 0x1:
		case 0x2:
		case 0xA:
		case 0xB:;
			static const char *ops_1[] = {
				[0x0] = "sys",  [0x1] = "jp",    [0x2] = "call",
				[0xA] = "ld i", [0xB] = "jp v0",
			};
			if (op == 0x00E0)
				puts("cls");
			else if (op == 0x00EE)
				puts("ret");
			else
				printf("%s %s\n", ops_1[op >> 12], getlabel(op & 0xFFF));
			break;

		case 0x3:
		case 0x4:
		case 0x6:
		case 0x7:
		case 0xC:;
			static const char *ops_2[] = {
				[0x3] = "se",  [0x4] = "sne", [0x6] = "ld",
				[0x7] = "add", [0xC] = "rnd",
			};
			printf("%s v%x 0x%02X\n", ops_2[op >> 12], (op >> 8) & 0xF,
			       op & 0xFF);
			break;

		case 0x5:
		case 0x8:
		case 0x9:
		case 0xD:;
			uint8_t n1, n2, n3, n4, i;
			static const char *ops_3[0xFF + 1] = {
				[0x50] = "se",   [0x80] = "ld",  [0x81] = "or",  [0x82] = "and",
				[0x83] = "xor",  [0x84] = "add", [0x85] = "sub", [0x86] = "shr",
				[0x87] = "subn", [0x8E] = "shl", [0x90] = "sne",
			};

			n1 = op >> 12;
			n2 = (op >> 8) & 0xF;
			n3 = (op >> 4) & 0xF;
			n4 = (op >> 0) & 0xF;
			i = (n1 << 4) | n4;

			if (n1 == 0xD)
				printf("drw v%x v%x %u\n", n2, n3, n4);
			else if (!ops_3[i])
				goto print_bytes;
			else {
				printf(n4 == 0x6 || n4 == 0xE ? "%s v%x\n" : "%s v%x v%x\n",
				       ops_3[i], n2, n3);
			}
			break;

		case 0xE:
			if (op & 0xFF != 0x9E)
				goto print_bytes;
			printf("skp v%x\n", (op >> 12) & 0xF);
			break;

		case 0xF:;
			uint8_t x = (op >> 8) & 0xF;
			switch (op & 0xFF) {
			case 0x07:
				printf("ld v%x dt\n", x);
				break;
			case 0x0A:
				printf("ld v%x k\n", x);
				break;
			case 0x15:
				printf("ld dt v%x\n", x);
				break;
			case 0x18:
				printf("ld st v%x\n", x);
				break;
			case 0x1E:
				printf("add i v%x\n", x);
				break;
			case 0x29:
				printf("hex v%x\n", x);
				break;
			case 0x33:
				printf("bcd v%x\n", x);
				break;
			case 0x55:
				printf("stor v%x\n", x);
				break;
			case 0x65:
				printf("rstr v%x\n", x);
				break;
			default:
				goto print_bytes;
			}
			break;

		default:
print_bytes:
			printf("db 0x%02X\n%sdb 0x%02X\n", op >> 8, indent ? "\t" : "",
			       op & 0xFF);
		}
	}

	u8strfree(sb);
}

const char *
getlabel(uint16_t addr)
{
	static char buf[LABEL_BUFSIZ + 69];
	if (addr < MEM_RESERVED)
		snprintf(buf, sizeof(buf), "0x%03X", addr);
	else {
		snprintf(buf, sizeof(buf), "label_%" PRIu16,
		         labelsfxs[addr - MEM_RESERVED]);
	}
	return buf;
}
