#include "SDL.h"
#include "Game.h"

Game *Engine = nullptr;


int main(int argc, char* args[]) {

	const int FrameLimit = 60;
	const int FrameDelay = 1000 / FrameLimit;
	Uint32 FrameStart;
	int FrameTime;

	Engine = new Game();

	Engine->Init("Engine", 800, 600, 800, 600, false);

	while (Engine->Running()) {

		FrameStart = SDL_GetTicks();

		Engine->HandleEvents();
		Engine->Update();
		Engine->Render();

		FrameTime = SDL_GetTicks() - FrameStart;

		if (FrameDelay > FrameTime) {
			SDL_Delay(FrameDelay - FrameTime);
		}

	}

	Engine->Clear();

	return 0;
}