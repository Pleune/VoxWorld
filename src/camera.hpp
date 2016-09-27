#ifndef BLOCKS_CAMERA_H
#define BLOCKS_CAMERA_H

#include "custommath.h"

struct Camera {
    vec3_t pos;
    vec3_t forward;
    vec3_t up;
};

#endif
