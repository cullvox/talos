#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "Config.h"
#include "WidgetDigitalClock.h"

namespace ts {

bool WidgetDigitalClock::fetch(WiFiClientSecure& client)
{
    time_t now = time(NULL);
    // now += GetConfig().utcOffset;

    tm date = {0};
    localtime_r(&now, &date);

    /* Construct our time string. */

    //if (GetConfig().is24HourClock)
        strftime(_time, sizeof(_time), "%H : %M", &date);
    //else 
    //    strftime(_time, sizeof(_time), "%I : %M %p", &date);

    return true;
}

void WidgetDigitalClock::render(Render& render)
{
    log_d("%s", _time);
    render.drawText(_time);
}

} /* namespace ts */