#ifndef ___ENEMY_WALK_H__
#define ___ENEMY_WALK_H__

#include "Entity.h"
#include "Point.h"
#include "Pathfinding.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Enemy_Walk : public Entity
{
public:

	Enemy_Walk();
	virtual ~Enemy_Walk();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void DeathAnimation();

public:

private:

	SDL_Texture* texture;
	const char* texturePath;

	float speedCap = 6.0f;

	DynArray<iPoint> pathToPlayer;
	bool jumping = false;

	bool pendingToDelete = false;
};

#endif // ___ENEMY_WALK_H__