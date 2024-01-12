#include "Config.h"

namespace ts {

Config& GetConfig()
{
    static Config config;
    return config;
}

} /* namespace ts */