#pragma once

#include "Result.h"

namespace ts {

/** @brief Classes that are error 'prone' should be based on this class. */
class Prone {
protected:
    Result _what;
public:
    Prone() = default;
    ~Prone() = default;

    virtual Result what() 
    { 
        return _what;
    }

    virtual bool ok()
    {
        return !_what; /* operator bool() on ts::Result returns true on failure. */
    }
};

} /* namespace ts */