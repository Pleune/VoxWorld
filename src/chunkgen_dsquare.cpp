#include "chunkgen_dsquare.hpp"

#include <algorithm>
#include "noise.hpp"
#include "custommath.h"

ChunkGeneratorDSquare::ChunkGeneratorDSquare()
{
    chunk_width = Chunk::size();
    heightmap_width = ipow(2, 7);

    hmap_levels = 0;
    for(int i=1; i<chunk_width; i*=2)
        hmap_levels++;
}

ChunkGeneratorDSquare::~ChunkGeneratorDSquare()
{
}

ChunkGeneratorDSquare::Heightmap::Heightmap(int levels, int x, int y)
    :levels(levels)
{
    side_len = ipow(2, levels) + 1;

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

float ChunkGeneratorDSquare::Heightmap::get(float x, float y)
{
    float xf = x*(side_len-2);
    float yf = y*(side_len-2);

    int xi = xf;
    int yi = yf;

    float xm = xf - xi;
    float ym = yf - yi;

    return
        (at(xi  , yi  ) +
         at(xi+1, yi  ) +
         at(xi  , yi+1) +
         at(xi+1, yi+1))/4.0;
}

float &ChunkGeneratorDSquare::Heightmap::at(int x, int y)
{
    assert(x < side_len);
    assert(y < side_len);
    return data[x + y*side_len];
}

float ChunkGeneratorDSquare::Heightmap::seed(int x, int y)
{
    return
        Noise::Perlin2D::noise(5, .01, x, y) * 1000 +
        Noise::Perlin2D::noise(1, .1, x, y) * 100 +
        Noise::Perlin2D::noise(2, .3, x, y) * 80 +
        Noise::Perlin2D::noise(3, .4, x, y) * 100 +
        Noise::Perlin2D::noise(4, .8, x, y) * 40;
}

void ChunkGeneratorDSquare::bias(double &h)
{
    h = (h*h*h) / (h*h + 300);
}

void ChunkGeneratorDSquare::generate(Chunk *target)
{
    target->lock(Chunk::WRITE);
    long3_t cpos = target->cpos();

    Heightmap hmap(hmap_levels, cpos.x, cpos.z);

    bool invert = false;
    if(cpos.y*chunk_width < hmap.get(0,0))
    {
        invert = true;
        target->fill(Block::STONE);
    }

    for(int x=0; x<chunk_width; x++)
    for(int z=0; z<chunk_width; z++)
    {
        double height = hmap.get((float)x/(float)(chunk_width-1), (float)z/(float)(chunk_width-1));

        bias(height);

        if(invert)
            for(int y=chunk_width-1; y>=0; y--)
            {
                long Y = y + cpos.y*chunk_width;
                if(Y >= height)
                {
                    if(Y >= 0)
                        target->set(x, y, z, Block::AIR);
                    else
                        target->set(x, y, z, Block::WATER);
                } else {
                    if(Y >= height - 3)
                    {
                        if(Y < 3)
                            target->set(x, y, z, Block::SAND);
                        else
                            target->set(x, y, z, Block::GRASS);
                    } else if(Y >= height - 7)
                    {
                        if(Y < 3)
                            target->set(x, y, z, Block::SAND);
                        else
                            target->set(x, y, z, Block::DIRT);
                    } else
                        break;
                }
            }
        else
            for(int y=0; y<chunk_width; y++)
            {
                long Y = y + cpos.y*chunk_width;
                if(Y < height-7)
                    target->set(x, y, z, Block::STONE);
                else if(Y < height - 3)
                    target->set(x, y, z, Block::DIRT);
                else if(Y < height)
                {
                    if(height < 1.55)
                        target->set(x, y, z, Block::SAND);
                    else
                        target->set(x, y, z, Block::GRASS);
                } else if(Y < 0)
                    target->set(x, y, z, Block::WATER);
                else
                    break;
            }
    }

    target->unlock();
}

bool ChunkGeneratorDSquare::lod(int level, LODMesh *ret, long3_t cpos)
{
    if(level > 1)
        return false;

    GLfloat points[5];
    points[0] = Heightmap::seed(cpos.x  , cpos.z  );
    points[1] = Heightmap::seed(cpos.x+1, cpos.z  );
    points[2] = Heightmap::seed(cpos.x+1, cpos.z+1);
    points[3] = Heightmap::seed(cpos.x  , cpos.z+1);
    points[4] = (points[0]+points[1]+points[2]+points[3])/4.0;

    for(int i=0; ; i++)
    {
        if(i == 5)
            return false;

        if(points[i] > cpos.y*chunk_width &&
           points[i] < (cpos.y+1)*chunk_width)
            break;
    }

    GLfloat x = (cpos.x)*chunk_width;
    GLfloat X = (cpos.x+1)*chunk_width;
    GLfloat z = (cpos.z)*chunk_width;
    GLfloat Z = (cpos.z+1)*chunk_width;
    GLfloat x_mid = (x + X)/2;
    GLfloat z_mid = (z + Z)/2;

    /*
      +z
      3---2
      | 4 |
      0---1 +x
    */
    LODMesh mesh = {
        {
            X    , points[1], z    , .05, .27, .1,
            x    , points[0], z    , .05, .27, .1,
            x_mid, points[4], z_mid, .05, .27, .1,

            X    , points[2], Z    , .05, .27, .1,
            X    , points[1], z    , .05, .27, .1,
            x_mid, points[4], z_mid, .05, .27, .1,

            x    , points[3], Z    , .05, .27, .1,
            X    , points[2], Z    , .05, .27, .1,
            x_mid, points[4], z_mid, .05, .27, .1,

            x    , points[0], z    , .05, .27, .1,
            x    , points[3], Z    , .05, .27, .1,
            x_mid, points[4], z_mid, .05, .27, .1,
        }
    };

    *ret = mesh;

    return true;
}
