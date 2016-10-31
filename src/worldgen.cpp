#include "worldgen.hpp"

#include "block.hpp"
#include <cmath>

void WorldGen::normal(Chunk *chunk, Chunk *chunkabove, Chunk *chunkbelow)
{
    return;
    int c_len = Chunk::size();

    int dirtheight = std::min(4, c_len);

    Block::ID blocks_above[c_len][dirtheight+1][c_len];

    chunkabove->lock(Chunk::READ);
    for(int x=0; x<c_len; x++)
    for(int y=0; y<=dirtheight; y++)
    for(int z=0; z<c_len; z++)
        blocks_above[x][y][z] = chunkabove->get(x, y, z);
    chunkabove->unlock();

    chunk->lock(Chunk::WRITE);
    for(int x=0; x<c_len; x++)
    for(int z=0; z<c_len; z++)
    {
        Block::ID block_above = blocks_above[x][dirtheight][z];
        for(int y=c_len+dirtheight-1; y>=0; y--)
        {
            Block::ID block;
            if(y>=c_len)
                block = blocks_above[x][y-c_len][z];
            else
                block = chunk->get(x, y, z);

            if(block_above == Block::AIR && (block == Block::STONE || block == Block::GRASS))
            {
                if(y < c_len)
                    chunk->set(x, y, z, Block::GRASS);
                for(int i=1; i<dirtheight; i++)
                {
                    y--;
                    if(y<0)
                        break;
                    if(y < c_len)
                        chunk->set(x, y, z, Block::DIRT);
                }
            }

            block_above = block;
        }
    }
    chunk->unlock();
}
