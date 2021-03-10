#pragma once

class Knife : public GameObject {
public:
	Vector2D window;
	bool thrown = false;
	bool flip = false;
	~Knife() {};

	virtual void Init(Vector2D position, Vector2D window) {
		GameObject::Init();
		this->id = "KNIFE";
		this->position = position;
		this->window = window;
	}

	virtual void Receive(Message m, GameObject* go) {

		if (m == KNIFE_HIT) {
			enabled = false;
			thrown = false;
			Send(KNIFE_HIT);
			SDL_Log("KNIFE HIT");
		}

		if (m == HIT) {
			enabled = false;
			thrown = false;
		}
	}

};

class KnifeBehaviourComponent : public Component
{
	Knife* knife;
public:
	~KnifeBehaviourComponent() { SDL_Log("Knife::~Knife"); }

	virtual void Create(Engine* engine, GameObject* go, std::set<GameObject*>* game_objects)
	{
		Component::Create(engine, go, game_objects);
		this->knife = (Knife*)go;
	}

	void Update(float dt)
	{
		if (knife->thrown)
			if (abs(go->position.x - knife->window.x) < 0 || abs(go->position.x - knife->window.x) > SCREEN_WIDTH)
				go->enabled = false;
	}

};