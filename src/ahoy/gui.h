#ifndef AHOY_AHOY_GUI_H
#define AHOY_AHOY_GUI_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
	GUI_RUNNING,
	GUI_PAUSED,
	GUI_STOP,
} guistate;

void wininit(void);
void winfree(void);
void winclr(void);
void windrw(void);
void auplay(bool);
void readkb(void);

extern guistate gs;

#endif /* !AHOY_AHOY_GUI_H */
