#pragma once

class SpawnHandler {
	std::vector<int> isEnemySpawned;
	Player* player;
public:
	void Create(Player* player) {
		this->player = player;
		isEnemySpawned = std::vector<int>(20, 0);
	}

	bool inRange(int low, int high) {
		int x = int(player->position.x);
		return ((x - high) * (x - low) <= 0);
	}
	
	//spawn knife when thrower is within range of player
	void spawnKnife(Enemy* enemy, Camera* camera, ObjectPool<Knife>* knifePool) {
		Knife* knife = knifePool->FirstAvailable();
		if (knife == nullptr)
			return;
		Vector2D pos, window;
		knife->flip = enemy->flip;
		pos.x = enemy->position.x;
		pos.y = enemy->position.y + 20;
		window.x = camera->window.x;
		window.y = camera->window.y;
		knife->Init(pos, window);
		knife->GetComponent<RigidBodyComponent*>()->velocity.x = enemy->flip ? -KNIFE_SPEED : KNIFE_SPEED;
		knife->thrown = true;
	}

	void Update(int position, ObjectPool<Enemy> *enemyPool, float dt, int low, int high, char* enemyType, bool flip) {
		if (dt <= 0)
			return;

		if (inRange(low, high) && isEnemySpawned[position] == 0) {
			isEnemySpawned[position] = 1;
			for (int i = 0; i < 5; i++) {			//hardcoded grapplers to spawn in groups of 5
				int spawn = flip ? high + (SCREEN_WIDTH / 2): high - (SCREEN_WIDTH)/2;	
				Enemy* enemy = enemyPool->FirstAvailable();
				if (enemy == NULL)
					continue;
				if (enemyType == "GRAPPLER") {
					enemy->grabPlayer = false;
					enemy->firstGrab = true;
					enemy->Init(Vector2D(flip ? spawn + 50 * i : spawn - 50 * i, 211), enemyType);
				}
				else if (enemyType == "THROWER") {
					i = 4;							//only one thrower needed, so setting index
					enemy->hitPoints = 2;
					enemy->throwKnife = false;
					enemy->throwerHit = false;
					enemy->Init(Vector2D(flip ? spawn : spawn, 205), enemyType);
				}
				enemy->dead = false;
				enemy->flip = flip;
				enemy->GetComponent<RigidBodyComponent*>()->velocity.x = flip ? -ENEMY_SPEED : ENEMY_SPEED;
			}
		}
	}

	void Destroy() {
		isEnemySpawned.clear();
	}
};