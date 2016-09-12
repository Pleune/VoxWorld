#include <iostream>

#include "logger.hpp"
#include "gameengine.hpp"
#include "state_initalize.hpp"

int
main()
{
	Logger::stdout.set_print_level(Logger::DEBUG);
	Logger::stdout.log(Logger::DEBUG) << "Creating GameEngine..." << Logger::MessageStream::endl;

	GameEngine engine(StateInitalize::instance());

	engine.loop();

    return 0;
}
