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

	void Update(int position, ObjectPool<Enemy> *enemyPool, float dt, int low, int high, bool flip) {
		if (dt <= 0)
			return;

		if (inRange(low, high) && isEnemySpawned[position] == 0) {
			isEnemySpawned[position] = 1;
			for (int i = 0; i < 5; i++) {
				int spawn = flip ? high + (SCREEN_WIDTH / 2): high - (SCREEN_WIDTH)/2;
				Enemy* enemy = enemyPool->FirstAvailable();
				if (enemy == NULL)
					continue;
				enemy->grabPlayer = false;
				enemy->enabled = true;
				enemy->dead = false;
				enemy->firstGrab = true;
				enemy->Init(Vector2D(flip? spawn + 50*i: spawn - 50*i, 211), "GRAPPLER");
				enemy->GetComponent<RigidBodyComponent*>()->velocity.x = flip ? -ENEMY_SPEED : ENEMY_SPEED;
				enemy->flip = flip;
			}
		}
	}
};