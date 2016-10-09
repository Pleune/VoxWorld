#include "chunkgen_perlin.hpp"

#include "noise.hpp"

void ChunkGeneratorPerlin::generate(Chunk *target)
{
    target->lock(Chunk::WRITE);
    int c_width = Chunk::size();

    for(int x=0; x<c_width; x++)
    for(int y=0; y<c_width; y++)
    for(int z=0; z<c_width; z++)
    {
        long3_t cpos = target->cpos();

        int X = x + cpos.x*c_width;
        int Y = y + cpos.y*c_width;
        int Z = z + cpos.z*c_width;

        double density =
            Noise::Perlin::noise(0.01, X, Y, Z) +
            Noise::Perlin::noise(0.1, X, Y, Z) * .05;


        if(density - ((double)Y/100.0)> 0.0)
            target->set(x, y, z, Block::STONE);
    }

    target->unlock();
}
