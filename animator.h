#pragma once
#include <map>

class Animator {
	int totalFrames;
	float fps;
	float deathSpeed;
	bool flip;
	char* playerSprite;

	Engine* engine;
	Camera* camera;
	Sprite* spriteSheet;
	Sprite* idleSprite;
	SDL_Rect textureRect;

	std::map<char*, Sprite*> sprites;
	std::map<char*, Sprite*>::iterator spriteItr;

	std::map<Sprite*, std::pair<int, SDL_Rect>> spriteAttrib;
	std::map<Sprite*, std::pair<int, SDL_Rect>>::iterator itrAtb;

public:
	float dt;
	int faceDirection;

	void Create(Engine* engine, Camera* camera, char* sprite_name) {
		flip = false;
		deathSpeed = 5.f;
		this->engine = engine;
		this->camera = camera;
		engine->getFaceDirection(faceDirection);

		idleSprite = engine->createSprite(sprite_name);
		playerSprite = sprite_name;
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

		if (enemy->throwerHit) {
			enemy->throwerHit = false;
			return enemy->animIds[2];	//Throw
		}

		if (abs(player->position.x - enemy->position.x) > 200) {
			if (enemy->enemyType == "THROWER") {
				enemy->GetComponent<RigidBodyComponent*>()->velocity.x = enemy->flip ? -ENEMY_SPEED : ENEMY_SPEED;
				enemy->throwKnife = false;
			}
			return enemy->animIds[0];	//Walk
		}
		else {
			if (enemy->enemyType == "THROWER") {
				return enemy->animIds[4];
			}
			return enemy->animIds[1];	//Attack
		}
	}

	void getSprite(char* spriteId) {
		spriteItr = sprites.find(spriteId);
		if (spriteItr == sprites.end())
			return;

		spriteSheet = spriteItr->second;
		itrAtb = spriteAttrib.find(spriteSheet);
		textureRect = itrAtb->second.second;
		totalFrames = itrAtb->second.first;
		fps = 1 / totalFrames;
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
			player->position.y += deathSpeed / 5;
			animateDeath(camera, player->position, spriteSheet, 500);
			if (player->position.y > SCREEN_HEIGHT + textureRect.h)
				player->Send(LEVEL_RESTART);
		}

		if (enemy != nullptr && enemyPool != nullptr && !enemy->dead) {
			getSprite(enemy->animIds[3]);
			enemy->position.y += deathSpeed;
			enemy->position.x -= int(deathSpeed / 2);
			animateDeath(camera, enemy->position, spriteSheet, 200, enemy->flip);
			if (enemy->position.y > SCREEN_HEIGHT + textureRect.h) {
				removeEnemyAnimation(enemy, enemyPool);
				enemy->dead = true;
				SDL_Log("ENEMY DEAD");
			}
		}
	}

	void animateDeath(Camera* camera, Vector2D position, Sprite* spriteSheet, int delay = 100, bool flip = false) {
		int frame = (SDL_GetTicks() / delay) % totalFrames;
		(textureRect).x = frame * (textureRect).w;
		spriteSheet->draw(position.x - camera->window.x, position.y + camera->window.y, totalFrames, &textureRect, flip);
	}

	void removeEnemyAnimation(Enemy* killedEnemy, ObjectPool<Enemy>* enemyPool) {
		if (enemyPool->pool.begin() == enemyPool->pool.end())
			for (char* enemyId : killedEnemy->animIds) {
				spriteItr = sprites.find(enemyId);
				if (spriteItr == sprites.end())
					continue;
				sprites.erase(spriteItr);
			}
	}

	void setFaceDirection(float dt) {
		if (dt <= 0)
			return;

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

	void animatePlayer(Player* player, Camera* camera) {
		if (!player->enabled) {
			handleDeath(camera, player);
			return;
		}

		char* spriteId = player->getPlayerSprite();

		/*Uint32 current = SDL_GetTicks();
		Uint32 elapsedTime = (current - lastTime)/1000;
		timer += (spriteId == "PLAYER_IDLE") ? 0 : elapsedTime;

		if (frameCounter >= totalFrames) {
			frameCounter = 0;
		}

		getSprite(spriteId);
		if (timer > fps && spriteId != "PLAYER_IDLE") {
			frameCounter++ ;
			frameCounter %= totalFrames;
			textureRect.x = frameCounter * textureRect.w;
			timer -= fps;
			lastTime = current;
		}*/
		if (spriteId == NULL) {
			SDL_Log("SpriteID NULL, cannot animate");
			return;
		}
		getSprite(spriteId);
		int frame = int(engine->getElapsedTime()*10) % totalFrames; //TODO: have better timer implementation
		if (frame > totalFrames)
			frame = 0;
		(textureRect).x = frame * (textureRect).w;

		player->collideRect.h = textureRect.h;    		//Set bounding box collider dimensions 
		player->collideRect.w = textureRect.w;			//ugly implementation I know
		player->collideRect.x = player->position.x + (PLAYER_WIDTH - textureRect.w) - camera->window.x; //screen space coords for rendering
		player->collideRect.y = player->position.y + (PLAYER_HEIGHT - textureRect.h) + camera->window.y;
		spriteSheet->draw(player->collideRect.x, player->collideRect.y, totalFrames, &textureRect, faceDirection == 1 ? true : false);
	}

	void animateGo(GameObject* go, Camera* camera, ObjectPool<Enemy>* enemyPool, char* spriteId) {
		Enemy* enemy = (Enemy*)go;
		if (!go->enabled) {
			if (enemy != nullptr && enemy->spawned)
				handleDeath(camera, go, enemyPool);
			return;
		}

		getSprite(spriteId);

		bool flip = false;
		if (enemy != nullptr && go->getID() != "KNIFE")
			flip = enemy->flip;

		if (go->getID() == "KNIFE") {
			flip = ((Knife*)go)->flip;
		}

		int frame = (SDL_GetTicks() / 100) % totalFrames; //TODO: have better timer implementation
		(textureRect).x = frame * (textureRect).w;

		go->collideRect.h = textureRect.h;    		//Set bounding box collider dimension to draw dimensions
		go->collideRect.w = textureRect.w;			//ugly implementation I know
		go->collideRect.x = go->position.x - camera->window.x;
		go->collideRect.y = go->position.y + camera->window.y;
		spriteSheet->draw(int(go->position.x - camera->window.x), int(go->position.y + camera->window.y), totalFrames, &textureRect, flip);

	}

	void Destroy() {
		sprites.clear();
		spriteAttrib.clear();
	}
};