#include "chunkgen_perlin2d.hpp"

#include <algorithm>
#include "noise.hpp"

void ChunkGeneratorPerlin2D::gen_map(Heightmap &map)
{
    long3_t cpos = map.pos;
    for(int x=0; x<map.width; x++)
    for(int z=0; z<map.width; z++)
    {
        map.set(x, z,
                 Noise::Perlin2D::noise(1, .001, x + cpos.x*c_width, z + cpos.z*c_width)*200 +
                 Noise::Perlin2D::noise(2, .008, x + cpos.x*c_width, z + cpos.z*c_width)*90 +
                 Noise::Perlin2D::noise(3, .03, x + cpos.x*c_width, z + cpos.z*c_width)*20 +
                 Noise::Perlin2D::noise(4, .1, x + cpos.x*c_width, z + cpos.z*c_width)*4
                 );
    }
}
