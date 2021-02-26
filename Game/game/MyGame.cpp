#include "stdafx.h"
#include "MyGame.h"
#include "iostream"
#include <fstream>

using namespace std;

CMyGame::CMyGame(void)
{
}


CMyGame::~CMyGame(void)
{
}

void CMyGame::LoadMap(char* TileMap) {
	char* Arr[6];
	Arr[0] = "level00Background.bmp";
	Arr[1] = "level00Foreground.bmp";
	Arr[2] = "level00Layer1.bmp";
	Arr[3] = "level01Layer2.bmp";
	Arr[4] = "level01Layer3.bmp";
	Arr[5] = "level01Layer4.bmp";
	for (int i = 0; i < 3; i++) {
		CSprite* Background = new CSprite(0, 0, 0, 0, GetTime());
		Background->LoadImage(Arr[i], "Background", CColor::White());
		Background->SetImage("Background");
		int x = Background->GetWidth();
		int y = Background->GetHeight();
		Background->SetProperty("ScrollingSpeed", 100);
		Background->SetSize(x*2,y*2);
		Background->SetPos(640,640);
		//Background->SetBottom(0);
		Backgrounds.push_back(Background);
	}
	


	for (int x = 0; x < 14; x++) {//Gives the render tiles there images. This is all hard coded and therefor poor programming by Joe -Joe
		for (int y = 0; y < 14; y++) {
			RenderTiles[x][y].LoadImage(TileMap, CColor::White());
			int Index = 1;
			for (int TileImageX = 0; TileImageX < 3; TileImageX++) {
				for (int TileImageY = 0; TileImageY < 6; TileImageY++) {
					RenderTiles[x][y].LoadImage(TileMap, TileSet[Index], CSprite::Sheet(3, 6).Tile(TileImageX, TileImageY), CColor::White());
					Index++;
				}
			}
			
			RenderTiles[x][y].SetPos(x * 64, y * 64);
		}
	}
	//Reads map data
	fstream File;
	ifstream Data("MapData.txt");
	int x = 0;
	int y = 0;
	int ID = 0;
	if (Data.is_open()) {
		do {
			Data >> ID;
			if (ID == -1) {//If -1 is detected then it means theres a new line. I imagine theres an auto way of doing this
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

void CMyGame::SaveMap() {
	ofstream Data("MapData2.txt");
	for (int x = 0; x < MapSizeX; x++) {
		for (int y = 0; y < MapSizeY; y++) {
			
		}
	}
	Data.close();
}

void CMyGame::OnUpdate()
{

	if (IsMenuMode() || IsGameOver()) return;

	long t = GetTime();

	Player.Update(t);

	Player.SetProperty("TileX",(Player.GetX() / 64));//calcs and sets the players X tile
	Player.SetProperty("TileY", (Player.GetY() / 64));//calcs and sets the players Y tile


}

void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())//This is for drawing the non-exsistant menu
	{
		MainMenuScreen.Draw(g);
		return;
	}

	//GFC can be a bit funny relating to returning values so this has to be split
	int OffSetX = -Player.GetX();
	int OffSetY = -Player.GetY();

	for (CSprite* Background : Backgrounds) {
		int ScrollSpeed = Background->GetProperty("ScrollingSpeed");
		ScrollSpeed /= 100;
		g->SetScrollPos(OffSetX * ScrollSpeed, OffSetY);
		Background->Draw(g);
	}

	if (SmoothScrolling == true) {
		g->SetScrollPos(OffSetX % 64, OffSetY % 64);//Since tiles are 32 pixels, we want to find the remainder of 32 for player position. This is what allows the "smooth look" of scrolling
	}

	int PlayerTileX = Player.GetProperty("TileX");//TileX is the tile the player is on the X axis. 
	int PlayerTileY = Player.GetProperty("TileY");//TileY is the tile the player is on the Y axis. 
	int RenderX = 0;//This the the X value for the render tile.
	int RenderY = 0;//This the the Y value for the render tile.
	for (int x = PlayerTileX-6; x < PlayerTileX+8; x++){
		for (int y = PlayerTileY - 6; y < PlayerTileY + 8; y++) {
			if (0 <= x && x < 64) {
				if (0 <= y && y < 64) {
					char* TileID = TileSet[WorldTiles[x][y]];//Gets the image label from the TileSet List
					if (TileID != "Air") {//If the tile is air, we dont want to draw anything. be a waste of processing power.
						RenderTiles[RenderX][RenderY].SetImage(TileID);//Changes the tile being rendered into the right image.
						RenderTiles[RenderX][RenderY].SetSize(64, 64);
						RenderTiles[RenderX][RenderY].Draw(g);//You know what this does
					}
				}
			}
			RenderY++;//increases the render Y index by one
		}
		RenderX++;//increases the render X index by one
		RenderY = 0;//Sets the render Y to 0 cus its a grid layout lol
	}

	//g->SetScrollPos(-Player.GetX()+400, -Player.GetY()+400);//so we can render the player in the middle of the screen

	g->SetScrollPos(-Player.GetX(), -Player.GetY());
	Player.SetSize(128, 128);
	Player.Draw(g);//renders the player

	g->SetScrollPos(0, 0);//resets scroll

	//TEXT GO HERE----------------------

}

void CMyGame::OnInitialize()
{
	//Loads vampire here
	Player.LoadImage("VampireIdleLeft.bmp");
	Player.AddImage("VampireIdleLeft.bmp", "IdleLeft", 5, 1, 0, 0, 5, 0, CColor::White());
	Player.AddImage("VampireRunLeft.bmp", "RunLeft", 10, 1, 0, 0, 10, 0, CColor::White());
	Player.AddImage("VampireIdleRight.bmp", "IdleRight", 5, 1, 0, 0, 5, 0, CColor::White());
	Player.AddImage("VampireRunRight.bmp", "RunRight", 10, 1, 0, 0, 10, 0, CColor::White());
	Player.SetAnimation("IdleLeft");
	Player.SetPos(0, 0);

	MainMenuScreen.LoadImage("StartScreen.png", CColor::Black());
	MainMenuScreen.SetImage("StartScreen.png");
	MainMenuScreen.SetPosition(400, 300);

	LoadLevel(1);

}

void CMyGame::LoadLevel(int LevelNumber) {
	
	if (LevelNumber == 1) {//This is the place holder level
		TileSet[1] = "PlatformEdgeL";//Creates the tile set and sets it all to "air"
		TileSet[2] = "PlatformMid";//Creates the tile set and sets it all to "air"
		TileSet[3] = "PlatformEdgeR";//Creates the tile set and sets it all to "air"

		TileSet[4] = "CornerBR";//Creates the tile set and sets it all to "air"
		TileSet[5] = "CornerBL";//Creates the tile set and sets it all to "air"
		TileSet[6] = "PlatfromSingle";//Creates the tile set and sets it all to "air"
		TileSet[7] = "CornerTR";//Creates the tile set and sets it all to "air"
		TileSet[8] = "CornerTL";//Creates the tile set and sets it all to "air"
		TileSet[9] = "Blank";//Creates the tile set and sets it all to "air"

		TileSet[10] = "EdgeTL";//Creates the tile set and sets it all to "air"
		TileSet[11] = "EdgeT";//Creates the tile set and sets it all to "air"
		TileSet[12] = "EdgeTR";//Creates the tile set and sets it all to "air"
		TileSet[13] = "EdgeL";//Creates the tile set and sets it all to "air"
		TileSet[14] = "Middle";//Creates the tile set and sets it all to "air"
		TileSet[15] = "EdgeR";//Creates the tile set and sets it all to "air"
		TileSet[16] = "EdgeBL";//Creates the tile set and sets it all to "air"
		TileSet[17] = "EdgeB";//Creates the tile set and sets it all to "air"
		TileSet[18] = "EdgeBR";//Creates the tile set and sets it all to "air"

		LoadMap("level01Tiles.bmp");//Thats the single texture. this should be changed cus its poor Joe, very poor. -Joe
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
	else {//Feel free to remove the movement I did. It was just temp-Joe
		if (sym == SDLK_a) {
			Player.SetAnimation("RunLeft");
			Player.SetXVelocity(-480);
		}
		if (sym == SDLK_d) {
			Player.SetAnimation("RunRight");
			Player.SetXVelocity(+480);
		}
		if (sym == SDLK_w) {
			Player.SetYVelocity(+480);
		}
		if (sym == SDLK_s) {
			Player.SetYVelocity(-480);
		}
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (IsMenuMode())
	{
		StartGame();
	}
	else {//Feel free to remove the movement I did. It was just temp-Joe
		if (sym == SDLK_a) {
			Player.SetAnimation("IdleLeft");
			Player.SetXVelocity(0);
		}
		if (sym == SDLK_d) {
			Player.SetAnimation("IdleRight");
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
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	//Please note this is super temp and will 100% be removed-Joe
	int X = Player.GetProperty("TileX");
	int Y = Player.GetProperty("TileY");
	WorldTiles[X][Y]++;
	if (WorldTiles[X][Y] > 9) {
		WorldTiles[X][Y] = 0;
	}
	//Please note this is super temp and will 100% be removed-Joe
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
	SaveMap();
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
