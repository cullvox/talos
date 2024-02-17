#include "Sandbox.h"

namespace ts {

PageSandbox::PageSandbox(SpotifyESP& spotify)
    : widSpotify(spotify)
{
}

void PageSandbox::render(Render& render)
{

    log_d("Rendering sandbox.");
    widSpotify.render(render);

    Vector2i clockPos{20, 20};
    render
        .setCursor(clockPos)
        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .setFontSize(60)
        .setAlignment(RenderAlign::eTopLeft);

    widClock.render(render);


    log_d("Rendered sandbox.");
}

bool PageSandbox::fetch(WiFiClientSecure& client)
{
    log_d("Fetching sandbox.");
    widSpotify.fetch(client);
    widClock.fetch(client);
    log_d("Fetched sandbox.");

    return true;
}

} /* namespace ts */