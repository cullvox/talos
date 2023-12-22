#pragma once

#include <stdint.h>

namespace ts {

#define TS_STRINGIFY(a) TS_XSTRINGIFY(a)
#define TS_XSTRINGIFY(a) #a

#ifndef TALOS_VERSION_GIT_TAG
#define TALOS_VERSION_GIT_TAG ""
#endif

#ifndef TALOS_VERSION_GIT_HASH
#define TALOS_VERSION_GIT_HASH ""
#endif

#ifndef TALOS_VERSION_GIT_HASH_SHORT
#define TALOS_VERSION_GIT_HASH_SHORT ""
#endif


#define TALOS_VERSION_TAG "Beta"
#define TALOS_VERSION_MAJOR 0
#define TALOS_VERSION_MINOR 7
#define TALOS_VERSION_PATCH 0
#define TALOS_VERSION_CODE (uint32_t)(TALOS_VERSION_TAG[0]) << 24 | \
                            (uint32_t)(TALOS_VERSION_MAJOR) << 16 | \
                            (uint32_t)(TALOS_VERSION_MINOR) << 8 | \
                            (uint32_t)(TALOS_VERSION_PATCH);

#define TALOS_VERSION_STRING TALOS_VERSION_GIT_TAG " " TALOS_VERSION_GIT_HASH_SHORT

} /* namespace ts */