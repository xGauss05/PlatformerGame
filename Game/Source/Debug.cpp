#include "Debug.h"

#include "App.h"
#include "Input.h"
#include "Render.h"

#include "Player.h"
#include "Render.h"
#include "Fonts.h"
#include "EntityManager.h"
#include "Physics.h"
#include <string>
using namespace std;

Debug::Debug() : Module() {
	debug = false;
}
Debug::~Debug() {
}

bool Debug::Start() {
	debug = false;
	return true;
}

bool Debug::Update(float dt) {

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		debug = !debug;

	if (debug) {
		if (app->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			variables = !variables;

		if (app->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
			hitboxes = !hitboxes;

		if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			debugCamera = !debugCamera;
	}

	return true;
}

bool Debug::PostUpdate() {

	if (debug)
	{
		DebugDraw();
	}


	return true;
}

void Debug::DebugDraw() {
	
	app->font->BlitText(10, + 20, 0, "PRESS V FOR VARIABLES");
	if (variables) {

		//Position debug
		app->font->BlitText(10, varBox + 30, 0, "PLAYER");

		app->font->BlitText(10, varBox + 40, 0, "X.");
		app->font->BlitText(25, varBox + 40, 0, std::to_string(app->entityManager->entities.At(0)->data->position.x).c_str());

		app->font->BlitText(10, varBox + 50, 0, "Y.");
		app->font->BlitText(25, varBox + 50, 0, std::to_string(app->entityManager->entities.At(0)->data->position.y).c_str());

		app->font->BlitText(10, 60, 0, "FREE CAM");
		if (!debugCamera)
			app->font->BlitText(80, 60, 0, "OFF");
		else
			app->font->BlitText(80, 60, 0, "ON");

		app->font->BlitText(10, varBox + 70, 0, "CAMERA X.");
		app->font->BlitText(80, varBox + 70, 0, std::to_string(app->render->camera.x).c_str());

		app->font->BlitText(10, varBox + 80, 0, "CAMERA Y.");
		app->font->BlitText(80, varBox + 80, 0, std::to_string(app->render->camera.y).c_str());
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

	app->font->BlitText(10, + 40, 0, "PRESS H FOR HITBOXES");
	if (hitboxes)
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
}