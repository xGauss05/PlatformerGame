#include "FadeToBlack.h"

#include "App.h"
#include "Render.h"

#include "SDL/include/SDL_render.h"

FadeToBlack::FadeToBlack() : Module() {
	screenRect = { 0, 0, 1600 * 1, 900 * 1 };
}

FadeToBlack::~FadeToBlack() {}

bool FadeToBlack::Start() {
	//LOG("Preparing Fade Screen");

	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool FadeToBlack::Update(float dt) {
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	if (currentStep == Fade_Step::TO_BLACK) {
		++frameCount;
		if (frameCount >= maxFadeFrames) {

			moduleToDisable->Disable();
			moduleToEnable->Enable();

			currentStep = Fade_Step::FROM_BLACK;
		}
	} else {
		--frameCount;
		if (frameCount <= 0) {
			currentStep = Fade_Step::NONE;
		}
	}

	return true;
}

bool FadeToBlack::PostUpdate() {
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	return true;
}

bool FadeToBlack::SceneFadeToBlack(Module* moduleToDisable, Module* moduleToEnable, float frames) {
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if (currentStep == Fade_Step::NONE) {
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		this->moduleToEnable = moduleToEnable;
		this->moduleToDisable = moduleToDisable;

		ret = true;
	}

	return ret;
}

bool FadeToBlack::CleanUp() {

	return true;
}
