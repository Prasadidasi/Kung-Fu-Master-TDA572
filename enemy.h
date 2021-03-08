#pragma once

class Camera;
class Enemy : public GameObject {
public:
	char* enemyType;
	bool firstGrab = true;
	bool grabPlayer = false;
	bool flip = false;
	bool spawned = false;
	std::vector<char*> animIds; //holds animation sprites in the order idle, action, hit, death, grab.
	Sprite* enemySprite;

	virtual ~Enemy() { SDL_Log("Enemy::~Enemy"); }

	virtual void Init(Vector2D position, char* enemyType)
	{
		GameObject::Init();
		this->position = position;
		SDL_Log("Enemy::Init");
		this->enemyType = enemyType;
		spawned = true;
	}

	virtual void Receive(Message m, GameObject* go)
	{
		Enemy* enemy = (Enemy*)go;
		if (m == HIT)
		{
			enabled = false;
			Send(ENEMY_HIT, go);
			SDL_Log("Enemy::Hit!");
		}

		if (m == GRAB) {
			if (enemyType == "GRAPPLER") {
				if (firstGrab) {
					if (flip)
						go->position.x -= 10;
					else
						go->position.x += 10;
					firstGrab = false;
				}
				go->GetComponent<RigidBodyComponent*>()->velocity.x = 0;
				Send(PLAYER_GRABBED, go);
			}
		}
	}
};

class EnemyBehaviorComponent : public Component {
	const double rightCorner = 50;
	ObjectPool<Knife>* knifePool;
	Enemy* enemy;
public:
	Vector2D velocity;
	virtual ~EnemyBehaviorComponent() { SDL_Log("Enemy::~Enemy"); }
	virtual void Create(Engine* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<Knife> *knifePool)
	{
		Component::Create(engine, go, game_objects);
		this->velocity.x = 0;
		this->velocity.y = 0;
		this->enemy = (Enemy*)go;
		this->knifePool = knifePool;
	}

	virtual void Update(float dt) {
		if (go->position.x > (LEVEL_WIDTH - rightCorner)) 
			go->position.x = LEVEL_WIDTH - rightCorner;
	}
};