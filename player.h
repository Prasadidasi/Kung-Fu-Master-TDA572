#pragma once

class Player : public GameObject
{
	char* playerSprite;				//spriteId of current player state
public:
	bool isLevelOver = false;
	bool dead = false;
	int health;
	bool grabbed = false;
	const double player_width = 28;
	const double player_height = 63;
	virtual ~Player() { SDL_Log("Player::~Player"); }

	virtual void Init(Vector2D position)
	{
		this->position = position;
		SDL_Log("Player::Init");
		this->id = "PLAYER";
		GameObject::Init();
	}

	virtual void setPlayerSprite(char* spriteId) {
		this->playerSprite = spriteId;
	}

	char* getPlayerSprite() {
		return this->playerSprite;
	}

	virtual void Receive(Message m, GameObject* go)
	{
		if (m == HIT)
		{
			grabbed = false;
			SDL_Log("Player::Hit!");
		}

		if (m == GRAB) 
			grabbed = true;
	}
};

class PlayerBehaviourComponent : public Component
{
	float time_fire_pressed;	// time from the last time the fire button was pressed
	const double rightCorner = 50;
	Vector2D velocity;
	double stepHeight = 13;
	double stepWidth = 8;
	Player* player;

public:
	bool step = false;
	bool stride = true;
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(engine, go, game_objects);
		this->velocity.x = 0;
		this->velocity.y = 0;
		this->player = (Player*)go;
	}

	virtual void Init() {}

	bool inRange(int low, int high) {
		int x = int(go->position.x);
		return ((x - high) * (x - low) <= 0);
	}

	virtual void Update(float dt)
	{
		if (inRange(115,125) && player->isLevelOver == false) {
			player->isLevelOver = true;
			go->Send(LEVEL_WIN);
			return;
		}

		if (player->isLevelOver == true) {
			if (go->position.x >= 20 || go->position.y >= 70) 
				MoveOnStairs(dt);
			return;
		}
		
		Engine::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.right && !keys.down && !keys.up && !keys.kick && !keys.punch) {
			velocity.x = PLAYER_SPEED * dt;
			Move();
		}
		if (keys.left && !keys.down && !keys.up && !keys.kick && !keys.punch) {
			velocity.x = -PLAYER_SPEED * dt;
			Move();
		}
	}

	void MoveOnStairs(float dt) {
		SDL_Delay(100);
		if (stride && !step) {
			go->position.x = go->position.x - stepWidth;
		}
		if (step && !stride) {
			go->position.y = go->position.y - stepHeight;
		}

		stride = stride ? false : true;
		step = step ? false : true;
	}

	// move the player left or right
	// param move depends on the time, so the player moves always at the same speed on any computer
	void Move()
	{
		if (!this->player->grabbed)
			go->position.x += velocity.x;
		
		if (go->position.x < 0)
			go->position.x = 0;

		if (go->position.x > (LEVEL_WIDTH - rightCorner))
			go->position.x = LEVEL_WIDTH - rightCorner;
	}
};