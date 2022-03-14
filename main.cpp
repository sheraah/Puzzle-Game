#include "Puzzle.h"

int main()
{
	srand(unsigned(time(0)));

	//Puzzle Object
	Puzzle puzzle(4, 4, "Textures/Test1.png", 1500, 1000);

	puzzle.run();
}