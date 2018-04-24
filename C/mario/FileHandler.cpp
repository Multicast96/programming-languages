#include"FileHandler.h"
#include<string.h>
#include<stdio.h>

const char texturesFolder[] = "textures/";
const char save_name[] = "super_mario.save"; // nazwa pliku zapisu gry
int levelsAmount;

int GetLvlAmount() {
	return levelsAmount;
}

int LoadTextures(Texture gameTextures[]) {
	for (int i = 0; i < GAME_TEXTURES_NUMBER; i++) {
		char filePath[128] = {};
		strcat(filePath, texturesFolder);
		strcat(filePath , gameTextures[i].fileName);
		gameTextures[i].bitmap = SDL_LoadBMP(filePath);
		if (gameTextures[i].bitmap == NULL) {
			printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
			return 0;
		}
		SDL_SetColorKey(gameTextures[i].bitmap, 1, 0xFF00FF);
	}
	return 1;
}

int GetLevelsAmount() {
	char levelName[128] = {};
	int lvlAmount = 0;
	while (true){
		sprintf(levelName, "etap%d.txt", lvlAmount);
		FILE *file = fopen(levelName, "r");
		if (file == NULL) break;
		fclose(file);
		lvlAmount++;
	}
	return lvlAmount;
}

void CreateObject(Level *level, int index, Vector2D position, Vector2D size, Texture *texture) {
	level->obstacles[index].position = position;
	level->obstacles[index].size = size;
	level->obstacles[index].texture = texture;
	if (size.x == 0 && size.y == 0) {
		level->obstacles[index].objectState = DRAW_ONLY;
	}else level->obstacles[index].objectState = ACTIVE_STATE;
}

int GenerateGround(Level *level, FILE *file , Texture gameTextures[]) {
	int positionX = 0;
	int currentObstacle = 0;
	while (true)
	{
		char c = fgetc(file);
		if (c == '\n') break;
		if (c == 'G') {
			int gNumber = 0;
			fscanf(file, "%d", &gNumber);
			for (int j = 0; j < gNumber; j++) {
				CreateObject(level, currentObstacle, { (double)positionX , SCREEN_HEIGHT - BLOCK_SIZE }, { BLOCK_SIZE , BLOCK_SIZE }, GetTextureByName(gameTextures, GROUND));
				currentObstacle++;
				CreateObject(level, currentObstacle, { (double)positionX , SCREEN_HEIGHT - 2 * BLOCK_SIZE }, { BLOCK_SIZE , BLOCK_SIZE }, GetTextureByName(gameTextures, GROUND));
				currentObstacle++;
				positionX += BLOCK_SIZE;
			}
		}
		if (c == 'H') {
			positionX += BLOCK_SIZE * 2; // rozmiar przerwy w pod³o¿u
		}
	}
	return currentObstacle;
}

int GeneratePlatform(int currentObstacle, Level *level, Texture *texture , int size , Vector2D position) {
	for (int i = 0; i < size; i++, currentObstacle++) {
		level->obstacles[currentObstacle].position = { position.x + BLOCK_SIZE * i , position.y };
		level->obstacles[currentObstacle].objectState = ACTIVE_STATE;
		level->obstacles[currentObstacle].size = { BLOCK_SIZE , BLOCK_SIZE };
		level->obstacles[currentObstacle].texture = texture;
		level->obstacles[currentObstacle].velocityY = ANIMATION_GRAVITY;

	}
	return currentObstacle;
}

Level* LoadLevelsFromFiles(Texture gameTextures[]) {
	levelsAmount = GetLevelsAmount();
	if (levelsAmount == 0) return NULL;

	Level *levels = (Level*)malloc(sizeof(Level) * levelsAmount);
	if (levels == NULL) return NULL; // B³¹d alokacji

	for (int i = 0; i < levelsAmount; i++) {
		char levelName[128] = {};
		sprintf(levelName, "etap%d.txt", i);
		FILE *file = fopen(levelName, "rt");
		fscanf(file, "%lf", &levels[i].time);
		fscanf(file, "%lf %lf\n" , &levels[i].size.x, &levels[i].size.y);
		fscanf(file, "%lf %lf\n", &levels[i].playerSpawnPosition.x, &levels[i].playerSpawnPosition.y);
		fscanf(file, "%d %d\n" , &levels[i].obstaclesAmount , &levels[i].enemiesAmout);

		levels[i].obstacles = (GameObject*)malloc(sizeof(GameObject) * levels[i].obstaclesAmount);
		levels[i].enemies = (Enemy*)(malloc(sizeof(Enemy) * levels[i].enemiesAmout));
		levels[i].levelMap = NULL;


		// Tworzenie pod³o¿a
		int currentObstacle = GenerateGround(&levels[i], file, gameTextures);
		int currentEnemy = 0;

		int textureId = 0; Vector2D objectPosition = {}; Vector2D objectSize = {};

		//Tworzenie dodatkowych obiektów
		while (fscanf(file, "(%d %lf %lf %lf %lf)", &textureId, &objectPosition.x, &objectPosition.y, &objectSize.x, &objectSize.y) > 0) {
			CreateObject(&levels[i], currentObstacle, objectPosition, objectSize, GetTextureByName(gameTextures, (enum TextureNames)textureId));
			currentObstacle++;
		}

		fgetc(file);

		//Tworzenie platform
		while (fscanf(file, "{%d %lf %lf %lf}", &textureId , &objectSize.x, &objectPosition.x , &objectPosition.y) > 0) {
			currentObstacle = GeneratePlatform(currentObstacle, &levels[i], GetTextureByName(gameTextures, (enum TextureNames)textureId), objectSize.x , objectPosition);
		}
		fgetc(file);

		//Tworzenie przeciwników
		Vector2D startPoint = {}, endPoint = {};
		int speed;
		while (fscanf(file, "<%d %lf %lf %lf %lf>" , &speed , &startPoint.x , &startPoint.y  , &endPoint.x , &endPoint.y) > 0) {
			Enemy *enemy = &levels[i].enemies[currentEnemy];
			enemy->speed = speed;
			enemy->startPoint = startPoint;
			enemy->endPoint = endPoint;
			enemy->gameObject.objectState = MOVING_STATE;
			enemy->gameObject.position = startPoint;
			enemy->gameObject.size = { BLOCK_SIZE , BLOCK_SIZE };
			enemy->gameObject.texture = GetTextureByName(gameTextures, ENEMY_MOVING_2);

			enemy->animator.moveAnimationFrames = 3;
			enemy->animator.moveActiveFrame = 0;
			enemy->animator.animationTime = 0;
			enemy->animator.movingRight = (Texture**)(malloc(sizeof(Texture) * enemy->animator.moveAnimationFrames));
			enemy->animator.movingRight[0] = GetTextureByName(gameTextures, ENEMY_MOVING_1);
			enemy->animator.movingRight[1] = GetTextureByName(gameTextures, ENEMY_MOVING_2);
			enemy->animator.movingRight[2] = GetTextureByName(gameTextures, ENEMY_MOVING_3);

			enemy->animator.movingLeft = (Texture**)(malloc(sizeof(Texture) * enemy->animator.moveAnimationFrames));
			enemy->animator.movingLeft[0] = GetTextureByName(gameTextures, ENEMY_MOVING_1);
			enemy->animator.movingLeft[1] = GetTextureByName(gameTextures, ENEMY_MOVING_2);
			enemy->animator.movingLeft[2] = GetTextureByName(gameTextures, ENEMY_MOVING_3);

			currentEnemy++;
		}
		fclose(file);
		GenerateLevelMap(&levels[i]);
	}


	return levels;
}

void SaveGame(Player *mario , Level *level) {
	FILE *file = fopen(save_name, "wt");
	fprintf(file, "%d %d %lf\n", mario->coins, mario->activeLevel , level->remaningTime);
	fprintf(file, "%d %lf %lf\n", mario->livesCounter, mario->gameObject.position.x, mario->gameObject.position.y);
	for (int i = 0; i < level->obstaclesAmount; i++) {
		if (level->obstacles[i].objectState == INACTIVE_STATE || level->obstacles[i].texture->textureName == COIN) {
			fprintf(file, "{%d}", i);
		}
	}
	fclose(file);
}

void LoadGame(Player *mario , Level *levels) {
	FILE *file = fopen(save_name, "rt");
	fscanf(file, "%d %d" , &mario->coins, &mario->activeLevel);
	LoadLevel(levels, mario);
	fscanf(file, "%lf", &levels[mario->activeLevel].remaningTime);
	fscanf(file, "%d %lf %lf\n", &mario->livesCounter, &mario->gameObject.position.x, &mario->gameObject.position.y);
	int index;
	while (fscanf(file, "{%d}", &index) > 0) {
		levels[mario->activeLevel].obstacles[index].objectState = INACTIVE_STATE;
	}
	GenerateLevelMap(&levels[mario->activeLevel]);
	fclose(file);
}