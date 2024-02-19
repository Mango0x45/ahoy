#ifndef AHOY_C8ASM_LEXER_H
#define AHOY_C8ASM_LEXER_H

#include <mbstring.h>

typedef enum [[clang::flag_enum]] {
	T_COLON = 1 << 0,
	T_EOL = 1 << 1,
	T_IDENT = 1 << 2,
	T_NUMBER = 1 << 3,
	T_STRING = 1 << 4,
} tokkind;

struct token {
	tokkind kind;
	struct u8view sv;
	int base; /* For number literals */
};

struct tokens {
	struct token *buf;
	size_t len, cap;
};

const char *tokrepr(tokkind);
struct tokens lexfile();

#endif /* !AHOY_C8ASM_LEXER_H */
