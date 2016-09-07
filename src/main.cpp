#include <iostream>

#include "logger.hpp"
#include "gameengine.hpp"
#include "state_test.hpp"

int
main()
{
    Logger log;

	std::cout << "Cteating engine..." << std::endl;
	GameEngine engine(StateTest::instance());
	engine.loop();

    return 0;
}
