#pragma once

class CMyGame : public CGame
{
public:

	CSprite RenderTiles[28][28];//Render Tiles is here so we can have a smaller list of CSprites and not chew through ram.
	int  WorldTiles[64][64];//This is the world tile data. ATM the world is stuck at 64 by 64. this should be changed
	char* TileSet[64] = { "Air" };//Creates the tile set and sets it all to "air"
	bool SmoothScrolling = true;//This toggles smooth moving of the world. if false the world will jitter

	CSprite Player;//Player sprite

	CSprite MainMenuScreen;

	CSpriteList Backgrounds;

	void LoadMap(char* TileMap);

	void SaveMap();

	void LoadLevel(int);

public:

	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
