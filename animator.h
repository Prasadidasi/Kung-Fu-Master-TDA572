#pragma once
#include <map>

class Animator {
	int frameCounter;
	int totalFrames;
	float deathSpeed;
	bool flip;

	Engine* engine;
	Camera* camera;
	Sprite* spriteSheet;
	Sprite* idleSprite;
	SDL_Rect textureRect;
	const int player_height = 63;

	std::map<SDL_KeyCode, Sprite*> playerSprites;
	std::map<SDL_KeyCode, Sprite*>::iterator playeritr;

	std::map<char*, Sprite*> enemySprites;
	std::map<char*, Sprite*>::iterator enemyItr;

	std::map<Sprite*, std::pair<int, SDL_Rect>> spriteAttrib;
	std::map<Sprite*, std::pair<int, SDL_Rect>>::iterator itrAtb;

public:
	float dt;
	int faceDirection;

	void Create(Engine* engine, Camera* camera, const char* sprite_name) {
		flip = false;
		frameCounter = 0;
		deathSpeed = 5.f;
		this->engine = engine;
		this->camera = camera;
		engine->getFaceDirection(faceDirection);

		idleSprite = engine->createSprite(sprite_name);
		textureRect.x = 0;
		textureRect.y = 0;
		idleSprite->getDimensions(&textureRect);
	}

	void AddSpriteSheet(Engine* engine, const char* sprite_name, int totalFrames, char* identifier) {
		Sprite* sprite = engine->createSprite(sprite_name);
		SDL_Rect texRect;
		texRect.x = 0;
		texRect.y = 0;
		sprite->getDimensions(&texRect);
		texRect.w /= totalFrames;

		std::pair<int, SDL_Rect>attribPair(totalFrames, texRect);
		enemySprites.insert(std::pair<char*, Sprite*>(identifier, sprite));
		spriteAttrib.insert(std::make_pair(sprite, attribPair));
	}

	void AddSpriteSheet(Engine* engine, const char* sprite_name, int totalFrames, SDL_KeyCode direction) {
		Sprite* sprite = engine->createSprite(sprite_name);
		SDL_Rect texRect;
		texRect.x = 0;
		texRect.y = 0;
		sprite->getDimensions(&texRect);
		texRect.w /= totalFrames;

		std::pair<int, SDL_Rect>attribPair(totalFrames, texRect);
		playerSprites.insert(std::pair<SDL_KeyCode, Sprite*>(direction, sprite));
		spriteAttrib.insert(std::make_pair(sprite, attribPair));
	}

	char* getEnemyState(Enemy* enemy, Player* player) {
		if (enemy->grabPlayer)
			return enemy->animIds[4];	//Grab

		if (abs(player->position.x - enemy->position.x) > 200)
			return enemy->animIds[0];	//Walk
		else
			return enemy->animIds[1];	//Attack
	}

	void getEnemySprite(char* enemyId) {
		enemyItr = enemySprites.find(enemyId);
		if (enemyItr == enemySprites.end())
			return;

		spriteSheet = enemyItr->second;
		itrAtb = spriteAttrib.find(spriteSheet);
		textureRect = itrAtb->second.second;
		totalFrames = itrAtb->second.first;
	}

	void getPlayerSprite(SDL_KeyCode direction) {
		playeritr = playerSprites.find(direction);
		spriteSheet = playeritr->second;
		itrAtb = spriteAttrib.find(spriteSheet);
		totalFrames = itrAtb->second.first;
		textureRect = itrAtb->second.second;
	}

	void enemyHit(Camera* camera, GameObject* go, char* enemyId) {
		getEnemySprite(enemyId);
		spriteSheet->draw(go->position.x - camera->window.x, go->position.y + camera->window.y, 1, &textureRect);
	}

	void handleDeath(Camera* camera, GameObject* go, ObjectPool<Enemy>* enemyPool = nullptr) {
		Player* player = dynamic_cast<Player*>(go);
		Enemy* enemy = dynamic_cast<Enemy*>(go);

		if (player != nullptr) {
			SDL_Log("Player died");
			
		}

		if (enemy != nullptr && enemyPool != nullptr) {
			SDL_Log("Enemy Died");
			getEnemySprite(enemy->animIds[3]);
			enemy->position.y += deathSpeed;
			enemy->position.x -= int(deathSpeed/2);
			animateDeath(camera, enemy->position, spriteSheet);
			if (enemy->position.y > SCREEN_HEIGHT + textureRect.h)
				removeEnemyAnimation(enemy, enemyPool);
		}
	}

	void animateDeath(Camera* camera, Vector2D position, Sprite* spriteSheet) {
		int frame = (SDL_GetTicks() / 100) % totalFrames; 
		(textureRect).x = frame * (textureRect).w;
		spriteSheet->draw(position.x - camera->window.x, position.y + camera->window.y, totalFrames, &textureRect);
	}

	void changeAnimations(Player* player, Enemy* enemy, Camera* camera) {
		
	}

	void removeEnemyAnimation(Enemy* killedEnemy, ObjectPool<Enemy>* enemyPool) {
		for (auto enemy = enemyPool->pool.begin(); enemy != enemyPool->pool.end(); enemy++) {
			if (*enemy == killedEnemy) {
				getEnemySprite((*enemy)->animIds[2]);
				enemyPool->pool.erase(enemy);
				break;
			}
		}

		if (enemyPool->pool.begin() == enemyPool->pool.end())
			for (char* enemyId : killedEnemy->animIds) {
				enemyItr = enemySprites.find(enemyId);
				if (enemyItr == enemySprites.end())
					continue;
				enemySprites.erase(enemyItr);
			}
	}

	void animateGo(GameObject* gameObj, Camera* camera, ObjectPool<Enemy>* enemyPool, SDL_KeyCode direction = SDLK_UNKNOWN, char* enemyId = nullptr) {		
		if (!gameObj->enabled) {
			handleDeath(camera, gameObj, enemyPool);
			return;
		}

		if (enemyId == nullptr) {
			playeritr = playerSprites.find(direction);
			if (playeritr == playerSprites.end()) {
				spriteSheet = idleSprite;
				totalFrames = 1;
				idleSprite->getDimensions(&textureRect);
			}
			else 
				getPlayerSprite(direction);
			
		}
		else 
			getEnemySprite(enemyId);
		
		/*Uint32 current = engine->getElapsedTime();
		float dT = (current - dt)*10;
		int framesToUpdate = floor(dT * 2); 

		if (framesToUpdate > 0) {
			frameCounter += framesToUpdate;
			frameCounter %= totalFrames;
			(textureRect).x = frameCounter * (textureRect).w;
		}*/

		int frame = (SDL_GetTicks() / 100) % totalFrames; //TODO: have better timer implementation
		(textureRect).x = frame * (textureRect).w;

		//Set bounding box collider dimension to draw dimensions
		//ugly implementation I know

		gameObj->collideRect.h = textureRect.h;
		gameObj->collideRect.w = textureRect.w;
		if (enemyId == nullptr) {
			gameObj->collideRect.x = gameObj->position.x + (PLAYER_WIDTH - textureRect.w) - camera->window.x; //screen space coords for rendering
			gameObj->collideRect.y = gameObj->position.y + (PLAYER_HEIGHT - textureRect.h) + camera->window.y;
			spriteSheet->draw(gameObj->collideRect.x, gameObj->collideRect.y, totalFrames, &textureRect);
		}
		else{
			gameObj->collideRect.x = gameObj->position.x - camera->window.x;
			gameObj->collideRect.y = gameObj->position.y + camera->window.y;
			spriteSheet->draw(int(gameObj->position.x - camera->window.x), int(gameObj->position.y + camera->window.y), totalFrames, &textureRect);
		}
	}

	void Destroy() {
		playerSprites.clear();
		enemySprites.clear();
	}
};