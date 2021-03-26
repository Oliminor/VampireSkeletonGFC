#pragma once

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSprite bg, moon, forest, castle, hills, wood;
	CSprite vampire, skeleton, camera, pointer;
	CSprite tiles;
	char* tileSet[20];
	CSprite RenderTiles[128][7];
	CSprite blackBG;
	CSprite finalScreen;

	CSprite MapEditor;
	bool InEditor = false;
	int EditorX = 0;
	int EditorY = 0;
	int EditorID = 1;

	bool GameOver = false;
	float TimeAtReContinue = GetDeltaTime();
	int DeathAnimationState = 0;
	char* CurrentLevelPath;
	
	CSpriteList leverList;
	CSpriteList particleList;
	CSpriteList rainList;
	CSpriteList loreList;

	CSoundPlayer bgNoise;
	CSoundPlayer music;
	CSoundPlayer sfx01;

	int size = 0;
	int tempID = 0;
	int mapSize = 128;
	int currentLevel = 1;
	int currentTutorial = 0;

	bool playerBool = true, leftBool = false, rightBool = false, jumpBool = false, animationBool = true;
	bool idleBool = true, runninBool = false, jumpHeightBool = false, runAnimationBool = false;
	bool skeletonAlign = true, vampireAlign = true, transformBool = false, transformAnimationBool = false;
	bool leverBool = false, pushBool = false, pushAnimBool = false, pointerBool = true;
	bool levelBool = true, startBool = false, finalBool = false, topBool = false;
	bool tutorialBool = false;

	float transformCD = 0;
	float switchCD = 0;
	float platformCD = 0;
	float mapStart = 0, mapEnd = 0;

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	virtual void OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle);
	virtual void OnLButtonDown(Uint16 x, Uint16 y);

	void LoadMap(char* TileMap, char* MapToLoad);
	void SaveMap();
	void VampireAnimation();
	void SkeletonAnimation();
	void AnimationRefresh();
	void CharacterMovement();
	void CharacterJump();
	void CharacterJump(CSprite* Character);
	void changeCharacter();
	void ParticleSystem();
	void MapScroll(CGraphics* g, CSprite& map, CSprite& camera, float divide, float limiter, float mapStart, float mapEnd);
	void LeverSystem(CSprite& _player);
	void LevelOneLore(CGraphics* g);
	void TransformationSystem();
	void JumpOnSkeleton();
	void PlatformCollision(CSprite& player, CSprite& platform);
	void PlatformTopCollision(CSprite& platform);
	void PushBox(CSprite& player, CSprite& platform);
	void Pointer(CSprite& player);
	void LeverPlatform(CSprite& platform);
	CVector VectorLerp(CVector targetA, CVector targetB, float time);
	float FloatLerp(float floatX, float floatY, float time);
	void CharacterDeath(CSprite& CharacterToDie, CSprite& CharacterToLive);
};