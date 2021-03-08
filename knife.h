#pragma once

class Knife : public GameObject {
public:
	bool thrown = false;
	bool flip = false;
	~Knife() {};

	virtual void Init(Vector2D position) {
		this->position = position;
	}

	virtual void Recieve(Message m, GameObject* go) {

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
		knife = (Knife*)go;
	}

	void Update(float dt)
	{
		if (knife->thrown)
			knife->position.x += knife->flip ? -KNIFE_SPEED : KNIFE_SPEED;
	}

};