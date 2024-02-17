#ifndef AHOY_AHOY_CONFIG_H
#define AHOY_AHOY_CONFIG_H

#include <stdint.h>

#define VOLMAX 10'000

struct config {
	bool scanls;
	bool seeded;
	uint16_t seed;
	unsigned cpu_hz;
	int vol;
};

extern struct config cfg;

#endif /* !AHOY_AHOY_CONFIG_H */
