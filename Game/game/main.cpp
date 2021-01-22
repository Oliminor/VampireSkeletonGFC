#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	app.OpenConsole();
	app.OpenWindow(800, 800, "Arcade Movement");
	//app.OpenFullScreen(800, 600, 24);
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}
