#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"

#include"Utilities.h"
#include"Constants.h"
#include"FileHandler.h"
#include"KeyboardControler.h"
#include"PlayerControler.h"

int BLACK, GREEN, RED, BLUE, WHITE, SKY;

double CalculateDeltaTime() {
	static int t1 = t1 = SDL_GetTicks();
	int t2 = SDL_GetTicks();
	double deltaTime = (t2 - t1) * 0.001;
	t1 = t2;
	return deltaTime;
}

int SDL_INIT(SDL_Renderer **renderer, SDL_Window **window , SDL_Texture **scrtex , SDL_Surface **screen) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 0;
	}
	int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer);

	if (rc != 0) {
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 0;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(*window, "Szablon do zdania drugiego 2014");
	*screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	*scrtex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	BLACK = SDL_MapRGB((*screen)->format, 0x00, 0x00, 0x00);
	GREEN = SDL_MapRGB((*screen)->format, 0x00, 0xFF, 0x00);
	RED = SDL_MapRGB((*screen)->format, 0xFF, 0x00, 0x00);
	BLUE = SDL_MapRGB((*screen)->format, 0x11, 0x11, 0xCC);
	WHITE = SDL_MapRGB((*screen)->format, 0xFF, 0xFF, 0xFF);
	SKY = SDL_MapRGB((*screen)->format, 0x00, 0xD8, 0xB4);

	return 1;
}

void PlayerInit(Player *mario) {
	mario->livesCounter = MARIO_LIVES;
	mario->gameObject.texture = GetTextureByName(gameTextures, MARIO_IDLE);
	mario->gameObject.size = { 32 , 64 };
	mario->velocity = {};
	mario->gameObject.objectState = ACTIVE_STATE;
	mario->activeLevel = 0;

	mario->animator.moveAnimationFrames = 3;
	mario->animator.moveActiveFrame = 0;
	mario->animator.animationTime = 0;
	mario->animator.movingRight = (Texture**)(malloc(sizeof(Texture) * mario->animator.moveAnimationFrames));
	mario->animator.movingRight[0] = GetTextureByName(gameTextures, MARIO_MOVING_RIGHT_1);
	mario->animator.movingRight[1] = GetTextureByName(gameTextures, MARIO_MOVING_RIGHT_2);
	mario->animator.movingRight[2] = GetTextureByName(gameTextures, MARIO_MOVING_RIGHT_3);

	mario->animator.movingLeft = (Texture**)(malloc(sizeof(Texture) * mario->animator.moveAnimationFrames));
	mario->animator.movingLeft[0] = GetTextureByName(gameTextures, MARIO_MOVING_LEFT_1);
	mario->animator.movingLeft[1] = GetTextureByName(gameTextures, MARIO_MOVING_LEFT_2);
	mario->animator.movingLeft[2] = GetTextureByName(gameTextures, MARIO_MOVING_LEFT_3);

	mario->animator.idle = GetTextureByName(gameTextures, MARIO_IDLE);

	mario->animator.jumpingRight = GetTextureByName(gameTextures, MARIO_JUMP_RIGHT);
	mario->animator.jumpingLeft = GetTextureByName(gameTextures, MARIO_JUMP_LEFT);
}

void DrawGUI(SDL_Surface *screen , Player *mario , double fps , double remainingTime) {
	char infoText[128] = {};
	Texture *charset = GetTextureByName(gameTextures, CHARSET);
	sprintf(infoText, "LEVEL");
	DrawString(screen, SCREEN_WIDTH*0.1, 20, infoText, charset->bitmap);
	sprintf(infoText, "%.5d", mario->activeLevel + 1);
	DrawString(screen, SCREEN_WIDTH*0.1, 40, infoText, charset->bitmap);

	sprintf(infoText, "LIVES");
	DrawString(screen, SCREEN_WIDTH*0.3, 20, infoText, charset->bitmap);
	sprintf(infoText, "%.5d", mario->livesCounter);
	DrawString(screen, SCREEN_WIDTH*0.3, 40, infoText, charset->bitmap);

	sprintf(infoText, "FRAMES");
	DrawString(screen, SCREEN_WIDTH*0.5, 20, infoText, charset->bitmap);
	sprintf(infoText, "%.6d", (int)fps);
	DrawString(screen, SCREEN_WIDTH*0.5, 40, infoText, charset->bitmap);

	sprintf(infoText, "TIME");
	DrawString(screen, SCREEN_WIDTH*0.7, 20, infoText, charset->bitmap);
	sprintf(infoText, "%04.0f", remainingTime);
	DrawString(screen, SCREEN_WIDTH*0.7, 40, infoText, charset->bitmap);

	sprintf(infoText, "COINS");
	DrawString(screen, SCREEN_WIDTH*0.85, 20, infoText, charset->bitmap);
	sprintf(infoText, "%.3d", mario->coins);
	DrawString(screen, SCREEN_WIDTH*0.85, 40, infoText, charset->bitmap);
}

int main(int argc, char **argv) {
	SDL_Surface *screen; SDL_Texture *scrtex; SDL_Window *window; SDL_Renderer *renderer;
	int quit, frames; double gameTime, fpsTimer, fps; SDL_Event event;
	GameState gameState = MENU;

	if(!SDL_INIT(&renderer , &window , &scrtex , &screen)) 	SDL_Quit();

	if (!LoadTextures(gameTextures)){
		//B³¹d wczytywania
		FreeTextures(gameTextures);
		FreeSDL(scrtex, window, renderer);
	}

	Level *levels = LoadLevelsFromFiles(gameTextures);
	if (levels == NULL) {
		// b³¹d wczytywania poziomów
		FreeTextures(gameTextures);
		FreeSDL(scrtex, window, renderer);
	} 

	Player mario;
	PlayerInit(&mario);
	LoadLevel(levels, &mario);
	frames = 0; fpsTimer = 0; fps = 0; quit = 0; gameTime = 0;

	while(!quit) {
		double deltaTime = CalculateDeltaTime();
		KeyboardResponse input = ReadInput();
		switch (input)
		{
		case QUIT:
			quit = 1;
			break;
		case NEW_GAME:
			mario.coins = 0;
			mario.activeLevel = 0;
			mario.livesCounter = MARIO_LIVES;
			gameState = PLAYING;
			LoadLevel(levels, &mario);
			break;
		case SAVE:
			if(gameState == PLAYING)SaveGame(&mario , &levels[mario.activeLevel]);
			break;
		case LOAD:
			gameState = PLAYING;
			LoadGame(&mario , levels);
			break;
		}

		if (gameState == MENU) {
			mario.activeLevel = 0;
			mario.livesCounter = MARIO_LIVES;
			char text[128];
			SDL_FillRect(screen, NULL, BLACK);
			// tekst informacyjny
			char napisy[][128] =
			{ "STEROWANIE" , "N - Nowa Gra" , "S - zapisz" , "L - wczytaj" , "Esc - wyjscie" , "\030 - skok" , "\032 - ruch w lewo" , "\033 - ruch w prawo" , "END"};
			DrawRectangle(screen, SCREEN_WIDTH*0.25 , 0, SCREEN_WIDTH*0.5, SCREEN_HEIGHT, GREEN, BLUE);
			Vector2D textPosition = { SCREEN_WIDTH*0.3, SCREEN_HEIGHT*0.15 };
			for (int i = 0; strcmp(napisy[i] , "END"); i++) {
				sprintf(text, napisy[i]);
				DrawString(screen, textPosition.x, textPosition.y, text, GetTextureByName(gameTextures, CHARSET)->bitmap);
				textPosition.y += 16;
			}
		}
		else if(gameState == PLAYING) {
			levels[mario.activeLevel].remaningTime -= deltaTime;
			if (levels[mario.activeLevel].remaningTime < 0) {
				//skoñczy³ siê czas przeznaczony na aktywny etap
				mario.livesCounter--;
				LoadLevel(levels, &mario);
			}
			
			MovePlayer(&mario, input, deltaTime, levels);
			MoveEnemiesWithinLevel(&levels[mario.activeLevel], deltaTime);

			Collision collision;
			do {
				collision = CheckCollisionWithObstacles(&mario.gameObject, &levels[mario.activeLevel]);
				if (collision.collider == NULL) break;
				CollisionWithObstacle(collision, &mario);

				/* Kolizje specjalne (powoduj¹ce zajœcieje jakiegoœ zdarzenia)*/
				if (collision.collider->texture->textureName == FLAG) {
					mario.activeLevel++;
					if (!LoadLevel(levels, &mario)) { gameState = MENU; }
				}
				else if (collision.direction == UP) {
					if (collision.collider->texture->textureName == PLATFORM_3) {
						collision.collider->oldPosition = collision.collider->position;
						mario.coins++;
						collision.collider->objectState = DRAW_ONLY;
						collision.collider->texture = GetTextureByName(gameTextures, COIN);
					}
					else {
						collision.collider->objectState = INACTIVE_STATE;
						GenerateLevelMap(&levels[mario.activeLevel]);
					}
				}
			} while (true);

			if (mario.livesCounter == 0) {
				gameState = MENU;
				continue;
			}
			if (CheckCollisionWithEnemies(&mario.gameObject, &levels[mario.activeLevel])){
				//kolizja z przeciwnikiem
				mario.livesCounter--;
				LoadLevel(levels, &mario);
			}

			if (mario.gameObject.position.x > mario.camera.position.x) mario.camera.position.x = mario.gameObject.position.x;
			
			AnimateObject(&mario.gameObject, &mario.animator , mario.velocity , deltaTime);
			AnimateLevel(&levels[mario.activeLevel], deltaTime);

			SDL_FillRect(screen, NULL, SKY);

			//Rysowanie Lini granicznych poziomu
			DrawLine(levels[mario.activeLevel].levelMap, 0, 0, SCREEN_HEIGHT, 0, 1, BLACK);
			DrawLine(levels[mario.activeLevel].levelMap, levels[mario.activeLevel].size.x-1, 0, SCREEN_HEIGHT, 0, 1, BLACK);

			//Rysowanie przeciwników
			DrawEnemiesOnMap(&levels[mario.activeLevel], screen , &mario.camera);

			//Rysowanie sceny
			DrawSurface(screen, levels[mario.activeLevel].levelMap, SCREEN_WIDTH / 2 - mario.camera.position.x, 0);

			//Rysowanie interfejsu
			DrawGUI(screen, &mario, fps, levels[mario.activeLevel].remaningTime);

			//Drawing Player
			DrawSurface(screen, mario.gameObject.texture->bitmap, fabs(mario.gameObject.position.x - mario.camera.position.x + SCREEN_WIDTH/2), mario.gameObject.position.y);

			fpsTimer += deltaTime;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};
			frames++;
		}

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

	};

	//Zakoñczenie programu i zwalnianie zasobów
	FreeAnimator(&mario.animator);
	FreeTextures(gameTextures);
	FreeLevels(levels);
	FreeSDL(scrtex, window, renderer);
	return 0;
	};
