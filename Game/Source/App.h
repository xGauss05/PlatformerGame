#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "List.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene_Intro;
class Scene_Menu;
class Scene_Level1;
class Scene_Transition;
class Scene_Win;
class Scene_Die;
class EntityManager;
class Map;
class PathFinding;
class Physics;
class FadeToBlack;
class Fonts;
class UserInterface;
class GuiManager;
class Debug;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGameRequest();
	void SaveGameRequest() ;
	bool LoadFromFile();
	bool SaveToFile() ;

private:

	// Load config file
	bool LoadConfig();

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	Scene_Intro* scene_intro;
	Scene_Menu* scene_menu;
	Scene_Level1* scene;
	Scene_Transition* transition;
	Scene_Win* scene_win;
	Scene_Die* scene_die;
	EntityManager* entityManager;
	Physics* physics;
	Map* map;
	PathFinding* pathfinding;
	FadeToBlack* ftb;
	Fonts* font;
	UserInterface* ui;
	GuiManager* guiManager;
	Debug* debug;

	uint frames;
	uint32 maxFrameDuration = 0;
	int frcap;
	bool vsync;

private:

	int argc;
	char** args;
	SString title;
	SString organization;

	List<Module*> modules;

	// xml_document to store the config file and
	// xml_node(s) to read specific branches of the xml
	pugi::xml_document configFile;
	pugi::xml_node configNode;

	float dt;

	// required variables are provided:
	Timer timer;
	PerfTimer ptimer;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float secondsSinceStartup = 0.0f;


    bool saveGameRequested;
	bool loadGameRequested;

};

extern App* app;

#endif // __APP_H__