#include "chunkgen.hpp"

void ChunkGeneratorRandom::generate(Chunk *target)
{
    target->lock(RWLock::WRITE);
    int c_width = Chunk::size();

    int num_places = c_width * c_width;
    for(int i=0; i<num_places; i++)
    {
        int x = rand()%c_width;
        int y = rand()%c_width;
        int z = rand()%c_width;
        target->set(x,y,z, rand()%NUM_BLOCK_TYPES);
    }

    target->unlock();
}

void ChunkGeneratorFlat::generate(Chunk *target)
{
    target->lock(RWLock::WRITE);
    int c_width = Chunk::size();

    for(int x=0; x<c_width; x++)
    for(int y=0; y<c_width; y++)
    for(int z=0; z<c_width; z++)
    {
        long y_ = y + target->cpos().y * c_width;
        if(y_ < 2)
            target->set(x,y,z, Block::STONE);
        else if(y_ < 5)
            target->set(x,y,z, Block::DIRT);
        else if(y_ < 6)
            target->set(x,y,z, Block::GRASS);
    }

    target->unlock();
}

void ChunkGeneratorCrapHills::generate(Chunk *target)
{
    target->lock(RWLock::WRITE);
    int c_width = Chunk::size();

    for(int x=0; x<c_width; x++)
    for(int y=0; y<c_width; y++)
    for(int z=0; z<c_width; z++)
    {
        int offset = std::max(std::abs(x - c_width/2), std::abs(z - c_width/2));

        long y_ = y + target->cpos().y * c_width + offset;
        if(y_ < 2)
            target->set(x,y,z, Block::STONE);
        else if(y_ < 5)
            target->set(x,y,z, Block::DIRT);
        else if(y_ < 6)
            target->set(x,y,z, Block::GRASS);
    }

    target->unlock();
}
