#include "GameEngine.h"
#include "StressTest.h"

int main(int argc, char** argv) {
	GameEngine g("assets.txt", StressConfig::fromArgs(argc, argv));
	g.run();

	return 0;
}
