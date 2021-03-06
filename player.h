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
			//if (lives < 0)
			//	Send(GAME_OVER);
		}

		if (m == GRAB) {
			if (grabbed == false) {
				go->position.x -= 10;
				grabbed = true;
			}
		}
	}

	void RemoveLife()
	{
		lives--;
	}
};

class PlayerBehaviourComponent : public Component
{
	float time_fire_pressed;	// time from the last time the fire button was pressed
	const double rightCorner = 50;
	Vector2D velocity;

public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(engine, go, game_objects);
		this->velocity.x = 0;
		this->velocity.y = 0;
	}

	virtual void Init()
	{

	}

	virtual void Update(float dt)
	{
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