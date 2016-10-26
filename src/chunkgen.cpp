#include "chunkgen.hpp"

#include "noise.hpp"

ChunkGeneratorHeightmap::ChunkGeneratorHeightmap()
{
    c_width = Chunk::size();
    Heightmap init = {
        {0,0,0},
        0,
        c_width
    };
    std::fill_n(history, sizeof(history)/sizeof(*history), init);
}

ChunkGeneratorHeightmap::~ChunkGeneratorHeightmap()
{
    for(size_t i=0; i<sizeof(history)/sizeof(*history); i++)
    {
        if(history[i].data)
            delete[] history[i].data;
    }
}

float ChunkGeneratorHeightmap::Heightmap::get(int x, int z)
{
    return data[x + z*width];
}

void ChunkGeneratorHeightmap::Heightmap::set(int x, int z, float val)
{
    data[x + z*width] = val;
}

void ChunkGeneratorHeightmap::generate(Chunk *target)
{
    target->lock(Chunk::WRITE);

    long3_t cpos = target->cpos();

    Heightmap hmap;
    bool insert_into_history = true;

    int c_width = Chunk::size();

    hmap.pos = cpos;
    hmap.data = new float[c_width*c_width];
    hmap.width = c_width;

    history_m.lock();
    int i = history_index;
    do {
        if(history[i].pos.x == cpos.x &&
           history[i].pos.z == cpos.z &&
           history[i].data)
        {
            memcpy(hmap.data, history[i].data, hmap.width*hmap.width*sizeof(float));
            insert_into_history = false;
            history_m.unlock();
            goto have_hmap;
        }
        i = (i+1) % (sizeof(history)/sizeof(*history));
    } while(i != history_index);
    history_m.unlock();

    gen_map(hmap);

 have_hmap:

    if(cpos.y < 0)
        target->fill(Block::STONE);

    for(int x=0; x<c_width; x++)
    for(int z=0; z<c_width; z++)
    {
        float height = hmap.get(x, z);

        if(cpos.y < 0)
            for(int y=c_width-1; y>=0; y--)
            {
                if(y + cpos.y*c_width >= height)
                    target->set(x, y, z, Block::AIR);
                else
                    break;
            }
        else
            for(int y=0; y<c_width; y++)
            {
                if(y + cpos.y*c_width < height)
                    target->set(x, y, z, Block::STONE);
                else
                    break;
            }
    }

    target->unlock();

    if(insert_into_history)
    {
        history_m.lock();
        if(history[history_index].data)
            delete[] history[history_index].data;
        history[history_index].data = hmap.data;
        history[history_index].pos = hmap.pos;
        history[history_index].width = hmap.width;
        history_index = (history_index + 1) % (sizeof(history)/sizeof(*history));
        history_m.unlock();
    } else {
        delete[] hmap.data;
    }
}
