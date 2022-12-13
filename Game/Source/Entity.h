#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"

enum class EntityType
{
	PLAYER,
	ITEM,
	SAW,
	ENEMY_FLY,
	ENEMY_WALK,
	UNKNOWN
};

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update()
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node&)
	{
		return true;
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) 
	{
		
	};

	virtual void DeathAnimation() 
	{

	}

public:

	SString name;
	EntityType type;
	bool active = true;
	int level;
	pugi::xml_node parameters;
	PhysBody* pbody = nullptr;
	iPoint position;       

};

#endif // __ENTITY_H__