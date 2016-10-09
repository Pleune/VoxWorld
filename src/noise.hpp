#ifndef BLOCKS_NOISE
#define BLOCKS_NOISE

#include <vector>

namespace Noise {
    unsigned int hash(unsigned int x);
    unsigned int hash(unsigned int x, unsigned int y);
    unsigned int hash(unsigned int x, unsigned int y, unsigned int z);

    double linear_interpolation(double a, double b, double t);
    double fade(double x);

    class Perlin {
    public:
        static double noise(double freq, double x, double y, double z);

    private:
        static double grad(int hash, double x, double y, double z);
    };
};

#endif
