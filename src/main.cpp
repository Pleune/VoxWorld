#include <iostream>

#include "logger.hpp"
#include "gameengine.hpp"
#include "state_window.hpp"
#include "textbox.hpp"

int
main()
{
	Logger::stdout.set_print_level(Logger::DEBUG);
	Logger::stdout.log(Logger::DEBUG) << "Creating GameEngine..." << Logger::MessageStream::endl;

	GameEngine engine(StateWindow::instance());

	engine.loop();

    return 0;
}
