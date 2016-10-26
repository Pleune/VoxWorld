#include "noise.hpp"

#include <random>
#include <algorithm>

unsigned int Noise::hash(unsigned int x)
{
	uint32_t tmp = x;
	tmp = (x+0x7ed55d16) + (tmp<<12);
	tmp = (x^0xc761c23c) ^ (tmp>>19);
	tmp = (x+0x165667b1) + (tmp<<5);
	tmp = (x+0xd3a2646c) ^ (tmp<<9);
	tmp = (x+0xfd7046c5) + (tmp<<3);
	tmp = (x^0xb55a4f09) ^ (tmp>>16);
	return tmp;
}

unsigned int Noise::hash(unsigned int x, unsigned int y)
{
    x = rotl(x, 7);
    y = rotl(y, 13);

    x *= 115589;
    y *= 119723;

    return hash(hash(x) ^ rotr(y,3));
}

unsigned int Noise::hash(unsigned int x, unsigned int y, unsigned int z)
{
    return (x*73856093) + (y*19349663) + (z*83492791);
}

unsigned int Noise::rotl(unsigned int x, unsigned int num)
{
    return (x << num) | (x >> (sizeof(x)*8 - num));
}

unsigned int Noise::rotr(unsigned int x, unsigned int num)
{
    return (x >> num) | (x << (sizeof(x)*8 - num));
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
double Noise::Perlin3D::noise(unsigned int seed, double freq, double x, double y, double z)
{
    x += seed * 7701409;
    y += seed * 1684861;
    z += seed * 9161497;

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

    unsigned int hashes[8];
    hashes[0] = hash(X,   Y,   Z  ); //aaa
    hashes[1] = hash(X+1, Y,   Z  ); //baa
    hashes[2] = hash(X,   Y+1, Z  ); //aba
    hashes[3] = hash(X+1, Y+1, Z  ); //bba
    hashes[4] = hash(X,   Y,   Z+1); //aab
    hashes[5] = hash(X+1, Y,   Z+1); //bab
    hashes[6] = hash(X,   Y+1, Z+1); //abb
    hashes[7] = hash(X+1, Y+1, Z+1); //bbb

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

//Returns the dot product of the gradient vector with the direction vector
double Noise::Perlin3D::grad(unsigned int hash, double x, double y, double z)
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

/*
Corners:

y
|
|
+---x

2---3
|   |
|   |
0---1
 */
double Noise::Perlin2D::noise(unsigned int seed, double freq, double x, double y)
{
    //freq 1/period
    x *= freq;
    y *= freq;

    int X = floor(x);
    int Y = floor(y);

    double Xf = x - X;
    double Yf = y - Y;

    double u = fade(Xf);
    double v = fade(Yf);

    unsigned int hashes[8];
    hashes[0] = hash(X,   Y  ); //aaa
    hashes[1] = hash(X+1, Y  ); //baa
    hashes[2] = hash(X,   Y+1); //aba
    hashes[3] = hash(X+1, Y+1); //bba

    return
        linear_interpolation(
            linear_interpolation(
                grad(hashes[0], Xf,   Yf  ), //aaa
                grad(hashes[1], Xf-1, Yf  ), //baa
                u),
            linear_interpolation(
                grad(hashes[2], Xf,   Yf-1), //aba
                grad(hashes[3], Xf-1, Yf-1), //bba
                u),
            v);
}

//Returns the dot product of the gradient vector with the direction vector
double Noise::Perlin2D::grad(unsigned int hash, double x, double y)
{
    hash = hash ^ (hash>>4) ^ (hash >> 8);
    hash &= 0b1111;
    switch(hash)
    {
    case 0x0: return  x;
    case 0x1: return -x;
    case 0x2: return  y;
    case 0x3: return -y;
    case 0x4: return  x + y;
    case 0x5: return  x - y;
    case 0x6: return -x + y;
    case 0x7: return -x - y;
    case 0x8: return  1.4*x;
    case 0x9: return  1.4*y;
    case 0xA: return -1.4*x;
    case 0xB: return -1.4*y;
    case 0xC: return  1.4*x + y;
    case 0xD: return  1.4*y + x;
    case 0xE: return -1.4*x - y;
    case 0xF: return -1.4*y - x;
    default:  return 0;//never
    }
}
