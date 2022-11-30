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

	playerPathTex = app->tex->Load("Assets/Maps/path.png");
	mousePathTex = app->tex->Load("Assets/Maps/manualPath.png");
	xTex = app->tex->Load("Assets/Maps/x.png");

	return true;
}

bool Debug::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) debug = !debug;

	if (debug)
	{
		if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			if (app->scene->player->level > 1)
			{
				app->scene->player->level--;
				app->scene->player->LevelSelector();
				app->scene->player->TeleportTo(app->scene->player->spawn);
			}
		}
		//Pass level
		if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			if (app->scene->player->level < 4)
			{
				app->scene->player->level++;
				app->scene->player->LevelSelector();
				app->scene->player->TeleportTo(app->scene->player->spawn);
			}
		}
		if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) variables = !variables;

		if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) debugCamera = !debugCamera;
		if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		{
			app->scene->player->TeleportTo(app->scene->player->spawn);
		}
		if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) hitboxes = !hitboxes;

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
	}

	

	

	return true;
}

bool Debug::PostUpdate()
{

	if (debug)
	{
		app->font->BlitText(10, 10, 0, "Press F5 to SAVE the current game state");
		app->font->BlitText(10, 20, 0, "Press F6 to LOAD the current game state");
		app->font->BlitText(10, 30, 0, "Press F7 to show game VARIABLES");
		app->font->BlitText(10, 40, 0, "Press F8 to move the CAMERA freely");
		app->font->BlitText(10, 50, 0, "Press F9 to view COLLIDERS");
		app->font->BlitText(10, 60, 0, "Press F10 to activate GOD MODE");

		DebugDraw();
	}

	if (hitboxes)
	{
		DrawHitboxes();
	}

	return true;
}

void Debug::DebugDraw()
{

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

	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);
	iPoint mouseTile = app->map->ScreenToMap(mouseX - app->render->camera.x,
											 mouseY - app->render->camera.y);

	//Convert again the tile coordinates to world coordinates to render the texture of the tile
	iPoint highlightedTileWorld = app->map->MapToScreen(mouseTile.x, mouseTile.y);
	app->render->DrawTexture(xTex, highlightedTileWorld.x, highlightedTileWorld.y);

	iPoint playerTile = app->map->ScreenToMap(METERS_TO_PIXELS(app->scene->player->pbody->body->GetPosition().x),
											  METERS_TO_PIXELS(app->scene->player->pbody->body->GetPosition().y));

	app->font->BlitText(20, 200, 0, std::to_string(playerTile.x).c_str());
	app->font->BlitText(20, 210, 0, std::to_string(playerTile.y).c_str());


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
		app->render->DrawTexture(mousePathTex, pos.x, pos.y);
	}

#pragma endregion
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
