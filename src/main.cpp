#include <iostream>

#include <cstdlib>
#include <ctime>

#include "logger.hpp"
#include "gameengine.hpp"
#include "state_window.hpp"
#include "textbox.hpp"
#include "voxeltree.hpp"
#include <SDL.h>

int
main(int argc, char* args[])
{
    Logger::standard.set_print_level(Logger::LOG_DEBUG);

    Logger::standard.log(Logger::LOG_DEBUG) << "Creating GameEngine..." << Logger::MessageStream::endl;
    GameEngine engine(StateWindow::instance());
    engine.loop();

    return 0;
}
