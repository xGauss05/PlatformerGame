#pragma once
#include "Module.h"
#include "SDL/include/SDL.h"

#include <chrono>
using namespace std::chrono; //(testing)

class UserInterface : public Module
{
public:
	UserInterface();
	virtual ~UserInterface();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void StartTimer(int time);

	bool timerOn;
	high_resolution_clock::time_point startPause;
	high_resolution_clock::time_point endPause;
	milliseconds totalTime = milliseconds(30000);

private:
	
	milliseconds elapsed;
	high_resolution_clock::time_point startTime;
	high_resolution_clock::time_point currentTime;

	SDL_Texture* lives_tex;
	SDL_Texture* keycard_tex;
};

