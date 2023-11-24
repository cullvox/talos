#pragma once

namespace ts {

struct Result {
    enum Enum {
        eOK = 0,
        eError = 1,
        eNoFetch, /* The slide wasn't fetched before being used. */
        

        // Hardware
        eStdioError,
        eCyw43Error,

        // Software
        eInvalidArgumentError,
        eOutOfMemoryError,
        eSDLError,
        eSchriftError,
    } value;

    Result() : value(eOK) {}
    Result(Enum value) : value(value) {}
    operator Enum() { return value; }
    explicit operator bool() const { return value > 0; }
    inline const char* ToString() {
        switch (value) {
        case Result::eOK: return "OK";
        case Result::eStdioError: return "stdio error";
        case Result::eCyw43Error: return "CYW43 Error";
        case Result::eError: return "Generic Error";
        case Result::eInvalidArgumentError: return "Invalid Argument Error";
        case Result::eOutOfMemoryError: return "Out Of Memory Error";
        case Result::eSDLError: return "SDL Error";
        case Result::eSchriftError: return "Freetype Error";
        default: return "Undefined Error";
        }
    }
};

} // namespace ts
