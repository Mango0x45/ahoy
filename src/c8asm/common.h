#ifndef AHOY_C8ASM_COMMON_H
#define AHOY_C8ASM_COMMON_H

#include <rune.h>

#define DIE_AT_POS_WITH_CODE(HL, P, ...) \
	die_at_pos_with_code(filename, filebuf, (HL), (P)-baseptr, __VA_ARGS__);

extern size_t filesize;
extern const char *filename;
extern const char8_t *baseptr;
extern struct u8view filebuf;

#endif /* !AHOY_C8ASM_COMMON_H */
