#include"Utilities.h"
#include"Constants.h"
#include"FileHandler.h"

Texture gameTextures[GAME_TEXTURES_NUMBER] = {
	{ NULL , MARIO_IDLE , "mario_idle.bmp" },
	{ NULL , MARIO_JUMP_RIGHT , "mario_jump_right.bmp" },
	{ NULL , MARIO_JUMP_LEFT , "mario_jump_left.bmp" },
	{ NULL , MARIO_MOVING_RIGHT_1 , "mario_moving_right_1.bmp" },
	{ NULL , MARIO_MOVING_RIGHT_2 , "mario_moving_right_2.bmp" },
	{ NULL , MARIO_MOVING_RIGHT_3 , "mario_moving_right_3.bmp" },
	{ NULL , MARIO_MOVING_LEFT_1 , "mario_moving_left_1.bmp" },
	{ NULL , MARIO_MOVING_LEFT_2 , "mario_moving_left_2.bmp" },
	{ NULL , MARIO_MOVING_LEFT_3 , "mario_moving_left_3.bmp" },
	{ NULL , GROUND , "ground.bmp" },
	{ NULL , PLATFORM , "platform.bmp" },
	{ NULL , FLAG , "flag.bmp" },
	{ NULL , CHARSET , "cs32x32.bmp" },
	{ NULL , ENEMY_MOVING_1 , "enemy_moving_1.bmp" },
	{ NULL , ENEMY_MOVING_2 , "enemy_moving_2.bmp" },
	{ NULL , ENEMY_MOVING_3 , "enemy_moving_3.bmp" },
	{ NULL , PLATFORM_2 , "platform_2.bmp" },
	{ NULL , TREE_1 , "tree_1.bmp" },
	{ NULL , CLOUD_1 , "cloud_1.bmp" },
	{ NULL , PLATFORM_3, "platform_with_coin.bmp" },
	{ NULL , COIN , "coin.bmp" }

};

Texture* GetTextureByName(Texture gameTextures[] ,enum TextureNames name) {
	for (int i = 0; i < GAME_TEXTURES_NUMBER; i++) {
		if (gameTextures[i].textureName == name) return &gameTextures[i];
	}
	return NULL;
};

void GenerateLevelMap(Level *level) {
	if (level->levelMap != NULL) {
		SDL_FreeSurface(level->levelMap);
	}
	level->levelMap = SDL_CreateRGBSurface(0, level->size.x, level->size.y, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	for (int i = 0; i < level->obstaclesAmount; i++){
		if (level->obstacles[i].objectState == INACTIVE_STATE) continue;
		DrawSurface(level->levelMap, level->obstacles[i].texture->bitmap, level->obstacles[i].position.x, level->obstacles[i].position.y);
	}
}

void DrawGameObjectOnSurface(GameObject *gameObject , SDL_Surface *surface , GameObject* camera) {
	if (camera != NULL) {
		DrawSurface(surface, gameObject->texture->bitmap, SCREEN_WIDTH / 2 - camera->position.x +  gameObject->position.x, gameObject->position.y);
	}
	else {
		DrawSurface(surface, gameObject->texture->bitmap, gameObject->position.x, gameObject->position.y);
	}
}

void DrawEnemiesOnMap(Level *level, SDL_Surface *map , GameObject* camera) {
	for (int i = 0; i < level->enemiesAmout; i++) {
		DrawGameObjectOnSurface(&level->enemies[i].gameObject, map , camera);
	}
}

void MoveEnemiesWithinLevel(Level *level , double deltaTime) {
	for (int i = 0; i < level->enemiesAmout; i++) {
		Enemy *enemy = &level->enemies[i];
		if (enemy->gameObject.position.x > enemy->endPoint.x) {
			enemy->gameObject.position = enemy->endPoint;
			enemy->speed *= -1;
		}else if (enemy->gameObject.position.x < enemy->startPoint.x) {
			enemy->gameObject.position = enemy->startPoint;
			enemy->speed *= -1;
		}
		enemy->gameObject.position.x += enemy->speed* deltaTime;
	}
}

Vector2D VectorAdd(Vector2D a, Vector2D b) {
	return{ a.x + b.x , a.y + b.y };
}

Vector2D VectorMultiply(Vector2D a, Vector2D b) {
	return{ a.x * b.x , a.y * b.y };
}

Vector2D VectorAdd(Vector2D a, double b) {
	return{ a.x + b , a.y + b };
}

Vector2D VectorMultiply(Vector2D a, double b) {
	return{ a.x * b , a.y * b };
}

/* Zwraca 1 jeúli istnieje nastÍpny etap i 0 jeúli nie istnieje*/
int LoadLevel(Level *levels, Player *mario) {
	mario->camera.position = { SCREEN_WIDTH / 2  , SCREEN_HEIGHT / 2 };
	if (mario->activeLevel == levelsAmount) {
		mario->activeLevel = 0;
		return 0;
	}
	levels[mario->activeLevel].remaningTime = levels[mario->activeLevel].time;
	for(int i = 0; i < levels[mario->activeLevel].obstaclesAmount; i++){
		if (levels[mario->activeLevel].obstacles[i].objectState == INACTIVE_STATE) {
			levels[mario->activeLevel].obstacles[i].objectState = ACTIVE_STATE;
			if (levels[mario->activeLevel].obstacles[i].texture->textureName == COIN) {
				levels[mario->activeLevel].obstacles[i].position = levels[mario->activeLevel].obstacles[i].oldPosition;
				levels[mario->activeLevel].obstacles[i].texture = GetTextureByName(gameTextures, PLATFORM_3);
			}
		}
	}
	GenerateLevelMap(&levels[mario->activeLevel]);
	mario->gameObject.position = levels[mario->activeLevel].playerSpawnPosition;
	mario->velocity = {};
	return 1;
}

void AnimateObject(GameObject *gameObject, Animator *animator, Vector2D velocity, double deltaTime) {
	animator->animationTime += deltaTime;
	switch (gameObject->objectState)
	{
	case JUMPING_STATE:
		if (velocity.x <= 0) gameObject->texture = animator->jumpingRight;
		else gameObject->texture = animator->jumpingLeft;
		break;
	case MOVING_STATE:
		if (animator->animationTime > 0.1) {
			animator->animationTime = 0;
			animator->moveActiveFrame++;
			if (animator->moveActiveFrame == animator->moveAnimationFrames) animator->moveActiveFrame = 0;
		}
		if (velocity.x <= 0) gameObject->texture = animator->movingRight[animator->moveActiveFrame];
		else gameObject->texture = animator->movingLeft[animator->moveActiveFrame];

		break;
	case IDLE_STATE:
		gameObject->texture = animator->idle;
		break;
	}
}

void AnimateLevel(Level *level, double deltaTime) {
	int animSpeed = 200;
	for (int i = 0; i < level->enemiesAmout; i++){
		AnimateObject(&level->enemies[i].gameObject, &level->enemies[i].animator, { (double)level->enemies[i].speed , 0 }, deltaTime);
	}
	for (int i = 0; i < level->obstaclesAmount; i++) {
		if (level->obstacles[i].texture->textureName == COIN && level->obstacles[i].objectState == DRAW_ONLY) {

			level->obstacles[i].velocityY += animSpeed * deltaTime;
			level->obstacles[i].position.y += level->obstacles[i].velocityY * deltaTime;
			if (level->obstacles[i].position.y > SCREEN_HEIGHT) {
				level->obstacles[i].objectState = INACTIVE_STATE;
				level->obstacles[i].velocityY = ANIMATION_GRAVITY;
			}
			GenerateLevelMap(level);
		}
	}
}

void FreeSDL(SDL_Texture *srctex, SDL_Window *window, SDL_Renderer *renderer) {
	SDL_DestroyTexture(srctex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}
void FreeAnimator(Animator *animator){
	free(animator->movingLeft);
	free(animator->movingRight);
}
void FreeTextures(Texture gameTextures[]) {
	for (int i = 0; i < GAME_TEXTURES_NUMBER; i++) {
		SDL_FreeSurface(gameTextures[i].bitmap);
		gameTextures[i].bitmap = NULL;
	}
}
void FreeLevels(Level *levels) {
	for (int i = 0; i < levelsAmount; i++) {
		free(levels[i].obstacles);
		for (int j = 0; j < levels[i].enemiesAmout; j++) {
			FreeAnimator(&levels[i].enemies[j].animator);
		}
		free(levels[i].enemies);
	}
}

/*** FUNKCJE SZABLONU ***/

// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
// charset to bitmapa 128x128 zawierajπca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 16;
	s.h = 16;
	d.w = 16;
	d.h = 16;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 16;
		py = (c / 16) * 16;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 16;
		text++;
	};
};

// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);

};

// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};

// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

// rysowanie prostokπta o d≥ugoúci bokÛw l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};
