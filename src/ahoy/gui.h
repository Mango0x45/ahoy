#ifndef AHOY_AHOY_GUI_H
#define AHOY_AHOY_GUI_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
	ES_PAUSED,
	ES_RESET,
	ES_RUNNING,
	ES_STOP,
} emustate;

void wininit(void);
void winfree(void);
void windrw(void);
void auplay(bool);
void readevnt(void);

extern emustate estate;

#endif /* !AHOY_AHOY_GUI_H */
