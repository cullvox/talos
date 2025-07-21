#ifndef TALOS_RECT_H
#define TALOS_RECT_H

#include <stdint.h>

typedef struct _talos_vector
{
    int16_t x;
    int16_t y;
} talos_vec;

typedef struct _talos_extent
{
    uint16_t width;
    uint16_t height;
} talos_ext;

typedef struct _talos_rectangle
{
    talos_vec offset;
    talos_ext extent;
} talos_rect;

#endif