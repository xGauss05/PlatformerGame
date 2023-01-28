#include "App.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene_Intro.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "Scene_Win.h"
#include "Scene_Die.h"
#include "EntityManager.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Physics.h"
#include "FadeToBlack.h"
#include "Fonts.h"
#include "UserInterface.h"
#include "GuiManager.h"
#include "Debug.h"

#include "Defs.h"
#include "Log.h"

#include "Optick/include/optick.h"

#include <iostream>
#include <sstream>

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;

	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	font = new Fonts();
	audio = new Audio();
	scene_intro = new Scene_Intro();
	scene_menu = new Scene_Menu();
	scene = new Scene_Level1();
	scene_win = new Scene_Win();
	scene_die = new Scene_Die();
	entityManager = new EntityManager();
	map = new Map();
	pathfinding = new PathFinding();
	physics = new Physics();
	ftb = new FadeToBlack();
	ui = new UserInterface();
	debug = new Debug();
	guiManager = new GuiManager();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(win);
	AddModule(input);
	AddModule(tex);
	AddModule(audio);
	AddModule(debug);
	AddModule(font);
	AddModule(physics);
	AddModule(scene_intro);
	AddModule(scene_menu);
	AddModule(scene);
	AddModule(scene_win);
	AddModule(scene_die);
	AddModule(entityManager);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(guiManager);
	AddModule(ui);
	AddModule(ftb);

	// Render last to swap buffer
	AddModule(render);
}

// Destructor
App::~App()
{
	// Release modules
	ListItem<Module*>* item = modules.end;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.Clear();
}

void App::AddModule(Module* module)
{
	module->Init();
	modules.Add(module);
}

// Called before render is available
bool App::Awake()
{
	timer = Timer();
	
	maxFrameDuration = 16;
	bool ret = false;
	ret = LoadConfig();

	if (ret == true)
	{
		title = configNode.child("app").child("title").child_value();
		frcap = configNode.child("app").child("frcap").attribute("value").as_int();
		ListItem<Module*>* item;
		item = modules.start;

		while (item != NULL && ret == true)
		{
			pugi::xml_node node = configNode.child(item->data->name.GetString());
			ret = item->data->Awake(node);
			item = item->next;
		}
	}

	return ret;
}

// Called before the first frame
bool App::Start()
{
	timer.Start();
	startupTime.Start();
	lastSecFrameTime.Start();

	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	return ret;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// Load config from XML file
bool App::LoadConfig()
{
	bool ret = false;

	pugi::xml_parse_result parseResult = configFile.load_file("config.xml");

	if (parseResult) 
	{
		ret = true;
		configNode = configFile.child("config");
	}
	else 
	{
		LOG("Error in App::LoadConfig(): %s", parseResult.description());
	}

	return ret;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
	OPTICK_EVENT();
	frameTime.Start();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	OPTICK_EVENT();
	if (loadGameRequested == true) LoadFromFile();
	if (saveGameRequested == true) SaveToFile();

	frameCount++;
	
	secondsSinceStartup = startupTime.ReadSec();
	
	dt = frameTime.ReadMSec();
	
	lastSecFrameCount++;

	if (lastSecFrameTime.ReadMSec() > 1000) 
	{
		lastSecFrameTime.Start();
		framesPerSecond = lastSecFrameCount;
		lastSecFrameCount = 0;
		
		averageFps = (averageFps + framesPerSecond) / 2;
	}

	float delay = float(maxFrameDuration) - dt;

	PerfTimer delayTimer = PerfTimer();
	delayTimer.Start();
	if (maxFrameDuration > 0 && delay > 0) 
	{
		SDL_Delay(delay);
		LOG("We waited for %f milliseconds and the real delay is % f", delay, delayTimer.ReadMs());
		dt = maxFrameDuration;
	}
	else 
	{
		dt = lastSecFrameTime.ReadSec();
	}
	/*LOG("Av.FPS: %.2f Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u ",
		averageFps, framesPerSecond, dt, secondsSinceStartup, frameCount);*/

	static char title[256];
	if (vsync) 
	{
		if (frameTime.ReadSec() < dt) 
		{
			SDL_Delay(dt - frameTime.ReadSec());
		}

		sprintf_s(title, 256, "Super Metal Boy: Av.FPS: %.2f Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u VSYNC: ON",
			averageFps, framesPerSecond, dt, secondsSinceStartup, frameCount);
	}
	else 
	{
		sprintf_s(title, 256, "Super Metal Boy: Av.FPS: %.2f Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u VSYNC: OFF",
			averageFps, framesPerSecond, dt, secondsSinceStartup, frameCount);
	}
	

	app->win->SetTitle(title);
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	OPTICK_EVENT();
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) continue;
		
		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	OPTICK_EVENT();
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) continue;

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	OPTICK_EVENT();
	bool ret = true;
	ListItem<Module*>* item;
	Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) continue;
		 
		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* App::GetOrganization() const
{
	return organization.GetString();
}

void App::LoadGameRequest()
{
	// NOTE: We should check if SAVE_STATE_FILENAME actually exist
	loadGameRequested = true;
}

// ---------------------------------------
void App::SaveGameRequest() 
{
	// NOTE: We should check if SAVE_STATE_FILENAME actually exist and... should we overwriten
	saveGameRequested = true;
}

bool App::LoadFromFile()
{
	bool ret = true;

	pugi::xml_document gameStateFile;
	pugi::xml_parse_result result = gameStateFile.load_file("save_game_debug.xml");

	if (result == NULL)
	{
		LOG("Could not load xml file savegame.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		ListItem<Module*>* item;
		item = modules.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->LoadState(gameStateFile.child("save_state").child(item->data->name.GetString()));
			item = item->next;
		}
	}

	loadGameRequested = false;

	return ret;
}

bool App::SaveToFile() 
{
	bool ret = false;

	pugi::xml_document* saveDoc = new pugi::xml_document();
	pugi::xml_node saveStateNode = saveDoc->append_child("save_state");

	ListItem<Module*>* item;
	item = modules.start;

	while (item != NULL)
	{
		ret = item->data->SaveState(saveStateNode.append_child(item->data->name.GetString()));
		item = item->next;
	}

	ret = saveDoc->save_file("save_game_debug.xml");

	saveGameRequested = false;

	return ret;
}