#include "Textures.h"

SDL_Texture* TextureManager::LoadTexture(const char* Texture, SDL_Renderer* Ren)
{
	SDL_Surface* TMPSurface = IMG_Load(Texture);
	SDL_Texture* Tex = SDL_CreateTextureFromSurface(Ren, TMPSurface);
	SDL_FreeSurface(TMPSurface);
	
	return Tex;
}