#include "chunkgen_dsquare.hpp"

#include <algorithm>
#include "noise.hpp"
#include "custommath.h"

#define DLEVELS 6

ChunkGeneratorDSquare::ChunkGeneratorDSquare()
{
    chunk_width = Chunk::size();
    heightmap_width = ipow(2, 7);
}

ChunkGeneratorDSquare::~ChunkGeneratorDSquare()
{
}

template<int levels>
ChunkGeneratorDSquare::Heightmap<levels>::Heightmap(int x, int y)
{
    constexpr int side_len = ipow(2, levels) + 1;
    int size = side_len - 1;
    data = new float[side_len*side_len];

    //seed corners
    at(0         , 0         ) = seed(x  , y  );
    at(0         , side_len-1) = seed(x  , y+1);
    at(side_len-1, 0         ) = seed(x+1, y  );
    at(side_len-1, side_len-1) = seed(x+1, y+1);

    //fill in
    for(int a=side_len-1; a>1; )
    {
        int a_ = a/2;
        //diamond
        for(int bx=size - a_; bx>0; bx-=a)
        for(int by=size - a_; by>0; by-=a)
        {
            at(bx, by) =
                (at(bx + a_, by + a_) +
                 at(bx + a_, by - a_) +
                 at(bx - a_, by + a_) +
                 at(bx - a_, by - a_)) / 4.0;

            //TODO: add random factor
        }

        //square
        for(int bx=0; bx<side_len; bx+=a)
        for(int by=a_; by<side_len; by+=a)
        {
            bool notedge = (bx - a_ >= 0) && (bx + a/2 < side_len);

            at(by, bx) = at(by-a_, bx) + at(by+a_, bx);
            at(bx, by) = at(bx, by-a_) + at(bx, by+a_);

            if(notedge)
            {
                at(by, bx) += at(by, bx-a_) + at(by, bx+a_);
                at(bx, by) += at(bx-a_, by) + at(bx+a_, by);
            }

            at(by, bx) /= notedge ? 4.0 : 2.0;
            at(bx, by) /= notedge ? 4.0 : 2.0;

            //TODO: add random factor
        }

        a = a_;
    }
}

template<int levels>
float ChunkGeneratorDSquare::Heightmap<levels>::get(int x, int y)
{
    return
        (at(x  , y  ) +
         at(x+1, y  ) +
         at(x  , y+1) +
         at(x+1, y+1))/4.0;
}

template<int levels>
float &ChunkGeneratorDSquare::Heightmap<levels>::at(int x, int y)
{
    constexpr size_t side_len = ipow(2, levels) + 1;
    return data[x + y*side_len];
}

template<int levels>
float ChunkGeneratorDSquare::Heightmap<levels>::seed(int x, int y)
{
    return
        Noise::Perlin2D::noise(1, .1, x, y) * 300 +
        Noise::Perlin2D::noise(2, .3, x, y) * 120 +
        Noise::Perlin2D::noise(3, .4, x, y) * 60 +
        Noise::Perlin2D::noise(4, .8, x, y) * 40;
}

void ChunkGeneratorDSquare::generate(Chunk *target)
{
    target->lock(Chunk::WRITE);
    long3_t cpos = target->cpos();

    Heightmap<DLEVELS> hmap(cpos.x, cpos.z);

    bool invert = false;
    if(cpos.y*chunk_width < hmap.get(0,0))
    {
        invert = true;
        target->fill(Block::STONE);
    }

    for(int x=0; x<chunk_width; x++)
    for(int z=0; z<chunk_width; z++)
    {
        int height = floor(hmap.get(x, z));
        if(invert)
        for(int y=chunk_width-1; y>=0; y--)
        {
            if(y + cpos.y*chunk_width >= height)
            {
                target->set(x, y, z, Block::AIR);
            } else {
                break;
            }
        }
        else
        for(int y=0; y<chunk_width; y++)
        {
            if(y + cpos.y*chunk_width < height)
            {
                target->set(x, y, z, Block::STONE);
            } else {
                break;
            }
        }
    }

    target->unlock();
}

template float ChunkGeneratorDSquare::Heightmap<DLEVELS>::seed(int x, int y);
template ChunkGeneratorDSquare::Heightmap<DLEVELS>::Heightmap(int x, int yx);
template float ChunkGeneratorDSquare::Heightmap<DLEVELS>::get(int x, int y);
template float &ChunkGeneratorDSquare::Heightmap<DLEVELS>::at(int x, int y);
