#ifndef TALOS_DISPLAY_H
#define TALOS_DISPLAY_H

#include <driver/spi_master.h>

#include "result.h"
#include "bitmap.h"

typedef struct _talos_display
{
    spi_host_device_t host;
    uint16_t cs_pin, reset_pin, dc_pin, busy_pin, power_pin;
    spi_device_handle_t handle;
} talos_display;

talos_res talos_display_init(talos_display* display); // Set pins and host before initialization!
talos_res talos_display_free(talos_display* display);

void talos_display_get_extent(talos_display* display, talos_ext* extent);
void talos_display_clear(talos_display* display);
void talos_display_present(talos_display* display, const talos_bitmap* bitmap);
void talos_display_sleep(talos_display* display);
void talos_display_reset(talos_display* display);

#endif