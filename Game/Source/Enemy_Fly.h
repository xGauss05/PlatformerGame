#ifndef ___ENEMY_FLY_H__
#define ___ENEMY_FLY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Enemy_Fly : public Entity
{
public:

	Enemy_Fly();
	virtual ~Enemy_Fly();

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

};

#endif // ___ENEMY_FLY_H__