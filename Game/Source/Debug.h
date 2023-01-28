#ifndef __MODULE_DEBUG_H__
#define __MODULE_DEBUG_H__

#include "Module.h"
#include "Point.h"
#include "DynArray.h"
#include "List.h"

#include <chrono>
using namespace std::chrono;

#define DEBUG_BOX 225

struct SDL_Texture;

class Debug : public Module {
public:

	// Constructor
	Debug();

	// Destructor
	~Debug();

	bool Start() override;

	// Called at the middle of the application loop
	// Switches the debug mode on/off
	bool Update(float dt);

	// Called at the end of the application loop
	// Draw all functionality (if debug mode is enabled)
	bool PostUpdate();

	// Draws all existing colliders with some transparency
	void DebugDraw();

	void DrawHitboxes();

	bool debug;

	bool variables = false;
	bool hitboxes = false;
	bool debugCamera = false;
	bool godMode = false;
	bool limitFps = false;
	bool paths = false;
	bool preferenceMatrix = false;
	bool guiBounds = false;
	int varBox = 130;

	DynArray<iPoint> playerPath;
	DynArray<iPoint> mousePath;

	//Pathfinding debug
	iPoint origin;
	bool originSelected = false;

	SDL_Texture* enemyPathTex = nullptr;
	SDL_Texture* playerPathTex = nullptr;
	SDL_Texture* manualPathTex = nullptr;
	SDL_Texture* xTex = nullptr;
};

#endif // __MODULE_DEBUG_H__
