#include "game_object.h"
#include "component.h"
#include "engine.h"
#include "Vector2D.h"

void GameObject::Create()
{
	SDL_Log("GameObject::Create");

	enabled = false;
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
}


void GameObject::Init()
{
	SDL_Log("GameObject::Init");

	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Init();

	enabled = true;
}

void GameObject::Init(Vector2D position) {
	GameObject::Init();
	this->position = position;
}

void GameObject::Update(float dt)
{
	if (!enabled)
		return;

	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Update(dt);
}

void GameObject::Destroy()
{
	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Destroy();
}

GameObject::~GameObject()
{
	SDL_Log("GameObject::~GameObject");
}

void GameObject::AddReceiver(GameObject * go)
{
	receivers.push_back(go);
}

void GameObject::Send(Message m, GameObject* go)
{
	for (auto i = 0; i < receivers.size(); i++)
	{
		if (!receivers[i]->enabled)
			continue;

		if (go != nullptr)
			receivers[i]->Receive(m, go);
		else
			receivers[i]->Receive(m);
	}
}