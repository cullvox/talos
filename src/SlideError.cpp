#include "SlideError.h"

namespace ts {

bool SlideError::fetch(BitmapInterface* pBitmap)
{
    /* don't need to fetch. */
    return true;
}

template <typename T> T CLAMP(const T& value, const T& low, const T& high) 
{
    return value < low ? low : (value > high ? high : value); 
}

void SlideError::render(OpenFontRender& ofr)
{
    /* draw title */

    ofr.setAlignment(Align::TopLeft);
    ofr.setBackgroundColor(0xFF);
    ofr.setFontColor(0x00);
    
    ofr.setCursor(10, 10);
    ofr.setFontSize(145);

    char temp[16+1];
    memset(temp, 0, 16);
    Strings::copyTo(Strings::eError, temp, 16);

    ofr.printf("%s", temp);

    /* draw the large data */
    
    unsigned int topSize = ofr.calculateFitFontSize(700, 200, Layout::Horizontal, _primary);
    FT_BBox bounds = ofr.calculateBoundingBox(50, 480/2, topSize, Align::MiddleLeft, Layout::Horizontal, _primary);

    ofr.setAlignment(Align::TopLeft);
    ofr.setCursor(10, 480/2);
    ofr.setFontSize(topSize);
    ofr.printf("%s", _primary);

    /* draw the secondary smaller text*/
    unsigned int bottomSize = ofr.calculateFitFontSize(700, 100, Layout::Horizontal, _secondary);

    bottomSize = CLAMP(topSize * 0.5f, 45.f, (float)topSize); 

    ofr.setCursor(10, 480/2 + topSize);
    ofr.setFontSize(bottomSize);
    ofr.printf("%s", _secondary);

}

void SlideError::setPrimary(Strings::Select selection) 
{ 
    Strings::copyTo(selection, _primary, sizeof(_primary)-1); 
}

void SlideError::setSecondary(Strings::Select selection) 
{
    Strings::copyTo(selection, _secondary, sizeof(_secondary)-1); 
}

} /* namespace ts */