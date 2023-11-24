
### What is TALOS?
TALOS a.k.a. Time and Ambient Locale Output System is a program written to control ePaper displays and presenting useful information to end viewers.

The current slides TALOS can display include:
 - Last.fm
 - Digital Clock
 - (WIP) Advice Slips
 - (WIP) Weather

 → and more soon...

 It uses Platform IO to support embedded hardware and popular middle grade hobby boards fitted with the likes of ESP32's and other lesser known chip-sets. Some tested boards are the Adafruit HUZZAH32 (including V2). Further optimizations for other boards which are less capable are being worked on.

 ### Why Should I Build One?
 *The reason I created this project was because I constantly used my Alexa Show for the time and other info, but it simply wasn't useful enough. I know that I can't truly control it's listening habits and it's near constant advertising which had to be manual disabled (and would sometimes re-enable by itself).*

 An ePaper display powered by TALOS is a useful and its cohesive environment for presenting useful information to yourself or other without compromise to security and eases your peace of mind.
 
 TALOS doesn't gather any data of any kind except for the configuration that you set. Replace the other technology which is designed to intrude on your mental space with advertising and news stories that are irrelevant to the task at hand.

 ### How Do I Build One?

 There is documentation on how to build one in the [docs/builds](docs/builds/) folder. The current build guide you should use is [TALOSv1.md](docs/builds/TALOSv1.md). 

 ### Board Requirements

 - Networking: Wi-Fi Required
 - Bluetooth: optional
 - Memory: at least 320-512kb
 - SPI: at least 3 pin (CO, CLK, and CS) depending on display used
 - GPIO Pins: reasonable amount for display

I wish I could support the Raspberry Pi Pico, but it might not have enough memory for this type of project, especially with FreeType being used. Along with the Pico becoming exponentially harder to debug because it's error reporting can be hard to understand.

 ### Supported Boards

 I can only support boards which I have, if you want to add your own board add support for it, and add its environment in [platformio.ini](platformio.ini).
 - Adafruit Feather HUZZAH32
 - Adafruit Feather HUZZAH32 V2

 ### Supported ePaper Displays
 
 Once again I can only support ePapers that I have, add your own display if you want. A better interface for abstract displays will be created soon.
 
 - [Waveshare (SKU: 13504) 800×480, 7.5inch E-Ink display HAT for Raspberry Pi, SPI interface](https://www.waveshare.com/7.5inch-e-Paper-HAT.htm)
