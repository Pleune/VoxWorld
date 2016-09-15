#include <iostream>

#include <cstdlib>
#include <ctime>

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

    Logger::stdout.log(Logger::INFO) << "Counting nodes before doing anything... " << tree.count_nodes() << " nodes counted." << Logger::MessageStream::endl;

    for(int x=0; x<4; x++)
    for(int y=0; y<4; y++)
    for(int z=0; z<4; z++)
        tree.set(x,y,z,++i);

    Logger::stdout.log(Logger::INFO) << "Counting nodes after filling octree sequencally... " << tree.count_nodes() << " nodes counted." << Logger::MessageStream::endl;
    i=2;

    for(int x=0; x<4; x++)
    for(int y=0; y<4; y++)
    for(int z=0; z<4; z++)
        tree.set(x,y,z,i);

    Logger::stdout.log(Logger::INFO) << "Counting nodes after filling all with the same number... " << tree.count_nodes() << " nodes counted." << Logger::MessageStream::endl;


    std::srand((unsigned)time(0));

    for(int x=0; x<4; x++)
    for(int y=0; y<4; y++)
    for(int z=0; z<4; z++)
    {
        if(z%2 == 0)
            i = std::rand() % 2;
        tree.set(x,y,z,i);
    }

    Logger::stdout.log(Logger::INFO) << "Counting nodes after filling all with random-ish numbers 0-19... " << tree.count_nodes() << " nodes counted." << Logger::MessageStream::endl;

    return 0;
}
