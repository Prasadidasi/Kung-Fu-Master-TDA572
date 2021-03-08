#pragma once

class Camera;
class Player : public GameObject
{
public:

	int lives;
	int health;
	bool grabbed = false;
	const double player_width = 28;
	const double player_height = 63;
	Camera* camera;
	Sprite* playerSprite;

	virtual ~Player() { SDL_Log("Player::~Player"); }

	virtual void Init(Vector2D position, Camera* camera)
	{
		this->position = position;
		SDL_Log("Player::Init");
		GameObject::Init();
		this->camera = camera;
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

public:
	bool isLevelOver = false;
	bool step = false;
	bool stride = true;
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(engine, go, game_objects);
		this->velocity.x = 0;
		this->velocity.y = 0;
	}

	virtual void Init() {}

	bool inRange(int low, int high) {
		int x = int(go->position.x);
		return ((x - high) * (x - low) <= 0);
	}

	virtual void Update(float dt)
	{
		if (inRange(110,120) && isLevelOver == false) {
			go->Send(LEVEL_WIN);
			return;
		}

		if (isLevelOver == true) {
			if (go->position.x >= 20 || go->position.y >= 80) 
				MoveOnStairs(dt);
			return;
		}
		
		Engine::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.right) {
			velocity.x = PLAYER_SPEED * dt;
			Move();
		}
		if (keys.left) {
			velocity.x = -PLAYER_SPEED * dt;
			Move();
		}
	}

	void MoveOnStairs(float dt) {
		SDL_Delay(200);
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
		Player* player = (Player*)go;
		if (!player->grabbed)
			go->position.x += velocity.x;
		
		if (go->position.x < 0)
			go->position.x = 0;

		if (go->position.x > (LEVEL_WIDTH - rightCorner))
			go->position.x = LEVEL_WIDTH - rightCorner;
	}
};