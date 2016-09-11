#include <iostream>

#include "logger.hpp"
#include "gameengine.hpp"
#include "state_test.hpp"

int
main()
{
	Logger::stdout.set_print_level(Logger::DEBUG);
	Logger::stdout.log(Logger::DEBUG) << "Creating GameEngine..." << Logger::MessageStream::endl;

	GameEngine engine(StateTest::instance());

	engine.loop();

    return 0;
}
