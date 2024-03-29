#include "Debug.h"

#include "App.h"
#include "Input.h"
#include "Render.h"

#include "Player.h"
#include "Textures.h"
#include "Fonts.h"
#include "EntityManager.h"
#include "Physics.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Scene_Level1.h"
#include "Scene_Win.h"
#include "FadeToBlack.h"
#include "UserInterface.h"

#include "Defs.h"

#include <string>

using namespace std;

Debug::Debug() : Module()
{
	debug = false;
	name.Create("debug");
}

Debug::~Debug()
{

}

bool Debug::Start()
{
	debug = false;

	enemyPathTex = app->tex->Load("Assets/Maps/enemyPath.png");
	playerPathTex = app->tex->Load("Assets/Maps/playerPath.png");
	manualPathTex = app->tex->Load("Assets/Maps/manualPath.png");
	xTex = app->tex->Load("Assets/Maps/x.png");

	return true;
}

bool Debug::Update(float dt)
{
	// Toggle debug mode
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) debug = !debug;

	if (debug)
	{
		// Previous level
		if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			if (app->scene->player->level > 1)
			{
				app->scene->player->level--;
				app->scene->player->checkpointReached = false;
				app->scene->player->LevelSelector();
				app->entityManager->ActivateEnemies();
				app->scene->player->TeleportTo(app->scene->player->spawn);
			}
		}

		// Next level
		if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			if (app->scene->player->level < 4)
			{
				app->scene->player->level++;
				app->scene->player->checkpointReached = false;
				app->scene->player->LevelSelector();
				app->entityManager->ActivateEnemies();
				app->scene->player->TeleportTo(app->scene->player->spawn);
			}
		}

		// Reset level
		if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		{
			app->entityManager->ReviveAllEntities();
			app->entityManager->NeedsToSpawnAllEntities();
			app->entityManager->ActivateEnemies();
			app->scene->player->TeleportTo(app->scene->player->spawn);
		}

		// Save request
		if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveGameRequest();

		// Load request
		if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadGameRequest();

		// Move between checkpoints
		if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		{
			if (app->scene->player->level != 4)
			{
				iPoint nextCP = app->scene->checkpoints.At(app->scene->player->level)->data->position;
				app->scene->player->level++;

				app->scene->player->dashAvailable = true;
				app->entityManager->ActivateEnemies();
				app->scene->player->TeleportTo(nextCP);
				app->ui->StartTimer(30000);
			}
			else
			{
				iPoint nextCP = app->scene->checkpoints.At(0)->data->position;
				app->scene->player->level = 1;

				app->scene->player->dashAvailable = true;
				app->entityManager->ActivateEnemies();
				app->scene->player->TeleportTo(nextCP);
				app->ui->StartTimer(30000);
			}
		}

		// Enable/Disable GUI bounds
			if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) guiBounds = !guiBounds;
			

		// Enable/Disable hitboxes/paths
		if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		{
			hitboxes = !hitboxes;
			paths = !paths;
		}

		// Enable/Disable godmode
		if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			godMode = !godMode;
			if (godMode)
			{
				app->scene->player->pbody->body->SetGravityScale(0.0f);
				app->scene->player->pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			}
			else
			{
				app->scene->player->pbody->body->SetGravityScale(1.0f);
			}
		}

		// Enable/Disable FPS limit
		if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) limitFps = !limitFps;

		// Insta-win
		if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
		{
			app->entityManager->ReviveAllEntities();
			app->entityManager->NeedsToSpawnAllEntities();
			app->scene->player->ResetGame();
			app->ftb->SceneFadeToBlack(app->scene, app->scene_win, 0.0f);
		}

		// Insta-kill
		if (app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) app->scene->player->isDead = true;

		// Enable/Disable preference matrix
		if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) preferenceMatrix = !preferenceMatrix;

		// Enable/Disable variables
		if (app->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) variables = !variables;

		// Enable/Disable debug camera
		if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) debugCamera = !debugCamera;

	}
	else
	{
		if (variables != false) variables = false;

		if (hitboxes != false) hitboxes = false;

		if (debugCamera != false) debugCamera = false;

		if (godMode != false) godMode = false;

		if (limitFps != false) limitFps = false;

		if (paths != false) paths = false;

		if (preferenceMatrix != false) preferenceMatrix = false;

		if (originSelected != false) originSelected = false;

	}

	return true;
}

bool Debug::PostUpdate()
{
	if (debug)
	{
		DebugDraw();

		SDL_Rect rect({ 5,5,310,130 });
		app->render->DrawRectangle(rect, 10, 10, 10, 150, true, false);
		rect = SDL_Rect({ 315,5,238,60 });
		app->render->DrawRectangle(rect, 10, 10, 10, 150, true, false);

		app->font->BlitText(10, 10, 0, "Press F1 to GO BACK to the previous level");
		app->font->BlitText(10, 20, 0, "Press F2 to GO FORWARD to the next level");
		app->font->BlitText(10, 30, 0, "Press F3 to RESET the current level");
		app->font->BlitText(10, 40, 0, "Press F4 to DISABLE debug mode");
		app->font->BlitText(10, 50, 0, "Press F5 to SAVE the current game state");
		app->font->BlitText(10, 60, 0, "Press F6 to LOAD the current game state");
		app->font->BlitText(10, 70, 0, "Press F7 to Move to different checkpoints");
		app->font->BlitText(10, 80, 0, "Press F8 to debug GUI bounds and states");
		app->font->BlitText(10, 90, 0, "Press F9 to view COLLIDERS, LOGIC and PATHS");
		app->font->BlitText(10, 100, 0, "Press F10 to activate GOD MODE");
		app->font->BlitText(10, 110, 0, "Press F11 to Enable/Disable FPS cap to");
		app->font->BlitText(10, 120, 0, "frcap(Config)");

		app->font->BlitText(320, 10, 0, "Press I to instantly WIN");
		app->font->BlitText(320, 20, 0, "Press O to KILL the player");
		app->font->BlitText(320, 30, 0, "Press M to show preference map");
		app->font->BlitText(320, 40, 0, "Press C to move the CAMERA freely");
		app->font->BlitText(320, 50, 0, "Press V to show game VARIABLES");
	}
	else
	{
		app->font->BlitText(10, 10, 0, "Press F4 to ENABLE debug mode");
	}

	if (hitboxes) DrawHitboxes();

	return true;
}

void Debug::DebugDraw()
{
	if (preferenceMatrix)
	{
		iPoint currentPos = iPoint(11.0f, 11.0f);

		for (int i = 0; i < app->pathfinding->height; i++)
		{
			for (int j = 0; j < app->pathfinding->width; j++)
			{
				app->font->BlitText(currentPos.x + app->render->camera.x, currentPos.y + app->render->camera.y, 0, std::to_string(app->pathfinding->map[app->pathfinding->width * i + j]).c_str());
				currentPos.x += 32.0f;
			}
			currentPos.x = 11.0f;
			currentPos.y += 32.0f;
		}
	}

	if (variables)
	{
#pragma region Position debug

		app->font->BlitText(10, varBox + 0, 0, "PLAYER");

		app->font->BlitText(20, varBox + 10, 0, "X:");
		app->font->BlitText(100, varBox + 10, 0, std::to_string(app->entityManager->entities.At(0)->data->position.x).c_str());

		app->font->BlitText(20, varBox + 20, 0, "Y:");
		app->font->BlitText(100, varBox + 20, 0, std::to_string(app->entityManager->entities.At(0)->data->position.y).c_str());

		app->font->BlitText(10, varBox + 40, 0, "CAMERA");
		app->font->BlitText(20, varBox + 50, 0, "Free Cam");
		if (!debugCamera)
			app->font->BlitText(90, varBox + 50, 0, "OFF");
		else
			app->font->BlitText(90, varBox + 50, 0, "ON");

		app->font->BlitText(20, varBox + 60, 0, "Camera X:");
		app->font->BlitText(100, varBox + 60, 0, std::to_string(app->render->camera.x).c_str());

		app->font->BlitText(20, varBox + 70, 0, "Camera Y:");
		app->font->BlitText(100, varBox + 70, 0, std::to_string(app->render->camera.y).c_str());

#pragma endregion

#pragma region Normals Debug

		app->font->BlitText(10, varBox + 90, 0, "NORMALS ");

		app->font->BlitText(20, varBox + 100, 0, "X Normal: ");
		app->font->BlitText(100, varBox + 100, 0, std::to_string(app->scene->player->normal_x).c_str());
		app->font->BlitText(20, varBox + 110, 0, "Y Normal: ");
		app->font->BlitText(100, varBox + 110, 0, std::to_string(app->scene->player->normal_y).c_str());

		app->font->BlitText(20, varBox + 120, 0, "Jumps: ");
		app->font->BlitText(100, varBox + 120, 0, std::to_string(app->scene->player->currentJumps).c_str());

		app->font->BlitText(20, varBox + 130, 0, "State: ");
		if (app->scene->player->normal_x == 0.0f && app->scene->player->normal_y == 0.0f) { app->font->BlitText(100, varBox + 130, 0, "AIR"); }
		if (app->scene->player->normal_x == 0.0f && app->scene->player->normal_y == 1.0f) { app->font->BlitText(100, varBox + 130, 0, "CEILING"); }
		if (app->scene->player->normal_x == 0.0f && app->scene->player->normal_y == -1.0f) { app->font->BlitText(100, varBox + 130, 0, "GROUND"); }
		if (app->scene->player->normal_x == 1.0f && app->scene->player->normal_y == 0.0f) { app->font->BlitText(100, varBox + 130, 0, "RIGHT"); }
		if (app->scene->player->normal_x == -1.0f && app->scene->player->normal_y == 0.0f) { app->font->BlitText(100, varBox + 130, 0, "LEFT"); }

		app->font->BlitText(10, varBox + 150, 0, "CURRENT ANIMATION");

		app->font->BlitText(10, varBox + 180, 0, "LEVEL");
		app->font->BlitText(100, varBox + 180, 0, std::to_string(app->scene->player->level).c_str());

#pragma endregion
	}

	if (debugCamera)
	{
		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			app->render->camera.y += 1;

		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			app->render->camera.y -= 1;

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x += 1;

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x -= 1;
	}

#pragma region Pathfinding testing

	if (debug && paths)
	{
		int mouseX, mouseY;
		app->input->GetMousePosition(mouseX, mouseY);
		iPoint mouseTile = app->map->ScreenToMap(mouseX - app->render->camera.x,
			mouseY - app->render->camera.y);

		//Convert again the tile coordinates to world coordinates to render the texture of the tile
		iPoint highlightedTileWorld = app->map->MapToScreen(mouseTile.x, mouseTile.y);
		app->render->DrawTexture(xTex, highlightedTileWorld.x, highlightedTileWorld.y);

		iPoint playerTile = app->map->ScreenToMap(METERS_TO_PIXELS(app->scene->player->pbody->body->GetPosition().x),
			METERS_TO_PIXELS(app->scene->player->pbody->body->GetPosition().y));


		//Player path
		app->pathfinding->CreatePath(mouseTile, playerTile);
		playerPath.Clear();

		const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
		for (uint i = 0; i < path->Count(); i++)
		{
			playerPath.PushBack(iPoint(path->At(i)->x, path->At(i)->y));
		}

		for (uint i = 0; i < playerPath.Count(); ++i)
		{
			iPoint pos = app->map->MapToScreen(playerPath.At(i)->x, playerPath.At(i)->y);
			app->render->DrawTexture(playerPathTex, pos.x, pos.y);
		}

		//Mouse path
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			if (originSelected == true)
			{
				app->pathfinding->CreatePath(origin, mouseTile);
				mousePath.Clear();
				path = app->pathfinding->GetLastPath();
				for (uint i = 0; i < path->Count(); i++)
				{
					mousePath.PushBack(iPoint(path->At(i)->x, path->At(i)->y));
				}

				originSelected = false;
			}
			else
			{
				origin = mouseTile;
				originSelected = true;
				app->pathfinding->ClearLastPath();
			}
		}
		for (uint i = 0; i < mousePath.Count(); ++i)
		{
			iPoint pos = app->map->MapToScreen(mousePath.At(i)->x, mousePath.At(i)->y);
			app->render->DrawTexture(manualPathTex, pos.x, pos.y);
		}
	}

#pragma endregion

	if (limitFps)
	{   // 30 FPS
		if (app->maxFrameDuration != app->frcap) app->maxFrameDuration = app->frcap;
	}
	else
	{	// 60 FPS
		if (app->maxFrameDuration != 16) app->maxFrameDuration = 16;
	}
}

void Debug::DrawHitboxes()
{
	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = app->physics->world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				app->render->DrawCircle(METERS_TO_PIXELS(pos.x + app->render->camera.x), METERS_TO_PIXELS(pos.y + app->render->camera.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)

						app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				app->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				app->render->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}
		}
	}
}
