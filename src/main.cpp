#include <iostream>

#include "logger.hpp"
#include "gameengine.hpp"
#include "state_window.hpp"
#include "textbox.hpp"
#include "voxeltree.hpp"

int
main()
{
    Logger::stdout.set_print_level(Logger::DEBUG);

    //Logger::stdout.log(Logger::DEBUG) << "Creating GameEngine..." << Logger::MessageStream::endl;
    //GameEngine engine(StateWindow::instance());
    //engine.loop();

    int i = 0;
    VoxelTree<int, 2> tree(2, i);

    for(int x=0; x<4; x++)
    for(int y=0; y<4; y++)
    for(int z=0; z<4; z++)
        tree.set(x,y,z,++i);

    for(int x=0; x<4; x++)
    for(int y=0; y<4; y++)
    for(int z=0; z<4; z++)
        Logger::stdout.log(Logger::INFO) << tree.get(x,y,z) << Logger::MessageStream::endl;

    return 0;
}
