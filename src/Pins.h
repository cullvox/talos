#pragma once

#include <SPI.h>

namespace ts {
namespace Pin {

/* General SPI Pins */
static inline const uint16_t SpiClk = 5;
static inline const uint16_t SpiCipo = 19;
static inline const uint16_t SpiCopi = 18;

/* SPI SD Card */
static inline const uint16_t SdSpiCs = 21;

/* SPI/Pins ePaper Display */
static inline const uint16_t PaperSpiCs = 33;
static inline const uint16_t PaperDc = 15;
static inline const uint16_t PaperRst = 32;
static inline const uint16_t PaperBusy = 14;
static inline const uint16_t PaperPwr = 27;

} /* namespace Pin */
} /* namespace ts */