#ifndef BLOCKS_NOISE
#define BLOCKS_NOISE

#include <vector>

namespace Noise {
    unsigned int hash(unsigned int x);
    unsigned int hash(unsigned int x, unsigned int y);
    unsigned int hash(unsigned int x, unsigned int y, unsigned int z);

    unsigned int rotl(unsigned int x, unsigned int num);
    unsigned int rotr(unsigned int x, unsigned int num);

    double linear_interpolation(double a, double b, double t);
    double fade(double x);

    class Perlin2D {
    public:
        static double noise(unsigned int seed, double freq, double x, double y);

    private:
        static double grad(unsigned int hash, double x, double y);
    };

    class Perlin3D {
    public:
        static double noise(unsigned int seed, double freq, double x, double y, double z);

    private:
        static double grad(unsigned int hash, double x, double y, double z);
    };
};

#endif
