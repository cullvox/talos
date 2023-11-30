#include <Arduino.h>
#include <ArduinoJson.h>
#include <StreamString.h>
#include <HTTPClient.h>

#include "SlideLastFM.h"
#include "Print.h"
#include "Secrets.h"

const uint8_t bayerThresholdMap [4][4] = {
    {15, 135, 45, 165},
    {195, 75, 225, 105},
    {60, 180, 30, 150},
    {240, 120, 210, 90},
};

int JPEGDraw(JPEGDRAW *pDraw)
{
    TS_INFO("Drawing image!\n");

    bool dither = true;
    int16_t x = pDraw->x;
    int16_t y = pDraw->y;
    int16_t w = pDraw->iWidth;
    int16_t h = pDraw->iHeight;

    ts::BitmapInterface* pBuffer = (ts::BitmapInterface*)pDraw->pUser;
    uint8_t threshold = 128;

    for(int16_t j = 0; j < h; j++)
    {
        for(int16_t i = 0; i < w; i++)
        {
            pDraw->pPixels[i + j * w] = (pDraw->pPixels[i + j * w] & 0x7e0) >> 5; // extract just the six green channel bits.
        
            // Bayer Dithering
            // 4x4 Bayer ordered dithering algorithm
            const uint16_t bayerx = (i + j * w) % w;
            const uint16_t bayery = floor((i + j * w) / w);
            const uint16_t bayerMapped = 0 + (pDraw->pPixels[i + j * w] - 0) * (255 - 0) / (63 - 0);
            const uint16_t map = floor((bayerMapped + bayerThresholdMap[bayerx % 4][bayery % 4]) / 2);
            const ts::Vector2i pos{x+i, y+j};
            pBuffer->set(pos, (map < threshold) ? false : true);
        }
    }

    

    // 
    // if (dither)
    // {
    //     for(int16_t j = 0; j < h; j++)
    //     {
    //         for(int16_t i = 0; i < w; i++)
    //         {
    //             int8_t oldPixel = constrain(pDraw->pPixels[i + j * w], 0, 0x3F);
    //             int8_t newPixel = oldPixel & 0x30; // or 0x30 to dither to 2-bit directly. much improved tonal range, but more horizontal banding between blocks.
    //             pDraw->pPixels[i + j * w] = newPixel;
    //             int quantError = oldPixel - newPixel;      
    //             if(i + 1 < w) 
    //                 pDraw->pPixels[i + 1 + j * w] += quantError * 7 / 16;
    //             if((i - 1 >= 0) && (j + 1 < h)) 
    //                 pDraw->pPixels[i - 1 + (j + 1) * w] += quantError * 3 / 16;
    //             if(j + 1 < h) 
    //                 pDraw->pPixels[i + (j + 1) * w] += quantError * 5 / 16;
    //             if((i + 1 < w) && (j + 1 < h)) 
    //                 pDraw->pPixels[i + 1 + (j + 1) * w] += quantError * 1 / 16;
    //         } // for i
    //     } // for j
    // } // if dither
  // 
    // ts::BitmapInterface* pBuffer = (ts::BitmapInterface*)pDraw->pUser;
    // ts::Vector2i pos;
    // for(int16_t i = 0; i < w; i++)
    // {
    //     for(int16_t j = 0; j < h; j++)
    //     {
    //         pos = ts::Vector2i{(int16_t)(x+i), (int16_t)(y+j)};
    //         
    //         switch (constrain(pDraw->pPixels[i + j * w] >> 4, 0, 3))
    //         {
    //             
    //             case 0:
    //             pBuffer->set(pos, false);
    //             break;
    //             case 1:
    //             case 2:
    //             case 3:
    //                 pBuffer->set(pos, true);
    //             break;
    //         } // switch
    //     } // for j
    // } // for i
    return 1;
} /* JPEGDraw() */

namespace ts {


// array size is 4361
static const uint8_t Lusine[] PROGMEM  = {
  0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x01, 0x2c, 
  0x01, 0x2c, 0x00, 0x00, 0xff, 0xdb, 0x00, 0x43, 0x00, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 
  0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x06, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x06, 
  0x06, 0x05, 0x06, 0x09, 0x08, 0x0a, 0x0a, 0x09, 0x08, 0x09, 0x09, 0x0a, 0x0c, 0x0f, 0x0c, 0x0a, 
  0x0b, 0x0e, 0x0b, 0x09, 0x09, 0x0d, 0x11, 0x0d, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x10, 0x0a, 0x0c, 
  0x12, 0x13, 0x12, 0x10, 0x13, 0x0f, 0x10, 0x10, 0x10, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x03, 0x03, 
  0x03, 0x04, 0x03, 0x04, 0x08, 0x04, 0x04, 0x08, 0x10, 0x0b, 0x09, 0x0b, 0x10, 0x10, 0x10, 0x10, 
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0xc0, 
  0x00, 0x11, 0x08, 0x00, 0x80, 0x00, 0x80, 0x03, 0x01, 0x11, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 
  0x01, 0xff, 0xc4, 0x00, 0x1d, 0x00, 0x01, 0x00, 0x02, 0x03, 0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x04, 0x07, 0x08, 0x01, 0x03, 0x09, 0x02, 
  0xff, 0xc4, 0x00, 0x41, 0x10, 0x00, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x06, 0x03, 0x06, 
  0x05, 0x05, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x00, 0x05, 0x11, 0x06, 0x21, 0x07, 0x12, 
  0x31, 0x41, 0x51, 0x61, 0x71, 0x08, 0x13, 0x22, 0x32, 0x81, 0x14, 0x15, 0x23, 0x42, 0x52, 0xa1, 
  0x62, 0x72, 0x91, 0x17, 0x33, 0x43, 0x82, 0x93, 0xc1, 0x16, 0x24, 0xa2, 0xb1, 0xb2, 0x36, 0x44, 
  0x73, 0xd1, 0xf1, 0xff, 0xc4, 0x00, 0x14, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc4, 0x00, 0x14, 0x11, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
  0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00, 0xf9, 0x55, 0x40, 
  0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 
  0x81, 0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x06, 0x4f, 0xda, 0x62, 0x72, 0xe3, 0xee, 0xf4, 
  0x67, 0x04, 0x67, 0xde, 0x2b, 0xaf, 0x26, 0x33, 0xd7, 0xb2, 0xbe, 0x2f, 0xdb, 0xa5, 0x01, 0xc9, 
  0x31, 0x16, 0xda, 0x90, 0x8b, 0x73, 0x68, 0x51, 0xe6, 0xc2, 0xc3, 0x8b, 0x24, 0x65, 0x40, 0x8e, 
  0xa7, 0x1b, 0x00, 0x53, 0xf5, 0xdf, 0x7c, 0x1a, 0x0c, 0x6a, 0x05, 0x02, 0x81, 0x40, 0xa0, 0x50, 
  0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 
  0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x04, 0x9d, 0x97, 0x4c, 0xdf, 0xb5, 0x0b, 0x8a, 0x45, 0x9a, 
  0xd5, 0x22, 0x50, 0x6f, 0xfb, 0xc5, 0xa1, 0x38, 0x6d, 0xbf, 0x35, 0xac, 0xe1, 0x29, 0xfa, 0x91, 
  0x41, 0x3e, 0xd7, 0x0d, 0x64, 0xa3, 0x1f, 0x79, 0x6a, 0x4b, 0x34, 0x55, 0x7e, 0x64, 0x21, 0xd5, 
  0xc8, 0x50, 0xff, 0x00, 0x49, 0x2a, 0x4f, 0xfd, 0x54, 0x1f, 0xbf, 0xec, 0xee, 0xde, 0xe0, 0x22, 
  0x36, 0xb5, 0xb7, 0x29, 0x63, 0xb2, 0xe3, 0xbe, 0x91, 0x9f, 0x50, 0x93, 0xff, 0x00, 0x6a, 0x0c, 
  0x49, 0x5c, 0x34, 0xd4, 0xa8, 0x4a, 0x97, 0x6c, 0x4c, 0x4b, 0xc2, 0x52, 0x32, 0x7e, 0xee, 0x90, 
  0x1e, 0x73, 0xfd, 0x2d, 0x9c, 0xff, 0x00, 0xa6, 0x82, 0xae, 0xe3, 0x6e, 0x32, 0xe2, 0x9a, 0x75, 
  0x0a, 0x42, 0xd0, 0x4a, 0x54, 0x95, 0x0c, 0x10, 0x47, 0x50, 0x47, 0x63, 0x41, 0xf9, 0xa0, 0xda, 
  0x1a, 0x1b, 0xd9, 0x9b, 0x8d, 0xbc, 0x41, 0x8e, 0xdd, 0xc2, 0xc5, 0xa1, 0x26, 0xb3, 0x01, 0xd1, 
  0x94, 0xcd, 0xb8, 0x29, 0x30, 0x98, 0x50, 0xf1, 0x4a, 0x9e, 0x29, 0xe6, 0x1f, 0xca, 0x0d, 0x06, 
  0xc9, 0x87, 0xec, 0x0b, 0xc5, 0x57, 0xd1, 0x99, 0x7a, 0xc3, 0x45, 0xc6, 0x5f, 0xe8, 0xfb, 0x7b, 
  0xce, 0x91, 0xea, 0x50, 0xd1, 0x1f, 0xbd, 0x04, 0x75, 0xe7, 0xd8, 0x4f, 0x8e, 0x56, 0xe6, 0x16, 
  0xfd, 0xa4, 0xe9, 0xcb, 0xef, 0x2e, 0xe1, 0xa8, 0x17, 0x54, 0xa5, 0xc3, 0xe8, 0x97, 0x83, 0x79, 
  0xfa, 0x50, 0x69, 0x5d, 0x5f, 0xa0, 0x75, 0xa6, 0x80, 0xb8, 0x1b, 0x56, 0xb4, 0xd2, 0xf7, 0x3b, 
  0x2c, 0xaf, 0xca, 0x89, 0x91, 0xd4, 0xdf, 0x3f, 0x9a, 0x49, 0x18, 0x50, 0xf3, 0x04, 0x8a, 0x08, 
  0x1a, 0x05, 0x02, 0x81, 0x40, 0x03, 0x34, 0x1b, 0x06, 0xc1, 0xa1, 0x20, 0x5a, 0x61, 0xb3, 0x7b, 
  0xd6, 0x8d, 0x29, 0x6b, 0x7d, 0x01, 0xd8, 0xb6, 0xa0, 0xa2, 0x85, 0x2d, 0x04, 0x65, 0x2e, 0x3e, 
  0xa1, 0xba, 0x10, 0x46, 0xe1, 0x03, 0x0a, 0x50, 0xdf, 0x29, 0x18, 0x24, 0x2c, 0x8d, 0xa2, 0xf1, 
  0xa9, 0xa2, 0x85, 0xb9, 0x21, 0x9b, 0x5d, 0x8a, 0x2a, 0x8b, 0x68, 0x21, 0xbe, 0x48, 0xe9, 0x50, 
  0xea, 0x86, 0x5a, 0x4e, 0x39, 0xd7, 0xe3, 0x8e, 0x9f, 0x99, 0x42, 0x83, 0xdf, 0x0d, 0xab, 0x62, 
  0x24, 0x22, 0x0e, 0x9c, 0xb0, 0x2a, 0xe5, 0x29, 0x59, 0x21, 0xe9, 0xa9, 0xf7, 0xee, 0x2b, 0x1d, 
  0xc3, 0x63, 0xf0, 0xd0, 0x3d, 0x41, 0xc7, 0x75, 0x50, 0x4d, 0xa2, 0x5d, 0xda, 0x26, 0xd3, 0xf5, 
  0x43, 0x71, 0x08, 0xdf, 0xec, 0xf0, 0x1b, 0x0b, 0x29, 0xf2, 0xf8, 0x39, 0x50, 0x3e, 0x84, 0xd0, 
  0x65, 0xc4, 0xb8, 0x99, 0xbf, 0x2c, 0xbb, 0xb4, 0xe0, 0x83, 0xd5, 0x70, 0x19, 0x7c, 0x0f, 0xa1, 
  0x07, 0x07, 0xeb, 0x41, 0x62, 0x8d, 0xc3, 0x4b, 0x07, 0x18, 0xa4, 0xb1, 0xa7, 0x2e, 0x56, 0xc9, 
  0x31, 0xee, 0xf2, 0x7f, 0x0e, 0x2d, 0xc9, 0xb8, 0x8b, 0x69, 0xe6, 0xc8, 0x19, 0x1c, 0xe1, 0x45, 
  0x49, 0x71, 0x03, 0xba, 0x4a, 0xc6, 0xdf, 0x2e, 0x0d, 0x07, 0x4f, 0xf0, 0x33, 0xd8, 0x4f, 0x49, 
  0x70, 0x89, 0xb6, 0x2e, 0xfa, 0xa5, 0x98, 0x77, 0x8b, 0xf2, 0xbf, 0x11, 0xab, 0x8c, 0xb5, 0xa1, 
  0x0c, 0xa7, 0xf8, 0xa3, 0xa5, 0x41, 0x45, 0x18, 0xcf, 0x52, 0x8e, 0x71, 0xfa, 0x87, 0x4a, 0x0e, 
  0x81, 0x3a, 0x36, 0x0a, 0xdc, 0x57, 0xbb, 0x45, 0x8d, 0xe7, 0x02, 0x09, 0x25, 0x7e, 0xf2, 0x4a, 
  0xc2, 0xbc, 0xd4, 0xb2, 0x76, 0xc0, 0xfd, 0x38, 0xa0, 0xc6, 0x95, 0xa5, 0x16, 0x86, 0xd4, 0xa9, 
  0x9a, 0x4e, 0xd5, 0x3d, 0x9e, 0x9c, 0xf1, 0x99, 0x41, 0x50, 0x18, 0xee, 0x9c, 0x25, 0x59, 0xfe, 
  0x5c, 0x9e, 0x9b, 0x50, 0x56, 0x25, 0x68, 0xfd, 0x33, 0x76, 0x2a, 0x55, 0x8e, 0x44, 0x9b, 0x4c, 
  0x94, 0xa8, 0xa0, 0xa3, 0x9c, 0xb8, 0xda, 0x57, 0xfa, 0x54, 0xda, 0xb7, 0x1b, 0xf8, 0x11, 0xd7, 
  0xa7, 0x6a, 0x0a, 0x3e, 0xb0, 0xb5, 0x3c, 0x88, 0x4b, 0xd3, 0x3c, 0x40, 0xb0, 0xc0, 0xbd, 0x59, 
  0xe5, 0x65, 0x29, 0x4c, 0x86, 0xc3, 0xf1, 0x9e, 0x3d, 0xf9, 0x49, 0x19, 0x4a, 0xc7, 0xf9, 0x54, 
  0x31, 0x91, 0xe3, 0x41, 0xc7, 0x3c, 0x7e, 0xf6, 0x46, 0x66, 0xc7, 0x6f, 0x95, 0xaf, 0x78, 0x41, 
  0xf6, 0x89, 0x76, 0x96, 0x12, 0x5e, 0x9d, 0x66, 0x71, 0x45, 0xc9, 0x10, 0x90, 0x37, 0x2b, 0x69, 
  0x5d, 0x5d, 0x68, 0x77, 0x07, 0xe3, 0x48, 0x19, 0xf8, 0x86, 0x48, 0x0e, 0x59, 0xa0, 0x50, 0x28, 
  0x2f, 0xdc, 0x36, 0xd3, 0xd1, 0x1a, 0x8e, 0xfe, 0xb9, 0xbd, 0xc6, 0x43, 0xd1, 0x60, 0x3a, 0x19, 
  0x81, 0x1d, 0xc0, 0x0a, 0x25, 0x4c, 0xc0, 0x56, 0x54, 0x3b, 0xa1, 0xb0, 0x52, 0xa5, 0x0e, 0x84, 
  0xa9, 0xb4, 0x9c, 0x82, 0x68, 0x2c, 0x96, 0xb8, 0xab, 0xd5, 0x57, 0x29, 0x97, 0x8b, 0xf4, 0xa7, 
  0xcc, 0x08, 0x87, 0xdf, 0xcd, 0x77, 0x9b, 0xf1, 0x1e, 0x5a, 0x89, 0xe5, 0x69, 0x24, 0xfe, 0x75, 
  0x90, 0x77, 0xfc, 0xa9, 0x0a, 0x57, 0x60, 0x08, 0x67, 0x05, 0x4e, 0xd5, 0xb7, 0x44, 0x43, 0x8c, 
  0x19, 0x89, 0x16, 0x3b, 0x79, 0xc2, 0x52, 0x43, 0x10, 0xe3, 0x27, 0xa9, 0x00, 0x76, 0x19, 0xe9, 
  0xd5, 0x4a, 0x23, 0xa9, 0x39, 0xa0, 0x95, 0x4b, 0x89, 0xc2, 0x6c, 0x1a, 0x66, 0x33, 0xc9, 0x6d, 
  0xe5, 0x25, 0x07, 0x18, 0xf7, 0xd2, 0x97, 0xd9, 0x4e, 0x1f, 0xdf, 0x97, 0xe5, 0x4f, 0xf5, 0x24, 
  0x3d, 0xce, 0x42, 0x81, 0x66, 0x41, 0x1c, 0xad, 0x4f, 0x9a, 0x37, 0x53, 0x8b, 0x1c, 0xcc, 0x36, 
  0x7c, 0x10, 0x93, 0xf3, 0x91, 0xfa, 0x95, 0xb7, 0x80, 0xef, 0x41, 0x9f, 0x06, 0xc5, 0xa8, 0xaf, 
  0x92, 0xda, 0x8d, 0x2a, 0x74, 0x84, 0xad, 0xe1, 0xcc, 0xdc, 0x74, 0x02, 0x56, 0x13, 0xfa, 0x8a, 
  0x76, 0x4b, 0x69, 0xf5, 0xfa, 0x0a, 0x0e, 0xeb, 0xf6, 0x64, 0xe0, 0x4d, 0xb7, 0x41, 0x59, 0x91, 
  0x79, 0x96, 0x87, 0x5c, 0xba, 0xce, 0x42, 0x1c, 0x75, 0xc7, 0x03, 0xae, 0x14, 0x27, 0xa8, 0x40, 
  0xe4, 0x09, 0x48, 0xf3, 0xdc, 0xef, 0x41, 0xd4, 0x56, 0x17, 0xe1, 0xbe, 0x93, 0x6d, 0x7e, 0x54, 
  0x75, 0x32, 0x7a, 0x06, 0xd9, 0x42, 0x56, 0xc9, 0xfd, 0x43, 0x18, 0x3e, 0xbb, 0x6e, 0x33, 0x41, 
  0x05, 0xa9, 0x19, 0x45, 0xae, 0x51, 0x89, 0xa8, 0x6d, 0x51, 0x4b, 0x47, 0x76, 0xe5, 0x36, 0x90, 
  0xa6, 0xd4, 0x3b, 0x6e, 0x7e, 0x26, 0xfe, 0xbb, 0x03, 0xde, 0x82, 0x02, 0x54, 0x79, 0x16, 0xe0, 
  0x93, 0x0d, 0xc7, 0x1f, 0x8a, 0x01, 0x06, 0x3a, 0x97, 0xcc, 0xac, 0x78, 0x36, 0xa3, 0xeb, 0xb2, 
  0x55, 0x90, 0x71, 0xb1, 0x1b, 0x50, 0x61, 0xca, 0xb5, 0x45, 0xd4, 0x08, 0x4d, 0xd2, 0x2b, 0x81, 
  0xa7, 0xc2, 0x00, 0x69, 0xf0, 0xd9, 0xc9, 0x1b, 0x82, 0x87, 0x07, 0xe6, 0x00, 0xf3, 0x02, 0x93, 
  0x82, 0x37, 0xc6, 0x0d, 0x05, 0x5d, 0xf8, 0xed, 0xce, 0x66, 0x4d, 0x86, 0xf5, 0x0c, 0x29, 0xa1, 
  0xf8, 0x0e, 0x30, 0xa3, 0x90, 0x9e, 0xe0, 0x85, 0x7f, 0x42, 0x95, 0x75, 0xdb, 0xc7, 0x60, 0x1a, 
  0xa2, 0xe9, 0x6f, 0xb8, 0xe8, 0x6d, 0x40, 0x21, 0x97, 0x96, 0xe4, 0x75, 0xfe, 0x2c, 0x57, 0x8f, 
  0xf8, 0xad, 0x67, 0x1b, 0xff, 0x00, 0x10, 0x3b, 0x11, 0xe8, 0x7b, 0xd0, 0x71, 0x5f, 0xb5, 0xef, 
  0x05, 0x21, 0x68, 0xab, 0xdb, 0x1c, 0x45, 0xd2, 0x50, 0xd2, 0xcd, 0x83, 0x50, 0xba, 0xa4, 0xbf, 
  0x1d, 0xb1, 0x84, 0x42, 0x9b, 0x82, 0xa5, 0x24, 0x0e, 0xc8, 0x58, 0xca, 0x92, 0x3b, 0x10, 0xa1, 
  0xd0, 0x0a, 0x0e, 0x73, 0xa0, 0xf2, 0x90, 0x54, 0xa0, 0x90, 0x09, 0x27, 0xa0, 0x1d, 0xe8, 0x37, 
  0x16, 0xb4, 0x69, 0x1a, 0x75, 0x8b, 0x7e, 0x8e, 0x61, 0x40, 0x37, 0x62, 0x88, 0x96, 0x1e, 0x29, 
  0x3b, 0x2a, 0x49, 0xf8, 0xe4, 0x2f, 0xfd, 0x45, 0x28, 0x7a, 0x25, 0x23, 0xb5, 0x06, 0x7d, 0xe9, 
  0x95, 0x58, 0x2c, 0x76, 0xcd, 0x34, 0x3e, 0x07, 0x52, 0xc8, 0x9d, 0x38, 0x77, 0x32, 0x5e, 0x48, 
  0x56, 0x0f, 0xf2, 0x37, 0xee, 0xd1, 0xe5, 0x85, 0x78, 0xd0, 0x66, 0xb4, 0xc8, 0xb2, 0xe9, 0xc8, 
  0xb0, 0x91, 0xf0, 0xc9, 0xbb, 0x25, 0x33, 0xa5, 0x1e, 0x84, 0x23, 0x7f, 0x70, 0xdf, 0xa0, 0x4e, 
  0x56, 0x47, 0x8a, 0xc7, 0x80, 0xa0, 0xb2, 0x69, 0xfb, 0x4a, 0xed, 0xf6, 0x54, 0xcd, 0x48, 0x3f, 
  0x6c, 0xba, 0x25, 0x49, 0x41, 0xee, 0xdc, 0x60, 0x79, 0x4e, 0x3c, 0xd6, 0xa0, 0x47, 0xf2, 0xa7, 
  0xf8, 0x8d, 0x05, 0xcf, 0x43, 0xf0, 0xba, 0xf7, 0xa8, 0x65, 0x34, 0x2d, 0xd6, 0xe5, 0x49, 0x9a, 
  0xf2, 0xca, 0x22, 0xa1, 0x49, 0xf8, 0x10, 0x46, 0x0a, 0xde, 0x5f, 0x82, 0x53, 0x91, 0x8f, 0xe2, 
  0xf1, 0xc0, 0x14, 0x1d, 0x35, 0xc2, 0xfe, 0x03, 0xda, 0xf4, 0xdd, 0xc2, 0x2c, 0x79, 0x41, 0xd9, 
  0x32, 0x54, 0xe0, 0x7a, 0x6c, 0xa2, 0xd8, 0xe7, 0x78, 0xa7, 0x7e, 0x51, 0x9c, 0x90, 0x9c, 0xe3, 
  0x6e, 0xb8, 0xf3, 0x34, 0x1d, 0x32, 0xd2, 0x54, 0xdc, 0x72, 0xa8, 0xb3, 0x42, 0x9b, 0x47, 0xe5, 
  0x58, 0x05, 0x1f, 0xb6, 0x08, 0xa0, 0xc3, 0x7e, 0xe9, 0x1a, 0x50, 0x28, 0x5b, 0x45, 0x0f, 0x30, 
  0x73, 0x82, 0x7e, 0x24, 0x9e, 0xca, 0x4a, 0x87, 0x40, 0x7f, 0xfd, 0xa0, 0x90, 0x45, 0xf6, 0x3d, 
  0xda, 0xd1, 0xf7, 0x75, 0xdd, 0xd4, 0x38, 0xb4, 0x12, 0x12, 0xe0, 0x3f, 0x11, 0x03, 0x1f, 0x17, 
  0x80, 0x38, 0x20, 0x11, 0xd0, 0xd0, 0x50, 0xd3, 0x79, 0x16, 0x5b, 0xc7, 0xdc, 0xb2, 0x1e, 0x41, 
  0x61, 0xd0, 0x57, 0x0d, 0xc2, 0x7a, 0x01, 0xf3, 0x36, 0x0f, 0x86, 0x37, 0x03, 0xb6, 0x14, 0x3c, 
  0x28, 0x27, 0x60, 0x48, 0x44, 0x4b, 0x93, 0x6f, 0xb2, 0x9c, 0xc5, 0xb9, 0x2d, 0x2d, 0xbc, 0x80, 
  0x32, 0x03, 0xf8, 0xc2, 0x17, 0x8f, 0xe2, 0xf9, 0x4f, 0x9f, 0x27, 0x9d, 0x06, 0x36, 0xbd, 0xb4, 
  0xa2, 0x32, 0xda, 0xbd, 0x86, 0x55, 0x86, 0x08, 0x62, 0x48, 0xe4, 0x07, 0x2d, 0x28, 0xfc, 0x27, 
  0x6f, 0xd2, 0xa2, 0x0f, 0xa1, 0x57, 0x8d, 0x05, 0x0f, 0x89, 0x96, 0x14, 0x5d, 0x34, 0x8c, 0x89, 
  0x28, 0xf8, 0xa5, 0xda, 0xb3, 0x2d, 0xa5, 0x0c, 0x92, 0xa4, 0x8c, 0x85, 0x8f, 0x3c, 0xa3, 0x3f, 
  0x54, 0x8f, 0x0a, 0x0d, 0x15, 0xac, 0xf4, 0xac, 0x7e, 0x25, 0xf0, 0xd7, 0x51, 0x68, 0x79, 0x21, 
  0x2e, 0x39, 0x3a, 0x1a, 0xdc, 0x88, 0xa2, 0x32, 0x53, 0x29, 0xbf, 0x8d, 0xa5, 0x0f, 0xf3, 0x00, 
  0x3d, 0x09, 0xa0, 0xf9, 0x9a, 0xe3, 0x6b, 0x69, 0xc5, 0x36, 0xe2, 0x4a, 0x56, 0x92, 0x52, 0xa4, 
  0x91, 0x82, 0x08, 0xea, 0x28, 0x27, 0xb8, 0x79, 0x0d, 0xbb, 0x8e, 0xbc, 0xd3, 0x90, 0x5e, 0x48, 
  0x53, 0x6f, 0xdd, 0x62, 0x36, 0xb0, 0x7a, 0x14, 0x97, 0x53, 0x9f, 0xdb, 0x34, 0x17, 0x7b, 0xca, 
  0xcd, 0xfb, 0x58, 0x84, 0x3e, 0xac, 0xfd, 0xe3, 0x72, 0x4a, 0x56, 0x7c, 0x7d, 0xeb, 0xc0, 0x1f, 
  0xfc, 0x8d, 0x04, 0xa6, 0xb8, 0x90, 0xab, 0x86, 0xa8, 0xb8, 0xad, 0x27, 0x1e, 0xfa, 0x63, 0xa8, 
  0x48, 0x1d, 0x81, 0x59, 0x03, 0xfa, 0x0c, 0x50, 0x4e, 0x6b, 0x3f, 0xfd, 0x47, 0x3d, 0xa6, 0x53, 
  0xca, 0x88, 0xce, 0x16, 0x1b, 0x1d, 0x39, 0x52, 0xd8, 0xe4, 0x48, 0xc7, 0xa2, 0x45, 0x06, 0xf6, 
  0xe1, 0x47, 0x0e, 0xe7, 0x71, 0x17, 0x88, 0xb6, 0xbe, 0x1e, 0xd9, 0xbd, 0xdf, 0xda, 0xdd, 0x4b, 
  0x30, 0xd9, 0x0e, 0x1c, 0x24, 0x25, 0xb6, 0x87, 0x3a, 0x89, 0xf0, 0x18, 0x51, 0x34, 0x1f, 0x48, 
  0xf4, 0x97, 0xb2, 0xf4, 0x5d, 0x05, 0x64, 0x8b, 0x68, 0xb3, 0xa6, 0x34, 0x99, 0x0b, 0x65, 0x28, 
  0x93, 0x2c, 0x8e, 0x52, 0xb5, 0x0d, 0xcf, 0x5e, 0x89, 0xc9, 0x38, 0x1e, 0x7e, 0x34, 0x1e, 0x47, 
  0x0a, 0x75, 0x15, 0xa9, 0xc7, 0x6e, 0x7f, 0x77, 0x05, 0xb6, 0xd0, 0xf7, 0x00, 0x05, 0x82, 0xa5, 
  0x2c, 0xa8, 0x64, 0x81, 0xd7, 0x1d, 0x07, 0xd2, 0x82, 0x3a, 0xed, 0xa3, 0xf5, 0x9c, 0x4b, 0x83, 
  0x96, 0xf8, 0xd6, 0x47, 0xd6, 0xf3, 0x71, 0x8c, 0xa7, 0xd2, 0x82, 0x92, 0x02, 0x37, 0x01, 0x59, 
  0xce, 0xe7, 0x62, 0x30, 0x37, 0xeb, 0x41, 0xab, 0x35, 0x02, 0xb5, 0x6c, 0x78, 0xf6, 0x7b, 0xa4, 
  0x5b, 0x1c, 0xe5, 0x37, 0x7e, 0x7d, 0x51, 0x20, 0x14, 0x37, 0x93, 0x2c, 0x93, 0x80, 0x86, 0xc7, 
  0x52, 0x72, 0x33, 0xda, 0x82, 0xb8, 0xde, 0xa9, 0xd4, 0x30, 0xa3, 0xdf, 0x8c, 0xbb, 0x4c, 0xc6, 
  0xd5, 0x60, 0x92, 0xdb, 0x73, 0x8a, 0x98, 0x50, 0x11, 0x1c, 0x3c, 0xc9, 0xe4, 0x70, 0xfe, 0x55, 
  0x1c, 0x63, 0x07, 0xc2, 0x83, 0x4f, 0xf1, 0x1f, 0x8d, 0x77, 0x0b, 0x33, 0xb0, 0x51, 0x3e, 0xc9, 
  0x74, 0x2f, 0x38, 0x5b, 0x9f, 0x1d, 0xb0, 0xc1, 0xf7, 0x8b, 0x6b, 0xe6, 0x0e, 0xa0, 0x63, 0x2a, 
  0x49, 0x48, 0x56, 0xe3, 0x20, 0x8e, 0x6f, 0x03, 0x41, 0xd1, 0xfc, 0x1d, 0xba, 0xff, 0x00, 0xc7, 
  0xfc, 0x38, 0x46, 0xad, 0xb7, 0x67, 0xec, 0xef, 0x47, 0x12, 0x63, 0xba, 0xa1, 0xb9, 0x52, 0x0f, 
  0x3a, 0x48, 0xfa, 0xa4, 0x50, 0x6d, 0xcd, 0x4d, 0x69, 0x66, 0x5d, 0x9e, 0x5b, 0x58, 0x4a, 0x1b, 
  0x7a, 0x3b, 0x83, 0x09, 0x4f, 0x72, 0x93, 0x83, 0xcd, 0xdb, 0x7c, 0x1f, 0x5a, 0x0d, 0x6c, 0x59, 
  0xfb, 0x64, 0x24, 0x85, 0x47, 0xe5, 0x4c, 0xb6, 0x30, 0xe2, 0x08, 0xdb, 0xe3, 0x48, 0xcf, 0xf5, 
  0xc9, 0x3f, 0x53, 0x41, 0xce, 0x1a, 0x61, 0x4b, 0x8b, 0x39, 0xa4, 0x03, 0xfd, 0xda, 0xc2, 0x0f, 
  0xf9, 0x4e, 0x3f, 0xda, 0x83, 0xe6, 0xf7, 0x16, 0xad, 0x6d, 0xd9, 0x78, 0xa3, 0xab, 0x6d, 0x4c, 
  0xa4, 0x25, 0xb8, 0xb7, 0xa9, 0xad, 0xa1, 0x23, 0xa0, 0x4f, 0xbe, 0x56, 0x07, 0xf4, 0xa0, 0xc2, 
  0xd0, 0x13, 0x9b, 0xb6, 0x6b, 0xad, 0x3d, 0x71, 0x79, 0x41, 0x2d, 0xc6, 0xba, 0x45, 0x75, 0x64, 
  0xfe, 0x90, 0xea, 0x73, 0xfb, 0x66, 0x82, 0xef, 0x7c, 0x4a, 0xac, 0x1a, 0xc7, 0x9d, 0xe4, 0xe0, 
  0xdb, 0xae, 0x29, 0x5a, 0xc7, 0xff, 0x00, 0x1b, 0xa0, 0x9f, 0xfc, 0x68, 0x25, 0xf5, 0xc4, 0x65, 
  0x40, 0xd4, 0xf7, 0x14, 0xa3, 0x70, 0xd4, 0xb7, 0x56, 0x82, 0x3b, 0x8e, 0x62, 0x53, 0xeb, 0x91, 
  0x8a, 0x09, 0xad, 0x62, 0x90, 0xad, 0x43, 0x39, 0xf6, 0x3e, 0x24, 0x4b, 0x5f, 0xda, 0x1a, 0x23, 
  0xf3, 0x21, 0xd1, 0xce, 0x0e, 0x4f, 0x92, 0x85, 0x06, 0xf3, 0xe1, 0x57, 0x11, 0xae, 0x1c, 0x3a, 
  0xd7, 0xf6, 0xce, 0x20, 0x58, 0xd4, 0xd0, 0x98, 0x86, 0xda, 0x96, 0xc2, 0xdc, 0x4e, 0x52, 0x42, 
  0xdb, 0x01, 0x69, 0x23, 0xc3, 0xe6, 0x06, 0x83, 0xe8, 0xce, 0x8a, 0xf6, 0xa5, 0x1c, 0x41, 0xb1, 
  0x46, 0xba, 0xdb, 0xde, 0x8d, 0x12, 0x73, 0x4c, 0xa5, 0x72, 0x62, 0x27, 0x7e, 0x52, 0x76, 0x27, 
  0x7d, 0xca, 0x72, 0x0e, 0x0f, 0x6e, 0x86, 0x83, 0xdc, 0x8e, 0x28, 0x6a, 0x2b, 0xb3, 0xcf, 0x5b, 
  0x95, 0x74, 0x5a, 0x1a, 0x7c, 0x7b, 0xee, 0x50, 0x90, 0x08, 0x58, 0x50, 0xce, 0x0f, 0x50, 0x37, 
  0x49, 0xc5, 0x06, 0x15, 0xd3, 0x52, 0xeb, 0x09, 0x93, 0x17, 0x39, 0x9b, 0xf4, 0x94, 0xba, 0xa8, 
  0xe6, 0x2b, 0xee, 0x21, 0x40, 0x73, 0x27, 0xb2, 0x31, 0x8c, 0x77, 0x27, 0x3f, 0xfd, 0xd0, 0x6b, 
  0x0b, 0xf4, 0x2d, 0x4d, 0x2d, 0xab, 0x65, 0xa9, 0x8b, 0xcd, 0xc0, 0x22, 0xce, 0xea, 0xa5, 0xc3, 
  0x4a, 0x64, 0x29, 0x3f, 0x64, 0xdf, 0xe7, 0x6f, 0x1f, 0x21, 0xc9, 0xc6, 0xd4, 0x15, 0xc6, 0xf4, 
  0x75, 0xe2, 0x5c, 0x6b, 0xd2, 0x65, 0x5c, 0xa6, 0xad, 0xdb, 0xdc, 0x84, 0x2a, 0x57, 0x33, 0xca, 
  0x22, 0x4a, 0x87, 0x31, 0x2a, 0x5e, 0xff, 0x00, 0x12, 0xb7, 0xce, 0x4f, 0x8d, 0x06, 0xa3, 0xe2, 
  0x07, 0x02, 0xbe, 0xf9, 0x93, 0x0d, 0x37, 0x0b, 0x8d, 0xc5, 0x4a, 0x0b, 0x44, 0x56, 0x48, 0x7d, 
  0x5c, 0xfc, 0x99, 0xc7, 0x22, 0x4e, 0x72, 0x94, 0x84, 0xf3, 0x6c, 0x30, 0x00, 0xcd, 0x07, 0x4b, 
  0xf0, 0x93, 0x4f, 0xb1, 0xa0, 0x78, 0x7a, 0x8d, 0x2b, 0x6c, 0xe6, 0x4c, 0x76, 0xda, 0xfb, 0x3c, 
  0x76, 0x4a, 0xba, 0x29, 0x78, 0x40, 0x03, 0x3e, 0x64, 0x50, 0x6d, 0x3d, 0x47, 0x73, 0x6a, 0x3d, 
  0xaa, 0x5b, 0xa1, 0x49, 0x52, 0x19, 0x61, 0xcd, 0xc1, 0xdf, 0x21, 0x27, 0x03, 0x07, 0x63, 0xb9, 
  0x02, 0x83, 0x5a, 0xb8, 0xe2, 0x60, 0xc1, 0x0a, 0x2f, 0x92, 0x88, 0xac, 0xf3, 0x29, 0x6a, 0x57, 
  0x50, 0x84, 0xef, 0xf4, 0x18, 0x20, 0x7a, 0x1a, 0x0e, 0x70, 0xd2, 0xcd, 0x2e, 0x54, 0xe6, 0x94, 
  0x3a, 0xb8, 0xb0, 0xb2, 0x7f, 0x98, 0xe7, 0xfd, 0xe8, 0x3e, 0x6d, 0xf1, 0x66, 0xe8, 0xdd, 0xef, 
  0x89, 0xfa, 0xb2, 0xec, 0xca, 0x82, 0x9b, 0x95, 0x7a, 0x98, 0xe2, 0x14, 0x3a, 0x14, 0x97, 0x95, 
  0x83, 0xfd, 0x31, 0x41, 0x54, 0x07, 0x04, 0x10, 0x71, 0xe7, 0x41, 0xb7, 0xf5, 0x7b, 0xc9, 0xd4, 
  0x70, 0xed, 0xba, 0xc5, 0x94, 0x85, 0x22, 0xf5, 0x15, 0x2b, 0x7f, 0x03, 0xe5, 0x94, 0x8f, 0x81, 
  0xf4, 0x9f, 0xf3, 0x82, 0xaf, 0x45, 0xa7, 0xc6, 0x82, 0x4e, 0xe6, 0xe9, 0xbf, 0x69, 0xfb, 0x66, 
  0xa2, 0x1f, 0x1b, 0xc8, 0x68, 0x5b, 0xe6, 0xf8, 0x87, 0xd9, 0x48, 0x00, 0x9f, 0xe7, 0x6f, 0x91, 
  0x5e, 0x67, 0x9b, 0xc0, 0xd0, 0x66, 0xc6, 0x78, 0x5e, 0x74, 0xdc, 0x79, 0x29, 0x24, 0xca, 0xb3, 
  0x84, 0xc3, 0x92, 0x06, 0xe4, 0xb2, 0x49, 0xf7, 0x2e, 0x7a, 0x75, 0x6c, 0x9e, 0xc5, 0x29, 0xf1, 
  0x14, 0x16, 0x3d, 0x3f, 0x75, 0x5d, 0xc6, 0xd0, 0x2d, 0xc1, 0x58, 0x9b, 0x6d, 0x4a, 0x96, 0xd0, 
  0x1d, 0x5c, 0x8e, 0x4f, 0x32, 0x80, 0xf3, 0x42, 0x89, 0x3f, 0xca, 0xa3, 0xfa, 0x68, 0x2d, 0x9a, 
  0x2f, 0x89, 0x77, 0x8b, 0x14, 0xa6, 0x45, 0xbe, 0xe6, 0xa8, 0x73, 0x58, 0x70, 0xb9, 0x11, 0xd5, 
  0x2b, 0xe0, 0xe6, 0x23, 0x0b, 0x69, 0x7e, 0x29, 0x5e, 0x07, 0x5d, 0xb3, 0xe1, 0x9c, 0x80, 0xe9, 
  0xde, 0x17, 0x71, 0xea, 0xdd, 0xa8, 0x6e, 0x11, 0x9c, 0x9e, 0x97, 0x22, 0xcc, 0x42, 0xc3, 0x33, 
  0xa2, 0x17, 0x40, 0x53, 0x65, 0x5b, 0x73, 0x27, 0x38, 0x25, 0x39, 0xc7, 0x7e, 0x9e, 0x62, 0x83, 
  0xa7, 0x99, 0x2e, 0xbd, 0x1b, 0x11, 0x22, 0xa1, 0x0d, 0x2c, 0x63, 0x9d, 0x44, 0x14, 0xfd, 0x00, 
  0xc9, 0xff, 0x00, 0xb5, 0x06, 0x1b, 0xd0, 0x22, 0x44, 0x24, 0x05, 0xa9, 0xd7, 0xde, 0x3b, 0x8c, 
  0x65, 0xc5, 0x1e, 0xd8, 0x03, 0xb0, 0xfe, 0x9f, 0xb9, 0xa0, 0xcf, 0x16, 0x68, 0xd6, 0x6b, 0x3f, 
  0xde, 0x37, 0x64, 0xb6, 0x1c, 0x59, 0x2a, 0x08, 0x03, 0xe5, 0x07, 0xf2, 0xe7, 0xa1, 0xe8, 0x09, 
  0x3d, 0x3f, 0x6a, 0x0a, 0x54, 0x4b, 0x29, 0xbd, 0x5e, 0x05, 0xf2, 0x4b, 0x49, 0x4b, 0x4d, 0xa4, 
  0xa6, 0x22, 0x14, 0x9c, 0x65, 0x27, 0x65, 0x38, 0x07, 0x86, 0x36, 0x4e, 0x77, 0x39, 0x27, 0xb8, 
  0xa0, 0xb3, 0xc4, 0x88, 0x99, 0x73, 0x90, 0xcb, 0x40, 0xfd, 0x96, 0xda, 0xa0, 0xa7, 0x54, 0x0e, 
  0x01, 0x7b, 0x07, 0x95, 0x20, 0xff, 0x00, 0x0e, 0xea, 0x3e, 0x7c, 0xbe, 0x06, 0x83, 0x0f, 0x5c, 
  0x5d, 0x19, 0x71, 0x68, 0xb2, 0xf3, 0xe4, 0x3b, 0x87, 0xe4, 0x92, 0xb1, 0xb3, 0x60, 0x9e, 0x44, 
  0x91, 0xfc, 0x4a, 0x00, 0x7a, 0x25, 0x54, 0x1a, 0xeb, 0x8a, 0x37, 0xe4, 0xda, 0xb4, 0x9b, 0xd0, 
  0x50, 0x7f, 0xe7, 0x2e, 0xc4, 0xc5, 0x69, 0x20, 0x6e, 0x10, 0x77, 0x70, 0xfa, 0x04, 0x9c, 0x7a, 
  0xa8, 0x50, 0x68, 0x8d, 0x75, 0xaa, 0xd8, 0xe1, 0x87, 0x0b, 0xf5, 0x16, 0xb7, 0x90, 0xa4, 0xa1, 
  0xf8, 0x91, 0x14, 0xcc, 0x24, 0x93, 0x82, 0xb9, 0x4e, 0x0e, 0x46, 0x80, 0xf4, 0x51, 0xcf, 0xa2, 
  0x49, 0xa0, 0xf9, 0x94, 0xb5, 0xa9, 0xc5, 0xa9, 0x6b, 0x51, 0x52, 0x94, 0x72, 0x49, 0x39, 0x24, 
  0xf7, 0x34, 0x1e, 0x28, 0x2f, 0x5c, 0x39, 0xd4, 0x70, 0x83, 0x32, 0x34, 0x5d, 0xf6, 0x4b, 0x6c, 
  0x41, 0xb8, 0xb8, 0x1e, 0x89, 0x25, 0xc3, 0x84, 0x44, 0x99, 0x8e, 0x50, 0xa5, 0x1e, 0xcd, 0xac, 
  0x61, 0x0b, 0x3d, 0xb0, 0x85, 0x1d, 0x92, 0x68, 0x2c, 0xb6, 0xd9, 0x4f, 0xe8, 0xfb, 0xb4, 0xbb, 
  0x5d, 0xee, 0x1b, 0xc6, 0x0c, 0xac, 0x31, 0x3d, 0x8c, 0x7e, 0x22, 0x0a, 0x49, 0xe5, 0x71, 0x00, 
  0xed, 0xce, 0x82, 0x49, 0x1d, 0x94, 0x0a, 0x93, 0xd1, 0x59, 0xa0, 0x93, 0xe4, 0x9b, 0xa5, 0xae, 
  0x2d, 0x5c, 0xed, 0xee, 0x33, 0x2e, 0x2c, 0x86, 0xc9, 0x6d, 0xc4, 0xe5, 0x4c, 0x4c, 0x8e, 0xaf, 
  0x99, 0x27, 0xbe, 0x0f, 0x42, 0x36, 0x52, 0x54, 0x3b, 0x29, 0x34, 0x12, 0x61, 0x96, 0xdf, 0x48, 
  0xbe, 0xe9, 0x87, 0xde, 0x21, 0x85, 0x25, 0xc5, 0xa0, 0x1f, 0xc7, 0x88, 0xac, 0xed, 0xcd, 0x8e, 
  0xa3, 0xb0, 0x58, 0x1c, 0xa7, 0xbe, 0x0e, 0x45, 0x07, 0xb9, 0xdb, 0x8d, 0xbe, 0xee, 0x8f, 0xc6, 
  0x5b, 0x56, 0xf9, 0xc7, 0x63, 0x91, 0x88, 0xee, 0x9f, 0x10, 0x47, 0xf7, 0x64, 0xf8, 0x1f, 0x87, 
  0xc0, 0x8e, 0x94, 0x19, 0xf0, 0xae, 0xda, 0x96, 0xcb, 0x21, 0x99, 0x0e, 0xc2, 0x7a, 0x47, 0xb9, 
  0xc0, 0x66, 0x4b, 0x4a, 0x3c, 0xe9, 0x4f, 0x87, 0xbc, 0x4e, 0x52, 0xb4, 0xe3, 0xb2, 0xb3, 0xe5, 
  0x8a, 0x0e, 0xe5, 0xf6, 0x65, 0xe3, 0xa4, 0x2d, 0x6f, 0x69, 0x45, 0x86, 0xe0, 0xc3, 0xcc, 0xdd, 
  0x21, 0x36, 0x96, 0xd6, 0x87, 0x9b, 0x5a, 0x7d, 0xe2, 0x7b, 0x2c, 0x16, 0xce, 0x0f, 0x9e, 0x47, 
  0x5f, 0x5a, 0x0e, 0x9b, 0xb5, 0x3f, 0x0a, 0x1b, 0x66, 0xe2, 0xea, 0x61, 0x36, 0xc9, 0xcf, 0x2a, 
  0x51, 0x21, 0x3c, 0xcf, 0x28, 0x74, 0x4e, 0xc3, 0x3e, 0xbb, 0xec, 0x3c, 0xce, 0xe1, 0x07, 0x78, 
  0x99, 0x1a, 0xe7, 0x2b, 0xed, 0xba, 0x82, 0xf3, 0x1d, 0xec, 0x83, 0xc9, 0x11, 0xac, 0x06, 0xd0, 
  0x3b, 0x61, 0x03, 0x2a, 0x5e, 0x3c, 0xf2, 0x3b, 0xe3, 0x34, 0x11, 0xef, 0x3d, 0x36, 0xe3, 0xc8, 
  0x62, 0xb2, 0xf4, 0x78, 0x6a, 0x05, 0x4a, 0x7d, 0x69, 0xe5, 0x74, 0x8f, 0xe0, 0x41, 0xf9, 0x7c, 
  0x94, 0xad, 0xf7, 0xd8, 0x1e, 0xc1, 0xfa, 0x97, 0x7c, 0x87, 0x62, 0x6b, 0xee, 0xc8, 0x48, 0x6d, 
  0xd7, 0xd2, 0xd0, 0x0d, 0xb0, 0x16, 0x70, 0xd2, 0x46, 0x4f, 0x33, 0x87, 0xf2, 0x83, 0xb9, 0x39, 
  0xca, 0x8f, 0x6c, 0x93, 0x41, 0x4d, 0x9b, 0x35, 0x9b, 0x63, 0x32, 0x6f, 0xd7, 0xa9, 0x9c, 0xad, 
  0x13, 0xef, 0x56, 0xf2, 0x87, 0xc4, 0xb5, 0x11, 0x8c, 0x04, 0xf8, 0x9c, 0x00, 0x94, 0x0e, 0x9f, 
  0x4c, 0xd0, 0x6a, 0x7b, 0x84, 0xcb, 0x8e, 0xb7, 0xbf, 0x1b, 0x8b, 0xed, 0x16, 0x98, 0x48, 0xf7, 
  0x71, 0xd9, 0x27, 0x66, 0x5a, 0x07, 0x38, 0x27, 0xa6, 0x4e, 0xe5, 0x47, 0xfd, 0x85, 0x07, 0x10, 
  0x7b, 0x5d, 0x71, 0xc2, 0x17, 0x10, 0x75, 0x03, 0x1a, 0x13, 0x48, 0xcb, 0x0f, 0x69, 0xbd, 0x3a, 
  0xe2, 0xb9, 0x9f, 0x41, 0xf8, 0x27, 0x4c, 0xc7, 0x2a, 0x9d, 0x1e, 0x28, 0x48, 0xca, 0x50, 0x7b, 
  0xe5, 0x47, 0xb8, 0xa0, 0xe7, 0x8a, 0x05, 0x02, 0x82, 0xf9, 0xa7, 0xb5, 0xd4, 0x19, 0xf0, 0x9a, 
  0xb0, 0x6b, 0x45, 0x38, 0x5b, 0x61, 0x01, 0xa8, 0x77, 0x34, 0x24, 0xad, 0xd8, 0xe8, 0x1b, 0x06, 
  0xdd, 0x4f, 0x57, 0x1b, 0x1d, 0x88, 0xf8, 0x92, 0x36, 0x1c, 0xc3, 0x09, 0x01, 0x67, 0x8e, 0x6f, 
  0x7a, 0x6a, 0x22, 0x9d, 0x8e, 0x88, 0xf7, 0x7b, 0x0c, 0xa5, 0x73, 0x1e, 0x45, 0xfb, 0xd8, 0xab, 
  0x57, 0x4e, 0x64, 0xad, 0x3b, 0xb4, 0xe7, 0xf4, 0x57, 0x65, 0x24, 0xf4, 0xa0, 0xf7, 0xc6, 0x93, 
  0x64, 0x90, 0xfa, 0x26, 0xd9, 0x6f, 0x8f, 0x5a, 0x65, 0xa3, 0x74, 0xb5, 0x31, 0x45, 0xb5, 0x20, 
  0xf8, 0x25, 0xf4, 0x7c, 0x27, 0xeb, 0xcb, 0xe6, 0x28, 0x26, 0xd0, 0x8b, 0xbc, 0xc0, 0x15, 0x36, 
  0xc5, 0x02, 0xe5, 0x9f, 0xfd, 0xc4, 0x59, 0x08, 0x6d, 0x6a, 0x3e, 0x24, 0xb4, 0x4a, 0x49, 0xf3, 
  0x29, 0xa0, 0xcd, 0x87, 0x6c, 0x61, 0x85, 0x25, 0x29, 0xb6, 0xca, 0x84, 0xb5, 0x1d, 0x82, 0xae, 
  0xec, 0xb4, 0x49, 0xf2, 0x01, 0x3c, 0xc7, 0xe8, 0x33, 0x41, 0x35, 0xfd, 0xa6, 0xe9, 0xae, 0x10, 
  0x49, 0x6e, 0xf7, 0x2a, 0x5b, 0x93, 0x6f, 0xb1, 0x87, 0x3c, 0x5b, 0x73, 0x52, 0x5c, 0x75, 0xce, 
  0x6c, 0x74, 0x75, 0x6e, 0x6c, 0xd2, 0x3c, 0x72, 0xd8, 0x51, 0x1d, 0x01, 0xeb, 0x41, 0xd1, 0xfc, 
  0x0f, 0xf6, 0xee, 0xd3, 0x5c, 0x53, 0x4b, 0x36, 0x7d, 0x71, 0x32, 0x2d, 0x96, 0xfc, 0x00, 0x6d, 
  0x98, 0xb2, 0x92, 0xda, 0xe2, 0x39, 0xd3, 0xe1, 0x60, 0xba, 0x40, 0xdf, 0x6f, 0x85, 0x4e, 0x25, 
  0x59, 0xfd, 0x5d, 0x68, 0x3a, 0x45, 0xad, 0x61, 0x19, 0x92, 0x0a, 0x67, 0xda, 0x23, 0x15, 0x20, 
  0xe3, 0xdf, 0x46, 0x76, 0x31, 0x56, 0xdb, 0x9c, 0x64, 0x24, 0x8d, 0xfc, 0x71, 0x41, 0xeb, 0x99, 
  0xaa, 0x98, 0x5a, 0x54, 0x99, 0xda, 0xde, 0xdf, 0x19, 0x9c, 0x1f, 0xc1, 0x88, 0xe0, 0x49, 0xc7, 
  0x9a, 0x92, 0x54, 0xbc, 0xef, 0xd0, 0x11, 0xeb, 0x41, 0x01, 0x27, 0x5a, 0xd8, 0xad, 0x89, 0x53, 
  0x3a, 0x72, 0xdd, 0x22, 0xe8, 0xee, 0x79, 0x83, 0x8b, 0x41, 0x61, 0xa2, 0xac, 0x7c, 0xc4, 0xab, 
  0xe2, 0x56, 0xfe, 0x09, 0xec, 0x37, 0xef, 0x41, 0x44, 0xd6, 0xd7, 0x98, 0xf0, 0x21, 0x1d, 0x57, 
  0xc5, 0x0d, 0x4d, 0x06, 0xcb, 0x6d, 0x60, 0x95, 0x36, 0xa9, 0x2b, 0x0c, 0xb2, 0x8c, 0x8e, 0x8d, 
  0x37, 0xf3, 0x2d, 0x47, 0xcb, 0x99, 0x47, 0x34, 0x1c, 0x5f, 0xed, 0x07, 0xed, 0x78, 0xad, 0x5f, 
  0x02, 0x56, 0x82, 0xe1, 0x43, 0x52, 0x2d, 0xba, 0x79, 0xf4, 0x96, 0xa6, 0xdc, 0x9d, 0x4f, 0xbb, 
  0x95, 0x70, 0x47, 0x74, 0x24, 0x7f, 0x84, 0xd1, 0xee, 0x3e, 0x65, 0x0d, 0x8e, 0x06, 0x52, 0x43, 
  0x98, 0x68, 0x14, 0x0a, 0x05, 0x02, 0x83, 0x3e, 0xcf, 0x7f, 0xbd, 0x58, 0x1f, 0x32, 0x6c, 0xd7, 
  0x49, 0x30, 0xd6, 0xad, 0x96, 0x59, 0x70, 0xa4, 0x2c, 0x78, 0x28, 0x74, 0x50, 0xf2, 0x20, 0x8a, 
  0x0b, 0x02, 0x38, 0x97, 0x73, 0x58, 0x1f, 0x78, 0xd9, 0x2c, 0xb3, 0x97, 0x9d, 0xdc, 0x5c, 0x42, 
  0xca, 0xcf, 0xa9, 0x65, 0x48, 0x1f, 0xb5, 0x07, 0xeb, 0xfb, 0x45, 0x6d, 0x1b, 0xb3, 0xa3, 0x2c, 
  0x61, 0x7f, 0xa9, 0x7e, 0xfd, 0x78, 0xfa, 0x17, 0x28, 0x31, 0x66, 0x71, 0x23, 0x55, 0x3e, 0xda, 
  0xe3, 0xc2, 0x98, 0xd5, 0xad, 0x95, 0x8c, 0x16, 0xed, 0xac, 0x22, 0x36, 0x47, 0x81, 0x5a, 0x07, 
  0x39, 0x1e, 0xaa, 0x34, 0x15, 0x95, 0x29, 0x4a, 0x25, 0x4a, 0x24, 0x92, 0x72, 0x49, 0xef, 0x41, 
  0xe3, 0x34, 0x1b, 0x13, 0x43, 0x7b, 0x42, 0xf1, 0x97, 0x87, 0x2c, 0x22, 0x16, 0x95, 0xd7, 0xf7, 
  0x36, 0x21, 0x23, 0x18, 0x84, 0xfa, 0xc4, 0x98, 0xc0, 0x0e, 0xc1, 0xa7, 0x42, 0x92, 0x91, 0xe8, 
  0x05, 0x06, 0xcc, 0x89, 0xed, 0xed, 0xc6, 0xb8, 0xed, 0x84, 0x3f, 0x6b, 0xd2, 0x13, 0x15, 0x8d, 
  0xd6, 0xed, 0xa0, 0xa5, 0x44, 0xf8, 0xfe, 0x1b, 0x89, 0x1f, 0xb5, 0x04, 0x75, 0xf3, 0xdb, 0x8f, 
  0x8f, 0xf7, 0x66, 0x4b, 0x30, 0x2f, 0x56, 0x9b, 0x18, 0x57, 0x55, 0x5b, 0x2d, 0x4d, 0x36, 0xbf, 
  0xa2, 0xd6, 0x16, 0xa1, 0xea, 0x0d, 0x06, 0x97, 0xd4, 0xda, 0xbb, 0x54, 0xeb, 0x3b, 0x8a, 0xae, 
  0xfa, 0xb3, 0x50, 0xdc, 0x6f, 0x13, 0x15, 0x9f, 0xc7, 0x9b, 0x25, 0x6f, 0x2c, 0x0f, 0x00, 0x54, 
  0x4e, 0x07, 0x90, 0xda, 0x82, 0x22, 0x81, 0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 
  0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 
  0x02, 0x83, 0x2f, 0xec, 0xb0, 0xb1, 0x9f, 0xbc, 0xdb, 0xcf, 0x2a, 0x8e, 0x3d, 0xda, 0xfa, 0x84, 
  0x05, 0x01, 0xd3, 0xba, 0x89, 0x4f, 0xa8, 0xcf, 0x4d, 0xe8, 0x3c, 0xaa, 0x24, 0x10, 0x95, 0x94, 
  0xdd, 0x9a, 0x25, 0x21, 0x7c, 0xa3, 0xdd, 0x2f, 0xe2, 0xc7, 0x2f, 0x28, 0xe9, 0xb7, 0x36, 0x4f, 
  0xa7, 0x2e, 0xfd, 0x45, 0x07, 0x95, 0xc3, 0x80, 0x94, 0x73, 0x26, 0xec, 0xd2, 0x8f, 0x2a, 0xcf, 
  0x28, 0x69, 0xcc, 0xe4, 0x2c, 0x00, 0x3a, 0x63, 0x74, 0xe5, 0x5e, 0x40, 0x60, 0xef, 0xb5, 0x07, 
  0xe5, 0xd8, 0x90, 0x90, 0xda, 0xd6, 0xdd, 0xd1, 0xb5, 0xa9, 0x29, 0x51, 0x4a, 0x7d, 0xd2, 0xc1, 
  0x51, 0x0b, 0x09, 0x03, 0x71, 0x81, 0x94, 0xe5, 0x5e, 0x83, 0x1d, 0x76, 0xa0, 0xc4, 0xa0, 0x50, 
  0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 
  0xa0, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x07, 0xff, 0xd9
};

bool SlideLastFM::fetch(Render& render)
{
    TS_INFO("Fetching LastFM HTTP GET Request\n");

    #define TALOS_LASTFM_URL "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&limit=1&format=json&user=" TS_SECRET_LASTFM_USER "&api_key=" TS_SECRET_LASTFM_KEY

    HTTPClient http;
    if (!http.begin(TALOS_LASTFM_URL))
    {
        TS_ERROR("http Failed!\n");
        return false;
    }

    http.addHeader("Connection", "close");

    // Send HTTP GET request
    String payload = "{}";
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        TS_INFOF("HTTP Response code: %d\n", httpResponseCode);
        payload = http.getString();
        TS_INFOF("%s\n", payload.c_str());
    }
    else {
        TS_ERRORF("Error code: %d\n", httpResponseCode);
        return false;
    }
    
    // Free resources
    http.end();

    StaticJsonDocument<1024> json{};
    deserializeJson(json, payload);

    _artist = json["recenttracks"]["track"][0]["artist"]["#text"].as<String>();
    _album = json["recenttracks"]["track"][0]["album"]["#text"].as<String>();
    _track = json["recenttracks"]["track"][0]["name"].as<String>();
    String imageURL = json["recenttracks"]["track"][0]["image"][1]["#text"].as<String>();

    TS_INFOF("Latest Artist: %s\n", _artist.c_str());
    TS_INFOF("Latest Album: %s\n", _album.c_str());
    TS_INFOF("Latest Tracks: %s\n", _track.c_str());

    if (_track.length() > 16)
    {
        _track = _track.substring(0, 16);
        _track.concat("...");
    }

    String nowplaying = json["recenttracks"]["track"][0]["@attr"]["nowplaying"].as<String>();
    nowplaying.toLowerCase();

    _currentlyListening = nowplaying == "true" ? true : false;

    imageURL = "http" + imageURL.substring(5);

    if (!http.begin(imageURL))
    {
        TS_ERROR("http Failed!\n");
        return false;
    }

    http.addHeader("Connection", "close");

    // Send HTTP GET request
    httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        printf("HTTP Response code: %d\n", httpResponseCode);
        
        payload = http.getString();
        
        TS_ERRORF("%s\n", payload.c_str());
    }
    else {
        printf("Error code: %d", httpResponseCode);
        return false;
    }
    
    // Free resources
    http.end();


    TS_INFO("Creting a JPEG description!\n");
    JPEGDEC* dec = new JPEGDEC();

    TS_INFOF("Bitmap: %p\n", render.getBitmap());

    dec->setUserPointer(render.getBitmap());
    dec->setPixelType(ONE_BIT_DITHERED);

    TS_INFO("Opening JPEG flash\n");
    if (!dec->openRAM((uint8_t*)payload.c_str(), payload.length(), JPEGDraw))
    {
        TS_ERROR("Could not open JPEGDEC from memory!\n");
    }    

    dec->setUserPointer(render.getBitmap());

    TS_INFO("Decoding jpeg image!\n");
    if (!dec->decode(0, 0, 0))
    {
        TS_ERROR("Could not decode JPEGDEC image!\n");
    }

    TS_INFO("Decoded JPEG image!\n");

    return true;
}

void SlideLastFM::render(Render& render)
{
    Strings::Select playingSelect = _currentlyListening ? Strings::eSpotNowPlaying : Strings::eSpotWasPlaying; 
    
    const int16_t titleY = 480/2;
    const int16_t largeFont = 96;
    const int16_t smallFont = 56;

    render
        .setAlignment(RenderAlign::eBottomLeft)
        .setCursor(Vector2i{10, (int16_t)(titleY - largeFont/2)}) // 480/2 + 1 - trackSize/2
        .setFontSize(smallFont)
        .drawTextFromFlash(playingSelect)

        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .setFontSize(96)
        .setAlignment(RenderAlign::eMiddleLeft)
        .setCursor(Vector2i{10, titleY})
        .drawText(_track.c_str())

        .setAlignment(RenderAlign::eTopLeft)
        .setCursor(Vector2i{10, (int16_t)(titleY + largeFont/2)})
        .drawText(_artist.c_str()); 

}



} /* namespace ts */
