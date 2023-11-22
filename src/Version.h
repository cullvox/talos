#pragma once

#include <stdint.h>

namespace ts {

#define TS_STRINGIFY(a) TS_XSTRINGIFY(a)
#define TS_XSTRINGIFY(a) #a

#define TALOS_VERSION_TAG "Beta"
#define TALOS_VERSION_MAJOR 0
#define TALOS_VERSION_MINOR 6
#define TALOS_VERSION_PATCH 5
#define TALOS_VERSION_CODE (uint32_t)(TALOS_VERSION_TAG[0]) << 24 | \
                            (uint32_t)(TALOS_VERSION_MAJOR) << 16 | \
                            (uint32_t)(TALOS_VERSION_MINOR) << 8 | \
                            (uint32_t)(TALOS_VERSION_PATCH);

#define TALOS_VERSION_STRING TALOS_VERSION_TAG " " TS_STRINGIFY(TALOS_VERSION_MAJOR) "." TS_STRINGIFY(TALOS_VERSION_MINOR) "." TS_STRINGIFY(TALOS_VERSION_PATCH) 

} /* namespace ts */