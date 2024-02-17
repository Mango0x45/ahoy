#ifndef AHOY_AHOY_CONFIG_H
#define AHOY_AHOY_CONFIG_H

#include <stdint.h>

#define FPS      60
#define CPUHZMAX 10'000
#define VOLMAX   10'000

struct config {
	bool scanls;
	bool seeded;
	uint16_t seed;
	int vol;
	int cpu_hz;
};

extern struct config cfg;

#endif /* !AHOY_AHOY_CONFIG_H */
