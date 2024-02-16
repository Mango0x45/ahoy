#ifndef AHOY_AHOY_CONFIG_H
#define AHOY_AHOY_CONFIG_H

#include <stdint.h>

struct config {
	/* Valid ranges are 0–UINT16_MAX.  >UINT16_MAX is for random seed. */
	uint32_t seed;
	unsigned cpu_hz;
};

extern struct config cfg;

#endif /* !AHOY_AHOY_CONFIG_H */
