#include"PlayerControler.h"
#include<math.h>
const Vector2D gravity = { 0 , -400 };
const int jumpVelocity = 250;

int CheckCollision(GameObject *source, GameObject *target) {
	if (target->objectState == DRAW_ONLY) return 0;
	if (target->objectState == INACTIVE_STATE) return 0;
	if (target->position.y > source->position.y + source->size.y) return 0;
	if (source->position.y > target->position.y + target->size.y) return 0;
	if (source->position.x + source->size.x < target->position.x) return 0;
	if (target->position.x + target->size.x < source->position.x) return 0;
	return 1;
}

int CheckCollisionWithEnemies(GameObject *source, Level *level) {
	for (int i = 0; i < level->enemiesAmout; i++) {
		if (CheckCollision(source, &level->enemies[i].gameObject)) return 1;
	}
	return 0;
}

void SetCollisionDirection(GameObject *source, Collision *coll) {
	if (source->oldPosition.y + source->size.y <= coll->collider->position.y &&
		source->position.y + source->size.y >= coll->collider->position.y) {
		coll->direction = DOWN;
		return;
	}
	if (source->oldPosition.y >= coll->collider->position.y + coll->collider->size.y &&
		source->position.y <= coll->collider->position.y + coll->collider->size.y) {
		coll->direction = UP;
		return;
	}
	if (source->oldPosition.x >= coll->collider->position.x + coll->collider->size.x &&
		source->position.x <= coll->collider->position.x + coll->collider->size.x) {
		coll->direction = LEFT;
		return;
	}
	if (source->oldPosition.x + source->size.x <= coll->collider->position.x &&
		source->position.x + source->size.x >= coll->collider->position.x) {
		coll->direction = RIGHT;
		return;
	}
}

Collision CheckCollisionWithObstacles(GameObject *source , Level *level) {
	Collision collision = { NULL , NO_COLLISION };
	for (int i = 0; i < level->obstaclesAmount; i++) {
		if (CheckCollision(source, &level->obstacles[i])){
			collision.collider = &level->obstacles[i];
			SetCollisionDirection(source, &collision);
			return collision;
		}
	}
	return collision;
}

void CollisionWithObstacle(Collision c , Player *mario) {
		if (c.direction == DOWN) {
			mario->gameObject.position.y = c.collider->position.y - mario->gameObject.size.y - 1;
			mario->velocity.y = 0;
			if (mario->gameObject.objectState == JUMPING_STATE) mario->gameObject.objectState = IDLE_STATE;
		}
		else if (c.direction == UP) {
			mario->gameObject.position.y = c.collider->position.y + c.collider->size.y + 1;
			mario->velocity.y = 0;
		}else if (c.direction == LEFT) {
			mario->gameObject.position.x = c.collider->position.x + c.collider->size.x + 1;
			mario->velocity.x = 0;
		}
		else if (c.direction == RIGHT) {
			mario->gameObject.position.x = c.collider->position.x - mario->gameObject.size.x - 1;
			mario->velocity.x = 0;
		}
}

void CheckBounds(Player *mario , Level*levels) {
	Level *level = &levels[mario->activeLevel];
	if (mario->gameObject.position.x + mario->gameObject.size.x > level->size.x) {
		mario->gameObject.position.x = level->size.x - mario->gameObject.size.x;
		mario->velocity.x = 0;
	}
	else if (mario->gameObject.position.x < mario->camera.position.x - SCREEN_WIDTH/2) {
		mario->gameObject.position.x = mario->camera.position.x - SCREEN_WIDTH / 2;
		mario->velocity.x = 0;
	}

	if (mario->gameObject.position.y > level->size.y) {
		mario->gameObject.position.y = level->size.y;
		mario->velocity.y = 0;
		mario->livesCounter--;
		LoadLevel(levels, mario);
	}
	else if (mario->gameObject.position.y < 0) {
		mario->gameObject.position.y = 0;
		mario->velocity.y = 0;
	}
}

void MovePlayer(Player *mario, enum KeyboardResponse input , double deltaTime , Level *level) {
	if (mario->velocity.x > MARIO_SPEED) mario->velocity.x = MARIO_SPEED;
	if (mario->velocity.x < -MARIO_SPEED) mario->velocity.x = -MARIO_SPEED;
	mario->gameObject.oldPosition = mario->gameObject.position;
	mario->gameObject.position = VectorAdd(mario->gameObject.position, VectorMultiply(mario->velocity, -deltaTime));
	mario->velocity = VectorAdd(mario->velocity, VectorMultiply(gravity, deltaTime));

	if (input == MOVE_RIGHT) {
		if (mario->gameObject.objectState != JUMPING_STATE) mario->gameObject.objectState = MOVING_STATE;
		if (mario->velocity.x > 0) mario->velocity.x = 0;
		mario->velocity = VectorAdd(mario->velocity, { MARIO_SPEED * -deltaTime , 0 });
	}
	else if(input == MOVE_LEFT) {
		if (mario->velocity.x < 0) mario->velocity.x = 0;
		if (mario->gameObject.objectState != JUMPING_STATE) mario->gameObject.objectState = MOVING_STATE;
		mario->velocity = VectorAdd(mario->velocity, { MARIO_SPEED * deltaTime , 0 });
	}
	else if (input == JUMP) {
		if (mario->gameObject.objectState != JUMPING_STATE && fabs(mario->gameObject.oldPosition.y - mario->gameObject.position.y) < 1) {
			mario->velocity.y = jumpVelocity;
			mario->gameObject.objectState = JUMPING_STATE;
		}
	}
	else {
		if (mario->gameObject.objectState != JUMPING_STATE) {
			mario->gameObject.objectState = IDLE_STATE;
			mario->velocity.x *= 0.99;
		}
	}
	CheckBounds(mario, level);
}