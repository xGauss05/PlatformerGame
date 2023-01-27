#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "Textures.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	selectFx = app->audio->LoadFx("Assets/Audio/Fx/metal.wav");
	hoverFx = app->audio->LoadFx("Assets/Audio/Fx/hover.wav");

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

		// I'm inside the limitis of the button
		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) 
		{

			state = GuiControlState::FOCUSED;
			if (previousState != state) 
			{
				LOG("Change state from %d to %d", previousState, state);
				if (state == GuiControlState::FOCUSED) app->audio->PlayFx(hoverFx);
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) 
			{
				state = GuiControlState::PRESSED;
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP) 
			{
				NotifyObserver();
				app->audio->PlayFx(selectFx);
			}
		}
		else 
		{
			state = GuiControlState::NORMAL;
		}
	}

	return false;
}

void GuiButton::SetFocusedTexture(SDL_Texture* tex)
{
	focusedTexture = tex;
}

void GuiButton::SetPressedTexture(SDL_Texture* tex)
{
	pressedTexture = tex;
}

bool GuiButton::Draw(Render* render)
{
	if (texture == NULL)
	{
		switch (state)
		{
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
		if (state != GuiControlState::DISABLED)
		{
			app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h, { 255,255,255 });
		}
	}
	else
	{
		switch (state)
		{
		case GuiControlState::NORMAL:
			render->DrawTexture(texture, bounds.x, bounds.y);
			break;
		case GuiControlState::FOCUSED:
			if (focusedTexture != NULL) render->DrawTexture(focusedTexture, bounds.x, bounds.y);
			break;
		case GuiControlState::PRESSED:
			if (pressedTexture != NULL) render->DrawTexture(pressedTexture, bounds.x, bounds.y);
			break;
		}
	}

	return false;
}
