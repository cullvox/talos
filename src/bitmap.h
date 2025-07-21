#ifndef TALOS_BITMAP_H
#define TALOS_BITMAP_H

#include "result.h"
#include "vec.h"

typedef enum _talos_bitmap_color
{
    TALOS_COLOR_WHITE = 1,
    TALOS_COLOR_BLACK = 0
} talos_color;

typedef struct _talos_bitmap
{
    talos_ext extent;
    char* bytes;
} talos_bitmap;

talos_res talos_bitmap_init(talos_bitmap* bitmap);
talos_res talos_bitmap_free(talos_bitmap* bitmap);

uint32_t talos_bitmap_get_size_bytes(const talos_bitmap* bitmap);
void talos_bitmap_draw_bit(talos_vec pos, talos_color color);
void talos_bitmap_draw_bitmap(talos_vec source_start, talos_vec source_end, talos_vec destination_start, talos_bitmap* destination, const talos_bitmap* source);
void talos_bitmap_draw_horizontal_strip(talos_vec start, uint16_t length, talos_color color);
void talos_bitmap_draw_horizontal_wrapping_strip(talos_vec start, talos_vec end, talos_color color); // Draws a straight line that wraps around to the next line of pixels.

#endif