#include "chunkgen.hpp"

void ChunkGen::random(Chunk *target)
{
    target->lock(Chunk::WRITE);

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
