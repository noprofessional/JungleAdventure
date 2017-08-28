#include "FatalError.h"
namespace Lengine {
//ERROR check function
void fatalError(const std::string& errorstring) {
	std::cout << errorstring << std::endl;
	std::cout << "Press any key to quit!";
	char a;
	std::cin.get(a);
	SDL_Quit();
	exit(-1);
}
}