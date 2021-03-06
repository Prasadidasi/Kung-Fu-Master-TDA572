#pragma once

#include <set>
#include "object_pool.h"
#include "Vector2D.h"

class GameObject;
class Engine;
class Sprite;

class Component
{
protected:
	Engine * engine;	// used to access the engine
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
	Sprite* sprite;

public:

	virtual void Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};

class RigidBodyComponent : public Component
{
public:
	Vector2D velocity;
	SDL_Rect rigidBodyRect;

	virtual void Create(Engine* engine, GameObject* go, std::set<GameObject*>* game_objects);
	virtual void Update(float dt);
};

/*class CameraComponent : public Component
{
	Sprite* sprite;
	Camera* camera;

public:

	virtual void Create(Engine* engine, GameObject* go, std::set<GameObject*>* game_objects, Camera* camera, const char* sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite* GetSprite() { return sprite; }
};*/