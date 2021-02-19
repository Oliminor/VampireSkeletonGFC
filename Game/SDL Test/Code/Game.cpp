#include "Game.h"

#include "Textures.h"

Game::Game() {

}

Game::~Game() {

}

SDL_Texture* TestTexture;
SDL_Rect Source, Size;


void Game::Init(const char* Name, int X, int Y, int SizeX, int SizeY, bool FullScreen) {

	int Flags = 0;

	if (FullScreen) {
		Flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		cout << "Init" << endl;
		Window = SDL_CreateWindow(Name, X, Y, SizeX, SizeY, Flags);
		if(Window) {
			cout << "Create window" << endl;
		}
		g = SDL_CreateRenderer(Window, -1, 0);
		if (g) {
			SDL_SetRenderDrawColor(g, 255, 255, 255, 255);
			cout << "Create Render" << endl;
		}

		IsRunning = true;
	}
	else {
		IsRunning = false;
	}

	TestTexture = TextureManager::LoadTexture("Assets/Images/TestImages.png", g);

}

void Game::HandleEvents() {
	SDL_Event Events;
	SDL_PollEvent(&Events);
	switch (Events.type)
	{
	case SDL_QUIT:
		IsRunning = false;
	default:
		break;
	}
}

void Game::Update() {
	Size.h = 32;
	Size.w = 32;
	Size.x = 32;
}

void Game::Render() {

	SDL_RenderClear(g);
	SDL_RenderCopy(g, TestTexture, NULL, &Size);
	SDL_RenderPresent(g);
}

void Game::Clear() {
	SDL_DestroyWindow(Window);
	SDL_DestroyRenderer(g);
	SDL_Quit();
	cout << "Closed and cleaned" << endl;
}