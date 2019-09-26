#include "Game.h"

int main(int arg, char **argc)
{
	CGame game(640, 480, 100, 100);
	
	float fBgColor[4] = {0.0f, 0.0f, 0.0f};
	double dLookAt[3][3] = {{0, 0, 5}, {0, 0, 0}, {0, 1, 0}};

	game.Create(arg, argc, fBgColor, dLookAt, false);

	return 0;
}