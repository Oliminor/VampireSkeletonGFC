#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>

using namespace std;

class Game {
private:

	bool IsRunning = true;
	
	SDL_Window* Window;
	SDL_Renderer* g;

public:
	Game();
	~Game();
	
	void Init(const char* Name, int X, int Y, int SizeX, int SizeY, bool FullScreen);

	void HandleEvents();

	void Update();

	void Render();

	void Clear();

	bool Running() { return IsRunning; };

};