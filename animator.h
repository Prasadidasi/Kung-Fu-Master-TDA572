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

	std::map<char*, Sprite*> sprites;
	std::map<char*, Sprite*>::iterator spriteItr;

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
		sprites.insert(std::pair<char*, Sprite*>(identifier, sprite));
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

	void getSprite(char* spriteId) {
		spriteItr = sprites.find(spriteId);
		if (spriteItr == sprites.end())
			return;

		spriteSheet = spriteItr->second;
		itrAtb = spriteAttrib.find(spriteSheet);
		textureRect = itrAtb->second.second;
		totalFrames = itrAtb->second.first;
	}

	void enemyHit(Camera* camera, GameObject* go, char* enemyId) {
		getSprite(enemyId);
		spriteSheet->draw(go->position.x - camera->window.x, go->position.y + camera->window.y, 1, &textureRect);
	}

	void handleDeath(Camera* camera, GameObject* go, ObjectPool<Enemy>* enemyPool = nullptr) {
		Player* player = dynamic_cast<Player*>(go);
		Enemy* enemy = dynamic_cast<Enemy*>(go);

		if (player != nullptr) {
			SDL_Log("Player died");
			getSprite("PLAYER_DEATH");
			player->position.y += deathSpeed/5;
			animateDeath(camera, player->position, spriteSheet);
			if (player->position.y > SCREEN_HEIGHT + textureRect.h)
				player->Send(LEVEL_RESTART);
		}

		if (enemy != nullptr && enemyPool != nullptr) {
			SDL_Log("Enemy Died");
			getSprite(enemy->animIds[3]);
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

	void removeEnemyAnimation(Enemy* killedEnemy, ObjectPool<Enemy>* enemyPool) {
		for (auto enemy = enemyPool->pool.begin(); enemy != enemyPool->pool.end(); enemy++) {
			if (*enemy == killedEnemy) {
				getSprite((*enemy)->animIds[2]);
				enemyPool->pool.erase(enemy);
				break;
			}
		}

		if (enemyPool->pool.begin() == enemyPool->pool.end())
			for (char* enemyId : killedEnemy->animIds) {
				spriteItr = sprites.find(enemyId);
				if (spriteItr == sprites.end())
					continue;
				sprites.erase(spriteItr);
			}
	}

	void setFaceDirection() {

		Engine::KeyStatus keys;
		engine->getKeyStatus(keys);

		if (faceDirection == -1 && keys.right) {
			faceDirection = 1;
		}
		if (faceDirection == 1 && keys.left) {
			faceDirection = -1;
		}
		if (faceDirection == 0 && keys.right) {
			faceDirection = 1;
		}
	}

	void handleMultipleInput() {

	}

	void animatePlayer(GameObject* go, Camera* camera) {
		if (!go->enabled) {
			handleDeath(camera, go);
			return;
		}

		SDL_KeyCode keyPressed;
		engine->getKeyPressed(keyPressed);
		char* spriteId = NULL;
		
		switch (keyPressed) {
			case SDLK_UNKNOWN:
				spriteId = "PLAYER_IDLE";
				break;
			case SDLK_RIGHT:
			case SDLK_LEFT:
				spriteId = "PLAYER_LEFT";
				break;
			case SDLK_UP:
				spriteId = "PLAYER_UP";
				break;
			case SDLK_DOWN:
				spriteId = "PLAYER_DOWN";
				break;
			case SDLK_a:
				spriteId = "PLAYER_KICK";
				break;
			case SDLK_d:
				spriteId = "PLAYER_PUNCH";
				break;
			default:
				spriteId = "PLAYER_IDLE";
				break;
		}

		getSprite(spriteId);
		int frame = (SDL_GetTicks() / 100) % totalFrames; //TODO: have better timer implementation
		(textureRect).x = frame * (textureRect).w;

		go->collideRect.h = textureRect.h;    		//Set bounding box collider dimension to draw dimensions
		go->collideRect.w = textureRect.w;			//ugly implementation I know
		go->collideRect.x = go->position.x + (PLAYER_WIDTH - textureRect.w) - camera->window.x; //screen space coords for rendering
		go->collideRect.y = go->position.y + (PLAYER_HEIGHT - textureRect.h) + camera->window.y;
		spriteSheet->draw(go->collideRect.x, go->collideRect.y, totalFrames, &textureRect, faceDirection == 1? true : false);

	}

	void animateGo(GameObject* go, Camera* camera, ObjectPool<Enemy>* enemyPool, char* spriteId) {		
		Enemy* enemy = (Enemy*)go;
		if (!go->enabled) {
			if (enemy != nullptr && enemy->spawned)
				handleDeath(camera, go, enemyPool);
			return;
		}
		
		getSprite(spriteId);

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

		bool flip = false;
		if (enemy != nullptr)
			flip = enemy->flip;						

		go->collideRect.h = textureRect.h;    		//Set bounding box collider dimension to draw dimensions
		go->collideRect.w = textureRect.w;			//ugly implementation I know
		go->collideRect.x = go->position.x - camera->window.x;
		go->collideRect.y = go->position.y + camera->window.y;
		spriteSheet->draw(int(go->position.x - camera->window.x), int(go->position.y + camera->window.y), totalFrames, &textureRect, flip);
		
	}

	void Destroy() {
		sprites.clear();
		sprites.clear();
	}
};