#include "chunkgen_perlin.hpp"

#include "noise.hpp"

void ChunkGeneratorPerlin::generate(Chunk *target)
{
    target->lock(Chunk::WRITE);
    int c_width = Chunk::size();

    long3_t cpos = target->cpos();

    if(cpos.y < 0)
        target->fill(Block::STONE);

    for(int x=0; x<c_width; x++)
    for(int y=0; y<c_width; y++)
    for(int z=0; z<c_width; z++)
    {
        int X = x + cpos.x*c_width;
        int Y = y + cpos.y*c_width;
        int Z = z + cpos.z*c_width;

        if(Y < 120 && y > -120)
        {

            double mod = Noise::Perlin::noise(0.0002, X, 0, Z) + 1;
            mod *= 60;

            double density =
                Noise::Perlin::noise(0.01, X, Y, Z) +
                Noise::Perlin::noise(0.1, X, Y, Z) * .02;

            density -= ((double)Y/(1.0 + mod));

            if(cpos.y < 0)
            {
                if(density <= 0)
                    target->set(x, y, z, Block::AIR);
            } else {
                if(density > 0.0)
                    target->set(x, y, z, Block::STONE);
            }
        }
    }

    target->unlock();
}
