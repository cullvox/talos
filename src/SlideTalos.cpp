#include "SlideTalos.h"
#include "FlashStrings.h"

namespace ts {

bool SlideTalos::fetch(BitmapInterface* pBitmap)
{
    /* no need to fetch! */
    return true;    
}

void SlideTalos::render(OpenFontRender& ofr)
{
    char temp[64+1];
    memset(temp, 0, 65);

    ofr.setCursor(50, 130);
    ofr.setAlignment(Align::BottomLeft);
    ofr.setFontSize(200);
    ofr.setFontColor(0x00);
    ofr.setBackgroundColor(0xFF);

    Strings::copyTo(Strings::eTitle, temp, 64);
    ofr.printf("%s", temp);

    ofr.setCursor(50, 325);
    ofr.setFontSize(45);
    
    Strings::copyTo(Strings::eTitleFull, temp, 64);
    ofr.printf("%s", temp);

    ofr.setCursor(50, 370);
    ofr.setFontSize(50);

    Strings::copyTo(Strings::eCredit, temp, 64);
    ofr.printf("%s", temp);

    ofr.setCursor(790, 180);
    ofr.setAlignment(Align::BottomRight);
    ofr.setFontSize(45);

    Strings::copyTo(Strings::eVersion, temp, 64);
    ofr.printf("%s", temp);

    

}

} /* namespace ts */
