#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void)
{

}

CMyGame::~CMyGame(void)
{

}

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	if (transformCD <= 0 && switchCD <= 0 && !levelBool && !finalBool) CharacterMovement();

	camera.Update(t);
	vampire.Update(t);
	skeleton.Update(t);
	blackBG.Update(t);

	if (GameOver) {
		if (DeathAnimationState == 1) {
			if (vampire.GetHealth() == -1) {
				vampire.SetAnimation("ReviveStatic");
				vampire.SetHealth(0);
			}
			else if (skeleton.GetHealth() == -1){
				skeleton.SetHealth(0);
				skeleton.SetAnimation("ReviveStatic");
			}
		}
		if (DeathAnimationState == 2) {
			if (TimeAtReContinue + 1600 < GetTime()) {
				if (vampire.GetHealth() == 0) {
					vampire.SetAnimation("IdleRight");
				}
				else if (skeleton.GetHealth() == 0) {
					skeleton.SetAnimation("IdleRight");
				}
				DeathAnimationState = 4;
				TimeAtReContinue = GetTime();
			}
		}
		else if (DeathAnimationState == 4) {
			if (TimeAtReContinue + 750 < GetTime()) {
				CharacterJump();
				DeathAnimationState = 5;
				TimeAtReContinue = GetTime();
			}
		}
		else if (DeathAnimationState == 5){
			if(TimeAtReContinue + 300 < GetTime()) {
				DeathAnimationState = 6;
				TimeAtReContinue = GetTime();
			}
		}
		else if (DeathAnimationState == 6) {
			if (TimeAtReContinue + 500 < GetTime()) {
				vampire.SetAnimation("RunRight");
				skeleton.SetAnimation("RunRight");
				DeathAnimationState = 7;
				TimeAtReContinue = GetTime();
			}
		}
		else if (DeathAnimationState == 7) {
			vampire.SetPos(vampire.GetX() + 10, vampire.GetY());
			skeleton.SetPos(skeleton.GetX() + 10, skeleton.GetY());
			if (TimeAtReContinue + 2000 < GetTime()) {
				vampire.SetAnimation("IdleRight");
				skeleton.SetAnimation("IdleRight");
				LoadMap("level01Tiles.bmp", CurrentLevelPath);
				GameOver = false;
				DeathAnimationState = 0;
			}
		}
	}

		if (InEditor) {
			MapEditor.Update(t);
		}

		for (CSprite* rains : rainList)
		{
			rains->Update(t);
			rains->SetSize(20, 20);
			rains->SetHealth(rains->GetHealth() - 4);
			if (rains->GetHealth() <= 0)
			{
				rains->Delete();
			}
		}
		rainList.delete_if(deleted);

		vampire.SetSize(size * 1.2, size * 1.2);
		skeleton.SetSize(size * 1.2, size * 1.2);

		if (!transformBool) vampire.Accelerate(0, -(size / 4));
		skeleton.Accelerate(0, -(size / 4));

		ParticleSystem();
		TransformationSystem();
		JumpOnSkeleton();

		if (playerBool)
		{
			LeverSystem(vampire);
			Pointer(vampire);
		}
		else
		{
			LeverSystem(skeleton);
			Pointer(skeleton);
		}

		for (int x = 0; x < mapSize; x++)
		{
			for (int y = 0; y < 7; y++)
			{
				RenderTiles[x][y].Update(t);

				//Side Collision
				PlatformCollision(vampire, RenderTiles[x][y]);
				PlatformCollision(skeleton, RenderTiles[x][y]);

				//Top Collision
				PlatformTopCollision(RenderTiles[x][y]);

				//Push box
				PushBox(skeleton, RenderTiles[x][y]);

				//Lever activated platform
				LeverPlatform(RenderTiles[x][y]);
			}
		}
		platformCD += 10;

		if (switchCD < 0)
		{
			vampire.Update(t);
			skeleton.Update(t);

			vampire.SetSize(size * 1.2, size * 1.2);
			skeleton.SetSize(size * 1.2, size * 1.2);
		}

}

void CMyGame::OnDraw(CGraphics* g)
{
	static float distFromStart = 0;
	static float distLimiter = 0;
	static float limiter = 0;
	distFromStart = camera.GetX();

	float distantV = vampire.GetX();
	float distantS = skeleton.GetX();
	if (GameOver) {

		vampire.SetSize(256, 256);
		skeleton.SetSize(256, 256);

		if (DeathAnimationState <= 2) {
			if (vampire.GetHealth() == 0) {
				vampire.SetSize(256 * 2, 256);
			}
			else if (skeleton.GetHealth() == 0) {
				skeleton.SetSize(256 * 2, 256);
			}
		}
		
		vampire.Draw(g);
		skeleton.Draw(g);
		if (DeathAnimationState == 1) {
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [Space] To Continue";
		}
		
	}
	else {
		if (distFromStart < mapStart) distLimiter = mapStart;
		else if (distFromStart > mapEnd - size * 5) distLimiter = mapEnd;

		if (playerBool)
		{
			if (distantV < mapStart) limiter = mapStart;
			else if (distantV > mapEnd - size * 5) limiter = mapEnd;

			if (distantV > mapStart && distantV < mapEnd)
			{
				float lerp = FloatLerp(vampire.GetX(), camera.GetX(), 0.91);
				camera.SetX(lerp);
				vampire.SetXVelocity(0);
			}
			else
			{
				float lerp = FloatLerp(limiter, camera.GetX(), 0.91);
				camera.SetX(lerp);
			}

		}
		else
		{
			if (distantS < mapStart) limiter = mapStart;
			else if (distantS > mapEnd - size * 5) limiter = mapEnd;

			if (distantS > mapStart && distantS < mapEnd)
			{
				float lerp = FloatLerp(skeleton.GetX(), camera.GetX(), 0.91);
				camera.SetX(lerp);
				skeleton.SetXVelocity(0);
			}
			else
			{
				float lerp = FloatLerp(limiter, camera.GetX(), 0.91);
				camera.SetX(lerp);
			}
		}

		bg.Draw(g);

		MapScroll(g, moon, camera, 20, distLimiter, mapStart, mapEnd);
		MapScroll(g, forest, camera, 6, distLimiter, mapStart, mapEnd);
		MapScroll(g, castle, camera, 3, distLimiter, mapStart, mapEnd);
		MapScroll(g, hills, camera, 2, distLimiter, mapStart, mapEnd);
		if (distFromStart >= mapStart && distFromStart <= mapEnd)
			g->SetScrollPos(-camera.GetX() + GetWidth() / 2, 0);
		else
			g->SetScrollPos(-distLimiter + GetWidth() / 2, 0);

		for (int x = 0; x < mapSize; x++)
		{
			for (int y = 0; y < 7; y++)
			{
				if ((RenderTiles[x][y].GetState() < 20 || RenderTiles[x][y].GetState() == 100) && RenderTiles[x][y].GetState() != 5) 
					RenderTiles[x][y].Draw(g);
			}
		}

		for (int x = 0; x < mapSize; x++)
		{
			for (int y = 0; y < 7; y++)
			{
				if (RenderTiles[x][y].GetState() == 5)
					RenderTiles[x][y].Draw(g);
			}
		}

		skeleton.Draw(g);
		vampire.Draw(g);

		if (pointerBool)
			pointer.Draw(g);

		if (InEditor) {
			MapEditor.Draw(g);
		}

		for (CSprite* levers : leverList) levers->Draw(g);

		for (CSprite* particles : particleList) particles->Draw(g);

		for (CSprite* rains : rainList) rains->Draw(g);

		g->SetScrollPos(0, 0);
		if (distFromStart > mapStart && distFromStart < mapEnd)
			g->SetScrollPos(-camera.GetX() * 1.3, 0);
		else
			g->SetScrollPos(-distLimiter * 1.3, 0);

		wood.Draw(g);
		g->SetScrollPos(0, 0);

		if (finalBool)
		{
			finalScreen.Draw(g);
			skeleton.Draw(g);
			vampire.Draw(g);

			skeleton.SetX(GetWidth() / 3);
			vampire.SetX(GetWidth() / 4);

			for (CSprite* particles : particleList) particles->Draw(g);

			*g << xy(GetWidth() / 3.3, GetHeight() / 1.3) << font("ChavaRegular.ttf", size / 1.5) << color(CColor::White()) << "Prototype Over";
			*g << xy(GetWidth() / 3.3, GetHeight() / 1.5) << font("ChavaRegular.ttf", size / 2.1) << color(CColor::White()) << "Thank you for playing!";
			*g << xy(GetWidth() / 2.1, GetHeight() / 1.9) << font("ChavaRegular.ttf", size / 2.1) << color(CColor::White()) << "Credits";
			*g << xy(GetWidth() / 2.2, GetHeight() / 2.2) << font("ChavaRegular.ttf", size / 2.3) << color(CColor::White()) << "Bence Nagy";
			*g << xy(GetWidth() / 2.5, GetHeight() / 2.6) << font("ChavaRegular.ttf", size / 2.3) << color(CColor::White()) << "Ewan Hart Prieto";
			*g << xy(GetWidth() / 2.2, GetHeight() / 3.15) << font("ChavaRegular.ttf", size / 2.3) << color(CColor::White()) << "Joe Simpson";
			*g << xy(GetWidth() / 2.45, GetHeight() / 4.0) << font("ChavaRegular.ttf", size / 2.3) << color(CColor::White()) << "Pratik M. Parmar";
			*g << xy(GetWidth() / 2.6, GetHeight() / 5.4) << font("ChavaRegular.ttf", size / 2.3) << color(CColor::White()) << "William Everingham";
		}

		if (camera.GetX() > size * 0 && !levelBool && !tutorialBool && currentTutorial == 0)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [A] or [D] to move";
		}
		else if (camera.GetX() > size * 10 && !levelBool && !tutorialBool && currentTutorial == 1)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [Space] to Jump";
		}
		else if (camera.GetX() > size * 22 && !levelBool && !tutorialBool && currentTutorial == 2)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [Tab] to change character";
		}
		else if (camera.GetX() > size * 22 && !levelBool && !tutorialBool && currentTutorial == 3)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Jump on Noteleks head with Yalar";
		}
		else if (camera.GetX() > size * 32 && !levelBool && !tutorialBool && currentTutorial == 4)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [E] to transform into bat with Yalar";
		}
		else if (camera.GetX() > size * 41 && !levelBool && !tutorialBool && currentTutorial == 5 && transformBool)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [E] to transform back";
		}
		else if (camera.GetX() > size * 41 && !levelBool && !tutorialBool && currentTutorial == 6 && !transformBool)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Press [F] to pull the lever";
		}
		else if (camera.GetX() > size * 45 && !levelBool && !tutorialBool && currentTutorial == 7)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "Push the boulder with Noteleks";
		}

		LevelOneLore(g);

		static int prevT = GetTime();
		if (GetTime() > prevT)
			//*g << left << xy(GetWidth() - 170, GetHeight() - 50) << font("ChavaRegular.ttf", 30) << color(CColor::White()) << "fps = " << (int)(1000 / (GetTime() - prevT));
		prevT = GetTime();

		//*g << left << xy(GetWidth() - 170, GetHeight() - 100) << font("ChavaRegular.ttf", 30) << color(CColor::White()) << camera.GetX();

		static bool skeletonFinish = false;
		static bool vampireFinish = false;
		static bool finishBool = false;

		if (vampire.GetX() > mapEnd + 350) vampireFinish = true;
		else vampireFinish = false;
		if (skeleton.GetX() > mapEnd + 350) skeletonFinish = true;
		else skeletonFinish = false;

		if (((playerBool && vampireFinish) || (!playerBool && skeletonFinish)) && ((vampireFinish && !skeletonFinish) || (!vampireFinish && skeletonFinish)))
		{
			*g << xy(GetWidth() / 10, GetHeight() / 2) << font("ChavaRegular.ttf", size / 3) << color(CColor::White()) << "To finish this level both character should be here";
		}

		if (vampireFinish && skeletonFinish && !finishBool)
		{
			currentLevel++;
			levelBool = true;
			finishBool = true;
			blackBG.SetX(GetWidth() / 2 + GetWidth() * 2.5);
			runninBool = false;
			idleBool = true;
			startBool = false;
			skeleton.SetXVelocity(0);
			vampire.SetXVelocity(0);
			AnimationRefresh();
			sfx01.Stop();
		}
		else if (!vampireFinish && !skeletonFinish)
		{
			finishBool = false;
		}
	}
}

void CMyGame::OnInitialize()
{
	size = GetHeight() / 7;

	bg.LoadImageW("level01Background.bmp", CColor::White());
	bg.SetImage("level01Background.bmp");
	bg.SetPosition(GetWidth() / 2, GetHeight() / 2);
	bg.SetSize(GetWidth(), GetHeight());

	moon.LoadImageW("level01Layer4.bmp", CColor::White());
	moon.LoadImageW("level02Layer4.bmp", CColor::White());
	moon.SetImage("level02Layer4.bmp");
	moon.SetPosition(GetWidth() / 1.6, GetHeight() / 2);
	moon.SetSize(GetWidth() * 2, GetHeight());

	forest.LoadImageW("level01Layer3.bmp", CColor::White());
	forest.LoadImageW("level02Layer3.bmp", CColor::White());
	forest.SetImage("level02Layer3.bmp");
	forest.SetPosition(GetWidth(), GetHeight() / 2);
	forest.SetSize(GetWidth() * 2, GetHeight());

	castle.LoadImageW("level01Layer2.bmp", CColor::White());
	castle.LoadImageW("level02Layer2.bmp", CColor::White());
	castle.SetImage("level02Layer2.bmp");
	castle.SetPosition(GetWidth(), GetHeight() / 2);
	castle.SetSize(GetWidth() * 2, GetHeight());

	hills.LoadImageW("level01Layer1.bmp", CColor::White());
	hills.LoadImageW("level02Layer1.bmp", CColor::White());
	hills.SetImage("level02Layer1.bmp");
	hills.SetPosition(GetWidth() * 1.5, GetHeight() / 2);
	hills.SetSize(GetWidth() * 2, GetHeight());

	wood.LoadImageW("level01Foreground.bmp", CColor::White());
	wood.SetImage("level01Foreground.bmp");
	wood.SetPosition(GetWidth() * 2, GetHeight() / 2);
	wood.SetSize(GetWidth(), GetHeight());

	vampire.SetHealth(100);

	vampire.AddImage("VampireIdleLeft.bmp", "IdleLeft", 5, 1, 0, 0, 5, 0, CColor::White());
	vampire.AddImage("VampireRunLeft.bmp", "RunLeft", 10, 1, 0, 0, 10, 0, CColor::White());
	vampire.AddImage("VampireIdleRight.bmp", "IdleRight", 5, 1, 0, 0, 5, 0, CColor::White());
	vampire.AddImage("VampireRunRight.bmp", "RunRight", 10, 1, 0, 0, 10, 0, CColor::White());
	vampire.AddImage("VampireJumpUpRight.bmp", "JumpUpRight", CColor::White());
	vampire.AddImage("VampireJumpUpLeft.bmp", "JumpUpLeft", CColor::White());
	vampire.AddImage("VampireJumpDownRight.bmp", "JumpDownRight", CColor::White());
	vampire.AddImage("VampireJumpDownLeft.bmp", "JumpDownLeft", CColor::White());
	vampire.AddImage("VampireTransformRight.bmp", "TransformRight", 11, 1, 0, 0, 11, 0, CColor::White());
	vampire.AddImage("VampireTransformLeft.bmp", "TransformLeft", 11, 1, 0, 0, 11, 0, CColor::White());
	vampire.AddImage("VampireReverseTransformRight.bmp", "ReverseTransformRight", 11, 1, 0, 0, 11, 0, CColor::White());
	vampire.AddImage("VampireReverseTransformLeft.bmp", "ReverseTransformLeft", 11, 1, 0, 0, 11, 0, CColor::White());
	vampire.AddImage("VampireBatRight.bmp", "BatRight", 5, 1, 0, 0, 5, 0, CColor::White());
	vampire.AddImage("VampireBatLeft.bmp", "BatLeft", 5, 1, 0, 0, 5, 0, CColor::White());
	vampire.AddImage("leverVampire.bmp", "leverVampire", 9, 1, 0, 0, 9, 0, CColor::White());
	vampire.AddImage("VampireRevive.bmp", "ReviveStatic", 16, 1, 0, 0, 0, 0, CColor::White());
	vampire.AddImage("VampireRevive.bmp", "Revive", 16, 1, 0, 0, 16, 0, CColor::White());
	vampire.SetAnimation("IdleRight");
	vampire.SetPos(GetWidth() / 2, size + size / 3);

	skeleton.SetHealth(100);

	skeleton.AddImage("SkeletonIdleLeft.bmp", "IdleLeft", 7, 1, 0, 0, 7, 0, CColor::White());
	skeleton.AddImage("SkeletonRunningLeft.bmp", "RunLeft", 9, 1, 0, 0, 9, 0, CColor::White());
	skeleton.AddImage("SkeletonIdleRight.bmp", "IdleRight", 7, 1, 0, 0, 7, 0, CColor::White());
	skeleton.AddImage("SkeletonRunningRight.bmp", "RunRight", 9, 1, 0, 0, 9, 0, CColor::White());
	skeleton.AddImage("SkeletonJumpUpRight.bmp", "JumpUpRight", CColor::White());
	skeleton.AddImage("SkeletonJumpUpLeft.bmp", "JumpUpLeft", CColor::White());
	skeleton.AddImage("SkeletonJumpDownRight.bmp", "JumpDownRight", CColor::White());
	skeleton.AddImage("SkeletonJumpDownLeft.bmp", "JumpDownLeft", CColor::White());
	skeleton.AddImage("leverSkeleton.bmp", "leverSkeleton", 9, 1, 0, 0, 9, 0, CColor::White());
	skeleton.AddImage("SkeletonPushLeft.bmp", "pushLeft", 7, 1, 0, 0, 7, 0, CColor::White());
	skeleton.AddImage("SkeletonPushRight.bmp", "pushRight", 7, 1, 0, 0, 7, 0, CColor::White());
	skeleton.LoadImageW("SkeletonTopLeft.bmp", CColor::White());
	skeleton.LoadImageW("SkeletonTopRight.bmp", CColor::White());
	skeleton.AddImage("SkeletonRevive.bmp", "ReviveStatic", 16, 1, 0, 0, 0, 0, CColor::White());
	skeleton.AddImage("SkeletonRevive.bmp", "Revive", 16, 1, 0, 0, 16, 0, CColor::White());
	skeleton.SetAnimation("IdleRight");
	skeleton.SetPosition((GetWidth() / 2) - 200, size + size / 3);

	camera.SetPosition(vampire.GetPosition());

	pointer.LoadImageW("pointer.bmp", CColor::White());
	pointer.SetImage("pointer.bmp");
	pointer.SetPosition(vampire.GetX(), vampire.GetY() + size);

	blackBG.LoadImageW("black.bmp", CColor::White());
	blackBG.SetImage("black.bmp");
	blackBG.SetSize(GetWidth() * 2, GetHeight());
	blackBG.SetPosition(GetWidth() / 2, GetHeight() / 2);

	finalScreen.LoadImageW("EntranceBg.bmp", CColor::White());
	finalScreen.SetImage("EntranceBg.bmp");
	finalScreen.SetSize(GetWidth(), GetHeight());
	finalScreen.SetPosition(GetWidth() / 2, GetHeight() / 2);

	MapEditor.LoadImage("MapEditorTile.bmp",CColor::White());
	MapEditor.SetImage("MapEditorTile.bmp", CColor::White());
	MapEditor.SetSize(128, 128);

	//Load Tiles
	LoadMap("level01Tiles.bmp", "MapData.txt");

	rightBool = true;

	music.Play("dracballs.ogg", -1);
	music.Volume(0.05);

	bgNoise.Play("rainSound.ogg", -1);
	bgNoise.Volume(0.4);

	mapStart = size * 8;
	mapEnd = size * 60;
}

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (!levelBool && DeathAnimationState==0)
	{
		if (sym == SDLK_SPACE)
		{
			if (currentTutorial == 1)
			{
				currentTutorial++;
			}
		}

		if (transformCD <= 0 && switchCD <= 0)
		{
			if (sym == SDLK_a)
			{
				if (currentTutorial == 0)
				{
					currentTutorial++;
				}

				if (IsKeyDown(SDLK_d))
				{
					runninBool = true;
					leftBool = false;
					rightBool = true;
					AnimationRefresh();
				}
				else
				{
					leftBool = true;
					rightBool = false;
					idleBool = false;
					runninBool = true;
					AnimationRefresh();
				}
			}

			if (sym == SDLK_d)
			{

				if (currentTutorial == 0)
				{
					currentTutorial++;
				}

				if (IsKeyDown(SDLK_a))
				{
					pushBool = false;
				}
				rightBool = true;
				leftBool = false;
				idleBool = false;
				runninBool = true;
				AnimationRefresh();
			}
		}
		if (sym == SDLK_f)
		{
			sfx01.Play("lever.wav", 0);
			if (currentTutorial == 6)
			{
				currentTutorial++;
			}

			for (CSprite* levers : leverList)
			{
				if (vampire.HitTest(levers) && playerBool && levers->GetState() % 20 == 0 && !transformBool)
				{
					levers->SetAnimation("Lever", 9);
					vampire.SetAnimation("leverVampire", 9);
					vampire.SetPosition(levers->GetPosition());
					switchCD = 1;

					transformAnimationBool = true;
					leverBool = true;

					for (int x = 0; x < mapSize; x++)
					{
						for (int y = 0; y < 7; y++)
						{
							if (RenderTiles[x][y].GetState() == levers->GetState())
							{
								RenderTiles[x][y].SetState(100);
								RenderTiles[x][y].SetY(RenderTiles[x][y].GetY() - 1500);
								platformCD = 0;
							}
						}
					}
					levers->SetState(levers->GetState() + 1);
				}

				if (skeleton.HitTest(levers) && !playerBool && levers->GetState() % 20 == 0)
				{
					levers->SetAnimation("Lever", 9);
					skeleton.SetAnimation("leverSkeleton", 9);
					skeleton.SetPosition(levers->GetPosition());
					switchCD = 1;

					transformAnimationBool = true;
					leverBool = true;

					for (int x = 0; x < mapSize; x++)
					{
						for (int y = 0; y < 7; y++)
						{
							if (RenderTiles[x][y].GetState() == levers->GetState())
							{
								RenderTiles[x][y].SetState(100);
								RenderTiles[x][y].SetY(RenderTiles[x][y].GetY() - 1500);
								platformCD = 0;
							}
						}
					}
					levers->SetState(levers->GetState() + 1);
				}
			}
		}
		/*if (sym == SDLK_F2) {
			if (playerBool) {
				CharacterDeath(skeleton,vampire);
			}
			else {
				CharacterDeath(vampire,skeleton);
			}
		}*/

		if (sym == SDLK_TAB && !runninBool && !jumpBool && !transformBool && transformCD < 0 && !leverBool)
		{
			if (currentTutorial == 2)
			{
				currentTutorial++;
			}

			pointerBool = true;
			//Changing character
			changeCharacter();

			//Its just because when you change characters they stuck in animation, so its prevent it
			if (playerBool)
			{
				pointer.SetPosition(vampire.GetX(), vampire.GetY() + size);
				if (vampireAlign)
				{
					rightBool = true;
					leftBool = false;
				}
				else
				{
					leftBool = true;
					rightBool = false;
				}

				if (rightBool)
				{
					vampire.SetAnimation("IdleRight");
					leftBool = false;
				}
				else if (leftBool)
				{
					vampire.SetAnimation("IdleLeft");
					rightBool = false;
				}
			}
			else
			{
				pointer.SetPosition(skeleton.GetX(), skeleton.GetY() + size);
				if (skeletonAlign)
				{
					rightBool = true;
					leftBool = false;
				}
				else
				{
					leftBool = true;
					rightBool = false;
				}

				if (rightBool)
				{
					skeleton.SetAnimation("IdleRight");
					leftBool = false;
				}
				else if (leftBool)
				{
					skeleton.SetAnimation("IdleLeft");
					rightBool = false;
				}
			}
		}

		if (sym == SDLK_e && !runninBool && !jumpBool && playerBool && transformCD < 0)
		{
			if (currentTutorial == 4)
			{
				currentTutorial++;
			}
			if (currentTutorial == 5 && camera.GetX() > size * 41)
			{
				currentTutorial++;
			}

			if (transformBool)
			{
				transformBool = false;
				if (leftBool) vampire.SetAnimation("ReverseTransformLeft", 11);
				else if (rightBool) vampire.SetAnimation("ReverseTransformRight", 11);
			}
			else
			{
				transformBool = true;
				if (leftBool) vampire.SetAnimation("TransformLeft", 11);
				else if (rightBool) vampire.SetAnimation("TransformRight", 11);
			}
			transformCD = 1;
			transformAnimationBool = true;
		}
	}
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (!levelBool && DeathAnimationState == 0)
	{
		if (transformCD <= 0 && switchCD <= 0)
		{
			if (sym == SDLK_a)
			{
				if (IsKeyDown(SDLK_d))
				{
					rightBool = true;
					leftBool = false;
					runninBool = true;
				}
				else
				{
					sfx01.Stop();
					pushBool = false;
					idleBool = true;
					runninBool = false;
					AnimationRefresh();
				}

				pushAnimBool = false;
			}

			if (sym == SDLK_d)
			{
				if (IsKeyDown(SDLK_a))
				{
					leftBool = true;
					rightBool = false;
					runninBool = true;
					pushBool = false;
				}
				else
				{
					sfx01.Stop();
					pushBool = false;
					idleBool = true;
					runninBool = false;
				}

				if (!pushBool)
					AnimationRefresh();
				pushAnimBool = false;
			}
		}
		if (InEditor && sym == SDLK_F1) {
			SaveMap();
		}
	}
}

void CMyGame::VampireAnimation()
{
	if (leftBool == true)
	{
		vampireAlign = false;
		if (runninBool)
		{
			vampire.SetAnimation("RunLeft");
			sfx01.Play("footstep.wav", -1);
			sfx01.Volume(0.03);
		}

		if (jumpBool)
		{
			vampire.SetAnimation("JumpUpLeft");
			sfx01.Stop();
		}
		if (!jumpHeightBool)
		{
			vampire.SetAnimation("JumpDownLeft");
		}

		if (idleBool && !jumpBool && !runninBool && !transformBool && DeathAnimationState == 0)
		{
			vampire.SetAnimation("IdleLeft");
		}

		if (transformBool)
		{
			vampire.SetAnimation("BatLeft", 8);
			sfx01.Play("wing.wav", -1);
		}
	}
	if (rightBool == true)
	{
		vampireAlign = true;
		if (runninBool)
		{
			vampire.SetAnimation("RunRight");
			sfx01.Play("footstep.wav", -1);
			sfx01.Volume(0.03);
		}

		if (jumpBool)
		{
			vampire.SetAnimation("JumpUpRight");
			sfx01.Stop();
		}

		if (!jumpHeightBool)
		{
			vampire.SetAnimation("JumpDownRight");
		}

		if (idleBool && !jumpBool && !runninBool && !transformBool && DeathAnimationState == 0)
		{
			vampire.SetAnimation("IdleRight");
		}

		if (transformBool)
		{
			vampire.SetAnimation("BatRight", 8);
			sfx01.Play("wing.wav", -1);
		}
	}
	//vampire.Update(GetTime());
}

void CMyGame::SkeletonAnimation()
{
	if (leftBool == true)
	{
		skeletonAlign = false;
		if (runninBool && !pushBool)
		{
			skeleton.SetAnimation("RunLeft");
			sfx01.Play("footstep.wav", -1);
			sfx01.Volume(0.03);
		}

		if (jumpBool)
		{
			skeleton.SetAnimation("JumpUpLeft");
			sfx01.Stop();
		}

		if (!jumpHeightBool)
		{
			skeleton.SetAnimation("JumpDownLeft");
		}

		if (idleBool && !jumpBool && !runninBool && !pushBool && DeathAnimationState == 0)
		{
			skeleton.SetAnimation("IdleLeft");
		}

		if (pushBool && !jumpBool)
		{
			skeleton.SetAnimation("pushLeft", 5);
		}

	}
	if (rightBool == true)
	{
		skeletonAlign = true;
		if (runninBool && !pushBool)
		{
			skeleton.SetAnimation("RunRight");
			sfx01.Play("footstep.wav", -1);
			sfx01.Volume(0.03);
		}

		if (jumpBool)
		{
			skeleton.SetAnimation("JumpUpRight");
			sfx01.Stop();
		}

		if (!jumpHeightBool)
		{
			skeleton.SetAnimation("JumpDownRight");
		}

		if (idleBool && !jumpBool && !runninBool && !pushBool && DeathAnimationState==0)
		{
			skeleton.SetAnimation("IdleRight");
		}

		if (pushBool && !jumpBool)
		{
			skeleton.SetAnimation("pushRight", 5);
		}
	}
	//skeleton.Update(GetTime());
}

void CMyGame::AnimationRefresh()
{
	pointerBool = false;
	if(switchCD <= 0)
	if (playerBool) VampireAnimation();
	else SkeletonAnimation();
}

void CMyGame::CharacterMovement()
{
	CVector vector = CVector(0, 0);
	float speed = GetHeight() / 1.8;

	if (playerBool && DeathAnimationState == 0)
	{
		if (IsKeyDown(SDLK_a) && vampire.GetX() > camera.GetX() - GetWidth() / 2)
		{
			vector.SetX(-1);
		}
		if (IsKeyDown(SDLK_d) && vampire.GetX() < camera.GetX() + GetWidth() / 2)
		{
			vector.SetX(1);
		}
	}
	else if (!playerBool && DeathAnimationState == 0)
	{
		if (IsKeyDown(SDLK_a) && skeleton.GetX() > camera.GetX() - GetWidth() / 2)
		{
			vector.SetX(-1);
		}
		if (IsKeyDown(SDLK_d) && skeleton.GetX() < camera.GetX() + GetWidth() / 2)
		{
			vector.SetX(1);
		}
	}

	if (playerBool)
	{
		vampire.SetXVelocity(vector.GetX() * speed);
	}
	else
	{
		if (pushBool && !jumpBool)skeleton.SetXVelocity(vector.GetX() * speed / 4);
		else skeleton.SetXVelocity(vector.GetX() * speed);
	}

	if (!IsKeyDown(SDLK_d) && !IsKeyDown(SDLK_a) && !idleBool)
	{
		idleBool = true;
		runninBool = false;
		runAnimationBool = false;
		pushBool = false;
		AnimationRefresh();
	}

	if (IsKeyDown(SDLK_SPACE) && !jumpBool && !transformBool &&! pushBool) // Jump
	{
		if (GameOver) {
			if (vampire.GetHealth() == 0) {
				vampire.SetAnimation("Revive");
			}
			else {
				skeleton.SetAnimation("Revive");
			}
			DeathAnimationState = 2;
			TimeAtReContinue = GetTime();
		}
		else {
			CharacterJump();
		}
		
	}

	if (playerBool)
	{
		if (vampire.GetYVelocity() == 0) 
		{
			jumpHeightBool = true;
		}

		if (vampire.GetYVelocity() < 0)
		{
			animationBool = false;
			jumpBool = true;
		}

		if (vampire.GetYVelocity() < 0 && jumpHeightBool)
		{
			jumpHeightBool = false;
			AnimationRefresh();
		}

		if (vampire.GetY() < 0)
		{
			CharacterDeath(skeleton, vampire);
			//vampire.SetPos(GetWidth() / 2, size + size / 3);
		}
	}
	else
	{
		if (skeleton.GetYVelocity() == 0)
		{
			jumpHeightBool = true;
		}

		if (skeleton.GetYVelocity() < 0)
		{
			animationBool = false;
			jumpBool = true;
		}

		if (skeleton.GetYVelocity() < 0 && jumpHeightBool)
		{
			jumpHeightBool = false;
			AnimationRefresh();
		}

		if (skeleton.GetY() < 0)
		{
			CharacterDeath(vampire, skeleton);
			skeleton.SetPosition((GetWidth() / 2) - 200, size + size / 3);
		}
	}
}

void CMyGame::CharacterJump() {
	jumpBool = true;
	jumpHeightBool = true;
	if (playerBool) vampire.SetYVelocity(size * 6);
	else skeleton.SetYVelocity(size * 6);
	AnimationRefresh();
	animationBool = false;
}

void CMyGame::CharacterJump(CSprite* Character) {
	jumpBool = true;
	jumpHeightBool = true;
	leftBool = true;
	Character->SetYVelocity(size * 6);
	AnimationRefresh();
	animationBool = false;
}

void CMyGame::changeCharacter()
{
	if (playerBool) playerBool = false;
	else playerBool = true;
}

void CMyGame::MapScroll(CGraphics* g, CSprite& map, CSprite& camera, float divide, float limiter, float mapStart, float mapEnd)
{
	static float distFromStart = 0;
	distFromStart = camera.GetX();

	if (distFromStart >= mapStart&& distFromStart <= mapEnd)
		g->SetScrollPos(-camera.GetX() / divide, 0);
	else
		g->SetScrollPos(-limiter / divide, 0);
	map.Draw(g);
}

void CMyGame::LevelOneLore(CGraphics* g)
{
	blackBG.Draw(g);

	static int levelNumber = 1;

	for (CSprite* lore : loreList)
	{

		lore->Update(GetTime());
		lore->SetX(blackBG.GetX());

		if (lore->GetState() == currentLevel)
		{
			lore->Draw(g);
		}
	}

	static float lerpPoint = 0;
	float endPoint = GetWidth() / 2 + GetWidth() - GetWidth() * 2.5;
	float middlePoint = GetWidth() / 2;

	if (levelBool)
	{
		lerpPoint = middlePoint;
	}


	if (IsKeyDown(SDLK_q) && !startBool)
	{
		startBool = true;
		lerpPoint = endPoint;
		levelBool = false;
		pointerBool = true;

		if (currentLevel == 2)
		{

			LoadMap("level01Tiles.bmp", "MapData - Copy (2).txt");
			bgNoise.Stop();

			mapStart = size * 9;
			mapEnd = size * 35;

			moon.SetImage("level01Layer4.bmp");
			moon.SetPosition(camera.GetX() / 20 + GetWidth() / 1.6, GetHeight() / 2);
			moon.SetSize(GetWidth(), GetHeight());

			forest.SetImage("level01Layer3.bmp");
			forest.SetPosition(camera.GetX() / 6 + GetWidth(), GetHeight() / 2);
			forest.SetSize(GetWidth() * 2, GetHeight());

			castle.SetImage("level01Layer2.bmp");
			castle.SetPosition(camera.GetX() / 3 + GetWidth(), GetHeight() / 2);
			castle.SetSize(GetWidth(), GetHeight());

			hills.SetImage("level01Layer1.bmp");
			hills.SetPosition(camera.GetX() / 2 + GetWidth() * 1.5, GetHeight() / 2);
			hills.SetSize(GetWidth() * 2, GetHeight());
		}
		else if (currentLevel == 3)
		{
			finalBool = true;
			bgNoise.Stop();
		}
	}

	blackBG.SetX(FloatLerp(lerpPoint, blackBG.GetX(), 0.96));

	static bool toggle = false;
	if (!toggle)
	{
		toggle = true;

		fstream File;
		ifstream Data("level01.txt");
		int x = 0;
		int y = 0;
		float nextLine = 1;
		string word = "";
		string sentence = "";
		if (Data.is_open()) {
			do {
				Data >> word;
				if (word == "-1") //If -1 is detected then it means theres a new line. I imagine theres an auto way of doing this
				{
					x = 0;

					CSprite* level;
					loreList.push_back(level = new CSpriteText(blackBG.GetX(), blackBG.GetY() + size * 2.5, "alagard.ttf", size * 1.2, sentence, CColor::White(), GetTime()));
					level->SetState(levelNumber);
					sentence = "";
					y++;
				}
				else if (word == "-2")
				{
					CSprite* level;
					loreList.push_back(level = new CSpriteText(blackBG.GetX(), blackBG.GetY() + size * 1.8, "alagard.ttf", size * 0.5, sentence, CColor::White(), GetTime()));
					level->SetState(levelNumber);
					sentence = "";
					y++;
				}
				else if (word == "-3")
				{
					CSprite* level;
					loreList.push_back(level = new CSpriteText(blackBG.GetX(), blackBG.GetY() + size * nextLine, "ChavaRegular.ttf", size * 0.3, sentence, CColor::White(), GetTime()));
					level->SetState(levelNumber);
					nextLine -= 0.4;
					sentence = "";
					y++;
				}
				else if (word == "-4")
				{
					levelNumber++;
					nextLine = 1;
				}
				else
				{
					sentence += word + " ";
					x++;
				}
			} while (Data.good());
			Data.close();
		}
	}
}

void CMyGame::LoadMap(char* TileMap, char* MapToLoad) {

	vampire.SetHealth(100);
	skeleton.SetHealth(100);
	leverList.delete_all();

	CurrentLevelPath = MapToLoad;

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			RenderTiles[x][y].Delete();
			RenderTiles[x][y] = CSprite();
		}
	}


	skeleton.SetPosition((GetWidth() / 2) - 200, size + size / 3);

	vampire.SetPos(GetWidth() / 2, size + size / 3);

	camera.SetPosition(vampire.GetPosition());

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			RenderTiles[x][y].LoadImageW("Air.bmp", "Air", CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile1", CSprite::Sheet(3, 6).Tile(0, 0), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile2", CSprite::Sheet(3, 6).Tile(0, 1), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile3", CSprite::Sheet(3, 6).Tile(0, 2), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile4", CSprite::Sheet(3, 6).Tile(1, 0), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile5", CSprite::Sheet(3, 6).Tile(1, 1), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile6", CSprite::Sheet(3, 6).Tile(1, 2), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile7", CSprite::Sheet(3, 6).Tile(2, 0), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile8", CSprite::Sheet(3, 6).Tile(2, 1), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile9", CSprite::Sheet(3, 6).Tile(2, 2), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile10", CSprite::Sheet(3, 6).Tile(0, 5), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile11", CSprite::Sheet(3, 6).Tile(1, 5), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile12", CSprite::Sheet(3, 6).Tile(2, 5), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile13", CSprite::Sheet(3, 6).Tile(2, 4), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile14", CSprite::Sheet(3, 6).Tile(0, 3), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile15", CSprite::Sheet(3, 6).Tile(0, 4), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile16", CSprite::Sheet(3, 6).Tile(1, 3), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile17", CSprite::Sheet(3, 6).Tile(1, 4), CColor::White());
			RenderTiles[x][y].LoadImageW(TileMap, "Tile18", CSprite::Sheet(3, 6).Tile(2, 3), CColor::White());
		}
	}

	tileSet[0] = "Air";
	tileSet[1] = "Tile1";
	tileSet[2] = "Tile2";
	tileSet[3] = "Tile3";
	tileSet[4] = "Tile4";
	tileSet[5] = "Tile5";
	tileSet[6] = "Tile6";
	tileSet[7] = "Tile7";
	tileSet[8] = "Tile8";
	tileSet[9] = "Tile9";
	tileSet[10] = "Tile10";
	tileSet[11] = "Tile11";
	tileSet[12] = "Tile12";
	tileSet[13] = "Tile13";
	tileSet[14] = "Tile14";
	tileSet[15] = "Tile15";
	tileSet[16] = "Tile16";
	tileSet[17] = "Tile17";
	tileSet[18] = "Tile18";

	fstream File;
	ifstream Data(MapToLoad);
	int x = 0;
	int y = 0;
	int reverseY = 6;
	int ID = 0;
	if (Data.is_open()) {
		do {
			Data >> ID;
			if (ID == -1) {//If -1 is detected then it means theres a new line. I imagine theres an auto way of doing this
				y++;
				reverseY--;
				x = 0;
			}
			else {
				int tempID = 0;
				if (ID == 0) {
					RenderTiles[x][reverseY].SetImage("Air");
					RenderTiles[x][reverseY].SetPosition(x * size + size / 2, reverseY * size + size / 4);
					RenderTiles[x][reverseY].SetState(-999);
				}
				else if (ID < 20)
				{
					if (ID == 19)
					{
						RenderTiles[x][reverseY].LoadImageW("box.bmp", CColor::White());
						RenderTiles[x][reverseY].SetImage("box.bmp");
						RenderTiles[x][reverseY].SetPosition(x * size + size / 2, reverseY * size + size / 4);
						RenderTiles[x][reverseY].SetState(5);
					}
					else if (ID >= 10 && ID <= 13)
					{
						RenderTiles[x][reverseY].SetImage(tileSet[ID]);
						RenderTiles[x][reverseY].SetSize(size, size);
						RenderTiles[x][reverseY].SetPos(x * size + size / 2, reverseY * size + size / 2);
						RenderTiles[x][reverseY].SetState(3);
					}
					else if (ID == 18)
					{
						RenderTiles[x][reverseY].SetImage(tileSet[ID]);
						RenderTiles[x][reverseY].SetSize(size, size);
						RenderTiles[x][reverseY].SetPos(x * size + size / 2, reverseY * size + size / 2);
						RenderTiles[x][reverseY].SetState(18);
					}
					else
					{
						RenderTiles[x][reverseY].SetImage(tileSet[ID]);
						RenderTiles[x][reverseY].SetSize(size, size);
						RenderTiles[x][reverseY].SetPos(x * size + size / 2, reverseY * size + size / 2);
						if (reverseY == 0)
							RenderTiles[x][reverseY].SetState(1);
					}
				}
				else if (ID >= 20)
				{
						tempID = ID / 20;
						tempID *= 20;
					if (ID % 20 == 0)
					{
						CSprite* lever;
						leverList.push_back(lever = new CSprite);
						lever->AddImage("leverAnimation.bmp", "Lever", 9, 1, 0, 0, 9, 0, CColor::White());
						lever->AddImage("leverEnd.bmp", "End", CColor::White());
						lever->AddImage("leverStart.bmp", "Start", CColor::White());
						lever->SetAnimation("Start");
						lever->SetPosition(x * size + size / 2, reverseY * size + size / 3.5);
						lever->SetState(tempID);
						cout << "yo" << endl;
					}
					else
					{
						RenderTiles[x][reverseY].SetImage(tileSet[ID - tempID]);
						RenderTiles[x][reverseY].SetSize(size, size);
						RenderTiles[x][reverseY].SetPos(x * size + size / 2, reverseY * size + size / 2);
						if (reverseY == 0)
							RenderTiles[x][reverseY].SetState(1);
						RenderTiles[x][reverseY].SetState(tempID);
					}
				}
				RenderTiles[x][reverseY].SetProperty("ID", ID - tempID);
				x++;
			}

		} while (Data.good());
		Data.close();
	}
}

void CMyGame::SaveMap() {
	ofstream File;
	File.open("MapDataTest.txt");
	for (int y = 6; y >= 0; y--) {
		for (int x = 0; x < 128; x++) {
			int ID = -999;
			ID = RenderTiles[x][y].GetProperty("ID");
				cout << ID;
				File << ID;
				File << " ";
			
		}
		File << "-1 \n";
		cout << endl;
	}
	
}

void CMyGame::ParticleSystem()
{
	if (currentLevel == 2 || currentLevel == 3)
	{
		for (CSprite* particles : particleList)
		{
			particles->Update(GetTime());
			particles->SetSize(particles->GetH() - 0.1, particles->GetW() - 0.1);
			particles->SetDirection(-120);
			particles->SetSpeed(100);
			particles->SetOmega(50);

			if (particles->GetH() <= 0)
			{
				particles->Delete();
			}
		}

		if (rand() % 15 == 1)
		{
			CSprite* particle;
			particleList.push_back(particle = new CSprite);
			particle->LoadImageW("particle.bmp", CColor::Black());
			particle->SetImage("particle.bmp");
			particle->SetPosition(camera.GetX() - GetWidth() / 2 + rand() % GetWidth(), camera.GetY() - GetHeight() / 2 + rand() % GetHeight());
			particle->SetSize(6, 6);
			particle->SetRotation(rand() % 360);
		}
	}
	else if (currentLevel == 1 && !levelBool)
	{
		for (CSprite* particles : particleList)
		{
			particles->Update(GetTime());
			particles->SetDirection(-180);
			particles->SetSpeed(1500);

			if (particles->GetX() - 20 < 0)
			{
				particles->Delete();
			}

			for (int i = 0; i < mapSize; i++)
			{
				for (int j = 0; j < 7; j++)
				{
					if (RenderTiles[i][j].HitTest(particles) && RenderTiles[i][j].GetState() < 20)
					{
						particles->Delete();

						CSprite* rains;
						rainList.push_back(rains = new CSprite);
						rains->AddImage("particle2Splash.bmp", "rain", 4, 1, 0, 0, 4, 0, CColor::White());
						rains->SetAnimation("rain", 8);
						rains->SetPosition(particles->GetX(), particles->GetY());
						rains->SetSize(10, 10);
						rains->SetHealth(100);
					}


				}
			}
		}

		int size = 3 + rand() % 3;
		CSprite* particle;
		particleList.push_back(particle = new CSprite);
		particle->LoadImageW("particle2.bmp", CColor::Black());
		particle->SetImage("particle2.bmp");
		particle->SetPosition(camera.GetX() - GetWidth() / 2 + rand() % GetWidth() * 1.5, GetHeight() + 50);
		particle->SetSize(3, size * 6);

	}

	particleList.delete_if(deleted);
}

void CMyGame::LeverSystem(CSprite& _player)
{
	if (switchCD > 0)
	{
		_player.SetSize(size * 2.4, size * 1.2);
	}

	for (CSprite* levers : leverList)
	{
		levers->Update(GetTime());
		levers->SetSize(size * 2.4, size * 1.2);

		if (_player.HitTest(levers) && leverBool * 1.2)
		{
			if (switchCD <= 0)
			{
				levers->SetAnimation("End");
				if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_d)) runninBool = true;

				if (IsKeyDown(SDLK_a))
				{
					leftBool = true;
					rightBool = false;
				}
				else
				{
					rightBool = true;
					leftBool = false;
				}
				AnimationRefresh();
				leverBool = false;
			}
		}
	}

	switchCD -= (float)GetDeltaTime() / 1000;
}

void CMyGame::TransformationSystem()
{
	transformCD -= (float)GetDeltaTime() / 1000;

	if (transformCD <= 0 && transformAnimationBool)
	{
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_d)) runninBool = true;

		if (IsKeyDown(SDLK_a))
		{
			leftBool = true;
			rightBool = false;
		}
		else if (IsKeyDown(SDLK_d))
		{
			leftBool = false;
			rightBool = true;
		}
		AnimationRefresh();
		transformAnimationBool = false;
	}
}

void CMyGame::JumpOnSkeleton()
{
	//Vampire jump on Skeleton
	CVector n = CVector(0, 1);
	CVector vectorN = Normalise(vampire.GetVelocity());

	if ((skeleton.HitTest(vampire.GetX(), vampire.GetY() - vampire.GetHeight() / 2.5) && vampire.GetY() > skeleton.GetY() + skeleton.GetHeight() / 2 &&
		vampire.GetX() > skeleton.GetX() - skeleton.GetW() / 7 && vampire.GetX() < skeleton.GetX() + skeleton.GetW() / 7 && Dot(vectorN, n) < 0))
	{
		vampire.SetYVelocity(0);
		vampire.SetY(skeleton.GetY() + (skeleton.GetHeight() / 2) + vampire.GetHeight() / 3.5);
		if (playerBool)
		{
			jumpBool = false;
			topBool = false;

			if (currentTutorial == 3)
			{
				currentTutorial++;
			}

			if (skeletonAlign)
			{
				skeleton.SetImage("SkeletonTopRight.bmp");
				skeleton.SetSize(size * 1.2, size * 1.2);
			}
			else
			{
				skeleton.SetImage("SkeletonTopLeft.bmp");
				skeleton.SetSize(size * 1.2, size * 1.2);
			}

			if (!animationBool)
			{
				animationBool = true;
				jumpHeightBool = true;
				AnimationRefresh();
			}
		}
	}
	if (!topBool && jumpBool)
	{
		if (skeletonAlign)
		{
			skeleton.SetAnimation("IdleRight");
			skeleton.SetSize(size, size);
		}
		else
		{
			skeleton.SetAnimation("IdleLeft");
			skeleton.SetSize(size, size);
		}
		topBool = true;
	}
}

void CMyGame::PlatformCollision(CSprite& player, CSprite& platform)
{
	float distance = Distance(player.GetPos(), platform.GetPos());
	if (platform.GetState() != -999) {
		if (player.HitTest(&platform, 0) && distance < size / 1.3 && player.GetX() < platform.GetX() &&
			IsKeyDown(SDLK_d) && player.GetY() < platform.GetY() + platform.GetH() / 2 &&
			platform.GetState() != 5 && platform.GetState() != 3 && platform.GetState() < 20)
		{
			player.SetX(platform.GetX() - platform.GetW() / 2 - player.GetW() / 5);
		}

		if (player.HitTest(&platform, 0) && distance < size / 1.3 && player.GetX() > platform.GetX() &&
			IsKeyDown(SDLK_a) && player.GetY() < platform.GetY() + platform.GetH() / 2 &&
			platform.GetState() != 5 && platform.GetState() != 3 && platform.GetState() < 20)
		{
			player.SetX(platform.GetX() + platform.GetW() / 2 + player.GetW() / 5);
		}
	}
	
}

void CMyGame::PlatformTopCollision(CSprite& platform)
{
	//walk on top
	if (platform.GetState() != -999) {
		CVector n = CVector(0, 1);
		CVector vectorN = Normalise(vampire.GetVelocity());

		if ((platform.HitTest(vampire.GetX(), vampire.GetY() - vampire.GetHeight() / 2.5) &&
			vampire.GetY() > platform.GetY() + platform.GetHeight() / 2 &&
			Dot(vectorN, n) < 0) && platform.GetState() < 20 ||
			(platform.HitTest(vampire.GetX(), vampire.GetY() - vampire.GetHeight() / 2.5) &&
				platform.GetState() == 1) && Dot(vectorN, n) < 0)
		{
			int ID = platform.GetProperty("ID");
			if (ID == 18) {
				CharacterDeath(skeleton, vampire);
				return;
			}
			vampire.SetYVelocity(0);
			vampire.SetY(platform.GetY() + (platform.GetHeight() / 2) + vampire.GetHeight() / 3.5);
			vampire.SetXVelocity(platform.GetXVelocity());

			if (playerBool)
			{
				jumpBool = false;

				if (!animationBool)
				{
					animationBool = true;
					jumpHeightBool = true;
					if (DeathAnimationState==0 || DeathAnimationState > 4) AnimationRefresh();
				}
			}
		}

		vectorN = Normalise(skeleton.GetVelocity());
		if ((platform.HitTest(skeleton.GetX(), skeleton.GetY() - skeleton.GetHeight() / 2.5) &&
			skeleton.GetY() > platform.GetY() + platform.GetHeight() / 2 &&
			Dot(vectorN, n) < 0) && platform.GetState() < 20 ||
			(platform.HitTest(skeleton.GetX(), skeleton.GetY() - skeleton.GetHeight() / 2.5) &&
				platform.GetState() == 1) && Dot(vectorN, n) < 0)
		{
			int ID = platform.GetProperty("ID");
			if (ID == 18) {
				CharacterDeath(vampire, skeleton);
				return;
			}
			skeleton.SetYVelocity(0);
			skeleton.SetY(platform.GetY() + (platform.GetHeight() / 2) + skeleton.GetHeight() / 3.5);

			if (!playerBool)
			{
				jumpBool = false;

				if (!animationBool)
				{
					animationBool = true;
					jumpHeightBool = true;
					if (DeathAnimationState == 0 || DeathAnimationState > 4)AnimationRefresh();
				}
			}
		}
	}
	
}

void CMyGame::PushBox(CSprite& player, CSprite& platform)
{
	if (platform.GetState() == 5) platform.SetSize(size, size);

	if (player.HitTest(&platform) && platform.GetState() == 5 && !jumpBool &&
		player.GetY() - player.GetH() / 2 < platform.GetY())
	{
		if (player.GetX() > platform.GetX() && IsKeyDown(SDLK_a) && !IsKeyDown(SDLK_d) ||
			player.GetX() < platform.GetX() && IsKeyDown(SDLK_d))
		{
			platform.SetXVelocity(player.GetXVelocity());

			if (currentTutorial == 7)
			{
				currentTutorial++;
			}

			if (!pushAnimBool)
			{
				pushBool = true;
				pushAnimBool = true;
				AnimationRefresh();
			}
		}
	}
	else
	{
		platform.SetXVelocity(0);
	}

	for (int i = 0; i < mapSize; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (RenderTiles[i][j].GetState() == 5 && platform.HitTest(&RenderTiles[i][j]) &&
				RenderTiles[i][j].GetY() < platform.GetY() + platform.GetH() / 2 &&
				RenderTiles[i][j].GetY() > platform.GetY() - platform.GetH() / 2 &&
				platform.GetState() != 5 && platform.GetState() < 20)

			{
				RenderTiles[i][j].SetXVelocity(0);
				RenderTiles[i][j].SetState(6);
			}
		}
	}
	platform.Update(GetTime());
}

void CMyGame::Pointer(CSprite& player)
{
	pointer.Update(GetTime());
	pointer.SetSize(size / 4, size / 4);

	static bool pointerMove = true;

	pointer.SetX(player.GetX());

	if (pointer.GetY() < player.GetY() + size - 5 && !pointerMove)
	{
		pointer.SetDirection(0);
		pointerMove = true;
	}
	if (pointer.GetY() > player.GetY() + size + 5 && pointerMove)
	{
		pointer.SetDirection(180);
		pointerMove = false;
	}
	pointer.SetSpeed(30);
}

void CMyGame::LeverPlatform(CSprite& platform)
{
	if (platform.GetState() == 100 && platformCD < 1500)
	{
		platform.SetY(platform.GetY() + 10);
	}
	else if (platformCD >= 1500 && platform.GetState() == 100)
	{
		platform.SetState(3);
	}
}

CVector CMyGame::VectorLerp(CVector targetA, CVector targetB, float time)
{
	return targetA * (1 - time) + targetB * time;
}

float CMyGame::FloatLerp(float floatX, float floatY, float time)
{
	if (time <= 1)
	{
		return floatX * (1 - time) + floatY * time;
	}
	else
		return floatY;
}

void CMyGame::CharacterDeath(CSprite& CharacterToLive, CSprite& CharacterToDie) {
	GameOver = true;
	DeathAnimationState = 1;
	CharacterToDie.SetHealth(-1);
	CharacterToDie.SetAnimation("ReviveStatic");
	CharacterToDie.SetPos((1280 / 3), 130);
	CharacterToLive.SetAnimation("IdleLeft");
	CharacterToLive.SetPos((1280 / 3) * 2, 130);
}

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
	if (InEditor) {
		EditorX = (x + camera.GetX()) / 128;
		EditorY = (y / 128);
		cout << EditorX << " " << EditorY << endl;
		MapEditor.SetPos(((x + camera.GetX() - 640) / 128), (y / 128) * 128);
	}
	
}

void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	if (InEditor) {
		EditorX = (x + camera.GetX()) / 128;
		EditorY = (y / 128);
		cout << EditorX << " " << EditorY << endl;
		RenderTiles[EditorX][EditorY].SetImage(tileSet[EditorID]);
		RenderTiles[EditorX][EditorY].SetProperty("ID", EditorID);
		RenderTiles[EditorX][EditorY].SetSize(size, size);
	}
	
}