#pragma once

// GameObject represents objects which moves are drawn
#include <vector>
#include <iostream>
#include "Vector2D.h"
#include <SDL.h>

enum Message {HIT, PLAYER_HIT, GRAB, PLAYER_GRABBED, ENEMY_HIT, PLAYER_DEAD, GAME_OVER, LEVEL_WIN, LEVEL_RESTART };

class Component;
class Vector2D;

class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;
	std::string id;

public:
	SDL_Rect collideRect;
	Vector2D position;
	bool enabled;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component* component);

	virtual void Init();
	virtual void Init(Vector2D position);
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void AddReceiver(GameObject* go);
	virtual void Receive(Message m, GameObject* go = nullptr) {}
	//virtual void Receive(Message m) {}
	void Send(Message m, GameObject* go = nullptr);

	template<typename T>
	T GetComponent() {
		for (Component* c : components) {
			T t = dynamic_cast<T>(c);  //ugly but it works...
			if (t != nullptr) {
				return t;
			}
		}

		return nullptr;
	}

	std::string getID() {
		return id;
	}
};