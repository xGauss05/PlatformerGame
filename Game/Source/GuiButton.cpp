#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	audioFxId = app->audio->LoadFx("Assets/Audio/Fx/metal.wav");
}

GuiButton::~GuiButton()
{

}

bool GuiButton::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		
		app->input->GetMousePosition(mouseX, mouseY);

		GuiControlState previousState = state;

		// I'm inside the limits of the button
		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {
			
			state = GuiControlState::FOCUSED;
			if (previousState != state) 
			{
				LOG("Change state from %d to %d",previousState,state);
				app->audio->PlayFx(audioFxId);
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) 
			{
				state = GuiControlState::PRESSED;
			}

			//
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP) 
			{
				NotifyObserver();
			}
		}
		else 
		{
			state = GuiControlState::NORMAL;
		}
	}

	return false;
}

bool GuiButton::Draw(Render* render)
{
	switch (state)
	{
	case GuiControlState::DISABLED:
		render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
		break;
	case GuiControlState::NORMAL:
		render->DrawRectangle(bounds, 170, 0, 0, 255, true, false);
		break;
	case GuiControlState::FOCUSED:
		render->DrawRectangle(bounds, 255, 0, 0, 255, true, false);
		break;
	case GuiControlState::PRESSED:
		render->DrawRectangle(bounds, 60, 0, 0, 255, true, false);
		break;
	}

	app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h, {255,255,255});

	return false;
}