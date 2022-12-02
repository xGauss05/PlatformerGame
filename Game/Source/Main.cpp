#include "App.h"

#include "Defs.h"
#include "Log.h"
#include "Debug.h"

#include "SDL/include/SDL.h"

#include <chrono>
using namespace std::chrono;
#include <stdlib.h>
#include <thread>

enum MainState
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

App* app = NULL;

int main(int argc, char* args[])
{
	LOG("Engine starting ...");
	
	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		high_resolution_clock::time_point start = high_resolution_clock::now();
		switch (state)
		{
			// Allocate the engine --------------------------------------------
		case CREATE:
			LOG("CREATION PHASE ===============================");

			app = new App(argc, args);

			if (app != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			LOG("AWAKE PHASE ===============================");
			if (app->Awake() == true)
				state = START;
			else
			{
				LOG("ERROR: Awake failed");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
		case START:
			LOG("START PHASE ===============================");
			if (app->Start() == true)
			{
				state = LOOP;
				LOG("UPDATE PHASE ===============================");
			}
			else
			{
				state = FAIL;
				LOG("ERROR: Start failed");
			}
			break;

			// Loop all modules until we are asked to leave ---------------------
		case LOOP:
			if (app->Update() == false)
				state = CLEAN;
			break;

			// Cleanup allocated memory -----------------------------------------
		case CLEAN:
			LOG("CLEANUP PHASE ===============================");
			if (app->CleanUp() == true)
			{
				result = EXIT_SUCCESS;
				state = EXIT;
			}
			else
				state = FAIL;

			break;

			// Exit with errors and shame ---------------------------------------
		case FAIL:
			LOG("Exiting with errors :(");
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
		//Time per cycle
		high_resolution_clock::time_point endCycle = high_resolution_clock::now();
		app->debug->elapsedCycle = duration_cast<microseconds>(endCycle - start);

		//Time per frame in microseconds acording to taget FPS
		int microSecCheck = (int)((1.0f / (float)app->debug->targetFPS) * 1E6);

		//This is to cap FPS, the diplaying of FPS on screen is calculated underneath
		if (app->debug->elapsedCycle < std::chrono::microseconds(microSecCheck))
		{
			std::this_thread::sleep_for(std::chrono::microseconds(std::chrono::microseconds(microSecCheck) - app->debug->elapsedCycle));
		}

		//Time per cycle + delay
		high_resolution_clock::time_point endFrame = high_resolution_clock::now();
		app->debug->elapsedFrame = duration_cast<microseconds>(endFrame - start);

		//Calculate FPSs
		app->debug->FPS = 1 / ((double)app->debug->elapsedFrame.count() * 10E-7);
	}

	RELEASE(app);
	LOG("... Bye! :)\n");

	// Dump memory leaks
	
	return result;
}