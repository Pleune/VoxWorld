#include "noise.hpp"

#include <random>
#include <algorithm>

unsigned int Noise::hash(unsigned int x)
{
    x = (x << 13) ^ x;
    return (x * (x * x * 15731 + 789221) + 1376312589);
}

unsigned int Noise::hash(unsigned int x, unsigned int y)
{
    return (x*73856093) + (y*19349663);
}

unsigned int Noise::hash(unsigned int x, unsigned int y, unsigned int z)
{
    return (x*73856093) + (y*19349663) + (z*83492791);
}

double Noise::linear_interpolation(double a, double b, double t)
{
    return a + t * (b-a);
}

double Noise::fade(double x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

/*
Corners:

  z
y |
 \|
  +---x

6---7
|\  |\
| 4---5
2-|-3 |
 \|  \|
  0---1
 */
double Noise::Perlin::noise(double freq, double x, double y, double z)
{
    //freq 1/period
    x *= freq;
    y *= freq;
    z *= freq;

    int X = floor(x);
    int Y = floor(y);
    int Z = floor(z);

    double Xf = x - X;
    double Yf = y - Y;
    double Zf = z - Z;

    double u = fade(Xf);
    double v = fade(Yf);
    double w = fade(Zf);

    int hashes[8];
    hashes[0] = hash(X,   Y,   Z  ) & 255; //aaa
    hashes[1] = hash(X+1, Y,   Z  ) & 255; //baa
    hashes[2] = hash(X,   Y+1, Z  ) & 255; //aba
    hashes[3] = hash(X+1, Y+1, Z  ) & 255; //bba
    hashes[4] = hash(X,   Y,   Z+1) & 255; //aab
    hashes[5] = hash(X+1, Y,   Z+1) & 255; //bab
    hashes[6] = hash(X,   Y+1, Z+1) & 255; //abb
    hashes[7] = hash(X+1, Y+1, Z+1) & 255; //bbb

    return linear_interpolation(
        linear_interpolation(
            linear_interpolation(
                grad(hashes[0], Xf,   Yf,   Zf  ), //aaa
                grad(hashes[1], Xf-1, Yf,   Zf  ), //baa
                u),
            linear_interpolation(
                grad(hashes[2], Xf,   Yf-1, Zf  ), //aba
                grad(hashes[3], Xf-1, Yf-1, Zf  ), //bba
                u),
            v),
        linear_interpolation(
            linear_interpolation(
                grad(hashes[4], Xf,   Yf,   Zf-1), //aab
                grad(hashes[5], Xf-1, Yf,   Zf-1), //bab
                u),
            linear_interpolation(
                grad(hashes[6], Xf,   Yf-1, Zf-1), //abb
                grad(hashes[7], Xf-1, Yf-1, Zf-1), //bbb
                u),
            v),
        w);
}

//Returns the
double Noise::Perlin::grad(int hash, double x, double y, double z)
{
    switch(hash & 0xF)
    {
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default:  return 0;//never
    }
}
