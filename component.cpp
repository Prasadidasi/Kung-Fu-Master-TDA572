#include "component.h"
#include "game_object.h"
#include "engine.h"

void Component::Create(Engine * engine, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->engine = engine;
	this->game_objects = game_objects;
}

void RenderComponent::Create(Engine * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(engine, go, game_objects);

	sprite = engine->createSprite(sprite_name);
}

void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->position.x), int(go->position.y));
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(Engine* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(engine, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(float dt)
{
	Engine::KeyStatus  keys;
	engine->getKeyStatus(keys);
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled)
		{
			if (SDL_HasIntersection(&go->collideRect, &go0->collideRect) && (keys.kick == true || keys.punch == true ))
			{
				go->Receive(HIT, go);
				go0->Receive(HIT, go0);
			}
			if (SDL_HasIntersection(&go->collideRect, &go0->collideRect) && (keys.kick == false && keys.punch == false))
			{
				if (go0->getID() == "GRAPPLER") {
					go->Receive(GRAB, go);
					go0->Receive(GRAB, go0);
				}
				if (go0->getID() == "KNIFE") {
					go->Receive(KNIFE_HIT, go);
					go0->Receive(KNIFE_HIT, go0);
				}
			}
		}
	}
}

void RigidBodyComponent::Create(Engine* engine, GameObject* go, std::set<GameObject*>* game_objects)
{
	Component::Create(engine, go, game_objects);
	velocity.x = 0;
	velocity.y = 0;
}



void RigidBodyComponent::Update(float dt)
{
	//Forward Euler method:
	go->position.x = go->position.x + velocity.x * dt;
}
