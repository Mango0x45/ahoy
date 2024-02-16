#include <err.h>
#include <stddef.h>
#include <stdint.h>

#include <SDL.h>

#include "cerr.h"
#include "emulator.h"
#include "gui.h"
#include "macros.h"

#define SCR_SCALE 20
#define SCR_WDTH  64
#define SCR_HIGH  32

#define diesx(fmt) diex(fmt ": %s", SDL_GetError())

static void audio_callback(void *, uint8_t *, int);

guistate gs;
static SDL_Window *win;
static SDL_Renderer *rndr;
static SDL_AudioDeviceID adev;

void
wininit(void)
{
	SDL_AudioSpec want, have;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
		diesx("failed to initialize SDL");

	win = SDL_CreateWindow("Ahoy! CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
	                       SDL_WINDOWPOS_CENTERED, SCR_WDTH * SCR_SCALE,
	                       SCR_HIGH * SCR_SCALE, 0);
	if (!win)
		diesx("failed to create window");

	if (!(rndr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED)))
		diesx("failed to create renderer");

	want = (SDL_AudioSpec){
		.freq = 44100,
		.format = AUDIO_S16LSB,
		.channels = 1,
		.samples = 512,
		.callback = audio_callback,
	};

	if (!(adev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0)))
		warnx("failed to get audio device: %s", SDL_GetError());
	if ((want.format != have.format) || (want.channels != have.channels))
		warnx("failed to get desired audio spec");
}

void
winfree(void)
{
	SDL_DestroyRenderer(rndr);
	SDL_DestroyWindow(win);
	SDL_CloseAudioDevice(adev);
	SDL_Quit();
}

void
windrw(void)
{
	SDL_Rect r = {
		.x = 0,
		.y = 0,
		.w = SCR_SCALE,
		.h = SCR_SCALE,
	};
	static const uint64_t cols[] = {
		63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48,
		47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32,
		31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
		15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
	};

	c8.needs_redraw = false;

	for (size_t i = 0; i < lengthof(c8.screen); i++) {
		if (c8.screen[i])
			goto noclr;
	}
	SDL_SetRenderDrawColor(rndr, 0, 0, 0, UINT8_MAX);
	SDL_RenderClear(rndr);
	return;

noclr:
	for (size_t i = 0; i < lengthof(c8.screen); i++) {
		for (size_t j = 64; j-- > 0;) {
			bool set = ((uint64_t)1 << j) & c8.screen[i];
			r.x = cols[j] * SCR_SCALE;
			r.y = i * SCR_SCALE;
			if (set)
				SDL_SetRenderDrawColor(rndr, 0, UINT8_MAX, 0, UINT8_MAX);
			else
				SDL_SetRenderDrawColor(rndr, 0, 0, 0, UINT8_MAX);
			SDL_RenderFillRect(rndr, &r);
		}
	}

	SDL_RenderPresent(rndr);
}

void
auplay(bool stop)
{
	SDL_PauseAudioDevice(adev, stop);
}

void
readkb(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			gs = GUI_STOP;
			break;

		case SDL_KEYDOWN:
			switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				gs = gs == GUI_RUNNING ? GUI_PAUSED : GUI_RUNNING;
				break;

			// case SDLK_EQUALS:
			// 	init_chip8(chip8, *config, chip8->rom_name);
			// 	break;
			//
			// case SDLK_j:
			// 	// 'j': Decrease color lerp rate
			// 	if (config->color_lerp_rate > 0.1)
			// 		config->color_lerp_rate -= 0.1;
			// 	break;
			//
			// case SDLK_k:
			// 	// 'k': Increase color lerp rate
			// 	if (config->color_lerp_rate < 1.0)
			// 		config->color_lerp_rate += 0.1;
			// 	break;
			//
			// case SDLK_o:
			// 	// 'o': Decrease Volume
			// 	if (config->volume > 0)
			// 		config->volume -= 500;
			// 	break;
			//
			// case SDLK_p:
			// 	// 'p': Increase Volume
			// 	if (config->volume < INT16_MAX)
			// 		config->volume += 500;
			// 	break;

			// Map qwerty keys to CHIP8 keypad
			case SDLK_1:
				c8.kbd[0x1] = true;
				break;
			case SDLK_2:
				c8.kbd[0x2] = true;
				break;
			case SDLK_3:
				c8.kbd[0x3] = true;
				break;
			case SDLK_4:
				c8.kbd[0xC] = true;
				break;
			case SDLK_q:
				c8.kbd[0x4] = true;
				break;
			case SDLK_w:
				c8.kbd[0x5] = true;
				break;
			case SDLK_e:
				c8.kbd[0x6] = true;
				break;
			case SDLK_r:
				c8.kbd[0xD] = true;
				break;
			case SDLK_a:
				c8.kbd[0x7] = true;
				break;
			case SDLK_s:
				c8.kbd[0x8] = true;
				break;
			case SDLK_d:
				c8.kbd[0x9] = true;
				break;
			case SDLK_f:
				c8.kbd[0xE] = true;
				break;
			case SDLK_z:
				c8.kbd[0xA] = true;
				break;
			case SDLK_x:
				c8.kbd[0x0] = true;
				break;
			case SDLK_c:
				c8.kbd[0xB] = true;
				break;
			case SDLK_v:
				c8.kbd[0xF] = true;
				break;
			}
			break;

		case SDL_KEYUP:
			switch (e.key.keysym.sym) {
			case SDLK_1:
				c8.kbd[0x1] = false;
				break;
			case SDLK_2:
				c8.kbd[0x2] = false;
				break;
			case SDLK_3:
				c8.kbd[0x3] = false;
				break;
			case SDLK_4:
				c8.kbd[0xC] = false;
				break;
			case SDLK_q:
				c8.kbd[0x4] = false;
				break;
			case SDLK_w:
				c8.kbd[0x5] = false;
				break;
			case SDLK_e:
				c8.kbd[0x6] = false;
				break;
			case SDLK_r:
				c8.kbd[0xD] = false;
				break;
			case SDLK_a:
				c8.kbd[0x7] = false;
				break;
			case SDLK_s:
				c8.kbd[0x8] = false;
				break;
			case SDLK_d:
				c8.kbd[0x9] = false;
				break;
			case SDLK_f:
				c8.kbd[0xE] = false;
				break;
			case SDLK_z:
				c8.kbd[0xA] = false;
				break;
			case SDLK_x:
				c8.kbd[0x0] = false;
				break;
			case SDLK_c:
				c8.kbd[0xB] = false;
				break;
			case SDLK_v:
				c8.kbd[0xF] = false;
				break;
			}
			break;
		}
	}
}

void
audio_callback(void *, uint8_t *stream, int len)
{
	uint16_t *data = (uint16_t *)stream;
	static uint32_t si;
	const uint32_t half_sqrwv_p = 44100 / 440 / 2;

	/* We are filling out 2 bytes at a time (uint16_t), len is in bytes, so
	   divide by 2.  If the current chunk of audio for the square wave is the
	   crest of the wave, this will add the volume, otherwise it is the trough
	   of the wave, and will add ‘negative’ volume. */
	for (int i = 0; i < len / 2; i++)
		data[i] = ((si++ / half_sqrwv_p) & 1) ? -3000 : +3000;
}
