#include"KeyboardControler.h"
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
#include"Constants.h"
SDL_Event event;

enum KeyboardResponse ReadInput() {
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			return QUIT;
			break;
		case SDLK_RIGHT:
			return MOVE_RIGHT;
			break;
		case SDLK_LEFT:
			return MOVE_LEFT;
			break;
		case SDLK_UP:
			return JUMP;
			break;
		case SDLK_n:
			return NEW_GAME;
			break;
		case SDLK_s:
			return SAVE;
			break;
		case SDLK_l:
			return LOAD;
			break;
		}
		break;
	case SDL_KEYUP:
		break;
	};
}