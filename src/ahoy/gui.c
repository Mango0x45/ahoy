#include <err.h>

#include <SDL2/SDL.h>

#include "cerr.h"

#define SCR_SCALE 10
#define SCR_WDTH  64
#define SCR_HIGH  32

#define diesx(fmt, ...) \
	diex(fmt ": %s" __VA_OPT__(,) __VA_ARGS__, SDL_GetError())

SDL_Window *win;
SDL_Renderer *rndr;
SDL_Texture *txtr;
SDL_AudioDeviceID adev;
unsigned long asmpcnt;
struct {
	void *p;
	size_t sz;
} abuf;

void
wininit(void)
{
	SDL_AudioSpec have;
	SDL_AudioSpec want = {
		.freq = 64 * 60,
		.format = AUDIO_F32,
		.channels = 1,
		.samples = 64,
	};

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		diesx("failed to initialize SDL");

	win = SDL_CreateWindow("Ahoy!", SDL_WINDOWPOS_CENTERED,
	                       SDL_WINDOWPOS_CENTERED, SCR_WDTH * SCR_SCALE,
	                       SCR_HIGH * SCR_SCALE, SDL_WINDOW_RESIZABLE);
	if (!win)
		diesx("failed to create window");

	rndr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED
	                                 | SDL_RENDERER_PRESENTVSYNC);
	if (!rndr)
		diesx("failed to create renderer");

	txtr = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888,
	                         SDL_TEXTUREACCESS_STREAMING, SCR_WDTH, SCR_HIGH);
	if (!txtr)
		diesx("failed to create texture");

	adev = SDL_OpenAudioDevice(nullptr, 0, &want, &have,
	                           SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (!adev) {
		warnx("failed to open audio device: %s", SDL_GetError());
		return;
	}

	asmpcnt = have.samples * have.channels;
	abuf.sz = asmpcnt * 4;
	if (!(abuf.p = malloc(abuf.sz)))
		die("malloc");
	SDL_PauseAudioDevice(adev, 0);
}

void
winfree(void)
{
}
