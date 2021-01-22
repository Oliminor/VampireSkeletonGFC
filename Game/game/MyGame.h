#pragma once

class CMyGame : public CGame
{
public:

	CSprite RayCaster;

	CSprite RenderTiles[16][16];
	int  WorldTiles[64][64];
	char* TileSet[64] = { "Crate" };

	CSprite Player;
	int PlayerSpeed = 480;
	int PlayerLevel = 1;
	int PlayerScore = 0;

	CSprite Camera;
	int CameraDX = 0;
	int CameraDY = 0;

	CSpriteList WorldObjects;

	CSpriteList EnemyList;

	CSprite MainMenuScreen;

	void PlayerControl();

	void EnemyAI(CSprite*, int t);

	void LoadMap();

	void LoadLevel(int);

	void PlaceObject(char* Image, int X, int Y, int Collision);

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
