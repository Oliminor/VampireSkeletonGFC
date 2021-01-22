#include "stdafx.h"
#include "MyGame.h"
#include "iostream"
#include <fstream>
#include <string>	

using namespace std;

CMyGame::CMyGame(void)
{
}


CMyGame::~CMyGame(void)
{
}

void CMyGame::LoadMap() {
	fstream File;
	ifstream Data("MapData.txt");
	int x = 0;
	int y = 0;
	int ID = 0;
	if (Data.is_open()) {
		do {
			Data >> ID;
			if (ID == -1) {
				y++;
				x = 0;
			}else {
				WorldTiles[x][y] = ID;
				x++;
			}
			
		} while (Data.good());
		Data.close();
	}
}

void CMyGame::PlaceObject(char* Image, int X, int Y, int Collision) {
}

void CMyGame::EnemyAI(CSprite* Enemy, int t) {
}

void CMyGame::OnUpdate()
{

	if (IsMenuMode() || IsGameOver()) return;

	long t = GetTime();

	float NewLevel = false;

	Player.Update(t);

	CameraDX = Player.GetX() / 64;
	CameraDY = Player.GetY() / 64;

	if (NewLevel == true) {
		LoadLevel(PlayerLevel);
	}

}

void CMyGame::PlayerControl()
{

}

void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		MainMenuScreen.Draw(g);
		return;
	}

	//g->SetScrollPos(-CameraDX, -CameraDY);
	int tx = 0;
	int ty = 0;
	int tempx = -Player.GetX();
	int tempy = -Player.GetY();
	g->SetScrollPos(tempx % 64, tempy % 64);
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			tx = CameraDX + x;
			ty = CameraDY + y;
			if (tx > -1 && tx < 64) {
				if (ty > -1 && ty < 64) {
					RenderTiles[x][y].SetImage(TileSet[WorldTiles[tx][ty]]);
					RenderTiles[x][y].Draw(g);
				}
			}
		}
	}

	g->SetScrollPos(-Player.GetX()+400, -Player.GetY()+400);

	Player.Draw(g);

	g->SetScrollPos(0, 0);

	*g << bottom << left << "Score: " << PlayerScore << endl;
}

void CMyGame::OnInitialize()
{
	Camera.LoadImage("Player.png", CColor::Black());
	Camera.SetImage("Player.png");
	Camera.SetPos(0, 0);

	Player.LoadImage("PlayerSpriteSheet.png");
	Player.SetImage("PlayerSpriteSheet.png");
	Player.AddImage("PlayerSpriteSheet.png", "WalkDown", 4, 4, 0, 3, 5, 3, CColor::Black());
	Player.AddImage("PlayerSpriteSheet.png", "WalkUp", 4, 4, 0, 2, 5, 2, CColor::Black());
	Player.AddImage("PlayerSpriteSheet.png", "WalkRight", 4, 4, 0, 1, 5, 1, CColor::Black());
	Player.AddImage("PlayerSpriteSheet.png", "WalkLeft", 4, 4, 0, 0, 5, 0, CColor::Black());
	Player.SetAnimation("WalkUp", 1);
	Player.SetPos(0, 0);

	LoadLevel(PlayerLevel);

	MainMenuScreen.LoadImage("StartScreen.png", CColor::Black());
	MainMenuScreen.SetImage("StartScreen.png");
	MainMenuScreen.SetPosition(400, 300);

}

void CMyGame::LoadLevel(int LevelNumber) {
	WorldObjects.delete_all();
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			RenderTiles[x][y].LoadImage("Crate.png", "Crate");
			RenderTiles[x][y].LoadImage("EnemyPad.png", "EnemyPad");
			RenderTiles[x][y].SetImage("Crate.png");
			RenderTiles[x][y].SetPos(x * 64, y * 64);
		}
	}
	if (LevelNumber == 1) {
		TileSet[0] = "Crate";
		TileSet[1] = "EnemyPad";
		LoadMap();

	}

	else if (LevelNumber == 2) {
	}
	else {
		GameOver();
	}
}

void CMyGame::OnDisplayMenu()
{
	
}

void CMyGame::OnStartGame()
{
}

void CMyGame::OnGameOver()
{

}

void CMyGame::OnTerminate()
{

}

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{

	if (IsMenuMode())
	{
		if (sym == SDLK_SPACE) {
			StartGame();
		}
	}
	else {
		if (sym == SDLK_a) {
			Player.SetAnimation("WalkLeft", 10);
			Player.SetXVelocity(-PlayerSpeed);
		}
		if (sym == SDLK_d) {
			Player.SetAnimation("WalkRight", 10);
			Player.SetXVelocity(+PlayerSpeed);
		}
		if (sym == SDLK_w) {
			Player.SetAnimation("WalkUp", 10);
			Player.SetYVelocity(+PlayerSpeed);
		}
		if (sym == SDLK_s) {
			Player.SetAnimation("WalkDown", 10);
			Player.SetYVelocity(-PlayerSpeed);
		}
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (IsMenuMode())
	{
		StartGame();
	}
	else {
		if (sym == SDLK_a) {
			Player.SetXVelocity(0);
		}
		if (sym == SDLK_d) {
			Player.SetXVelocity(0);
		}
		if (sym == SDLK_w) {
			Player.SetYVelocity(0);
		}
		if (sym == SDLK_s) {
			Player.SetYVelocity(0);
		}
	}
}


void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	//Player.SetDirection(x - Camera.GetX(),y - Camera.GetY());
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
