#include "chunkgen_perlin2d.hpp"

#include <algorithm>
#include "noise.hpp"

ChunkGeneratorPerlin::ChunkGeneratorPerlin()
{
    c_width = Chunk::size();
}

ChunkGeneratorPerlin::~ChunkGeneratorPerlin()
{
}

void ChunkGeneratorPerlin::generate(Chunk *target)
{
    target->lock(RWLock::WRITE);

    long3_t cpos = target->cpos();

    bool invert = false;
    if(cpos.y < 0)
        invert = true;

    if(invert)
      target->fill(Block::STONE);

    for(int x=0; x<c_width; x++)
    for(int z=0; z<c_width; z++)
    for(int y=c_width-1; y>=0; y--)
    {
        long X = x + cpos.x*c_width;
        long Y = y + cpos.y*c_width;
        long Z = z + cpos.z*c_width;
        float density =
            Noise::Perlin3D::noise(1, 0.01, X, Y*2, Z);

        density -= Y/60.0;

        if(density > 0 && !invert)
            target->set(x, y, z, Block::STONE);
        else if(density <= 0 && invert)
            target->set(x, y, z, Block::AIR);
    }

    target->unlock();
}
