#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "SlideDigitalClock.h"

namespace ts {

bool SlideDigitalClock::fetch(Render&)
{

    /* Setup an http client for "worldtimeapi.org" to easily get the time. */
    HTTPClient http;
    if (!http.begin("http://worldtimeapi.org/api/ip"))
    {
        printf("http Failed!\n");
        return false;
    }

    http.addHeader("Connection", "close");

    /* Send the get request. */
    String payload = "{}";
    int responseCode = http.GET();
    
    if (responseCode > 0) { /* Successful response, get the content of the response. */
        log_i("HTTP Response code: %d\n", responseCode);
        payload = http.getString();
        log_i("%s\n", payload.c_str());
    }
    else { /* Unsuccessful response from the http client. */        
        log_e("Error code: %d", responseCode);
        return false;
    }
    
    /* Free up http client resources. */
    http.end();

    /* Parse the content json for specific data. */
    StaticJsonDocument<650> json{};
    deserializeJson(json, payload);

    /* Retrieve all the unixtime codes and add them together to 
        get the current and local time. */
    int64_t unixtime = json["unixtime"].as<int64_t>();
    int64_t raw_offset = json["raw_offset"].as<int64_t>();
    int64_t dst_offset = json["dst_offset"].as<int64_t>();
    time_t gmunixtime = (time_t)(unixtime + raw_offset + dst_offset);

    /* Convert the unixtime to a tm struct to get individual time values. */
    tm ourtime{};
    gmtime_r(&gmunixtime, &ourtime);

    /* Construct our time string. */
    snprintf(_time, sizeof(_time)-1, "%d : %.02d", ourtime.tm_hour, ourtime.tm_min);

    return true;
}

void SlideDigitalClock::render(Render& render)
{
    const Vector2i pos{800/2, 480/2};

    render
        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .setFontSize(220)
        .setAlignment(RenderAlign::eBottomCenter)
        .setCursor(pos)
        .drawTextFormat("%s", _time);
}

} /* namespace ts */