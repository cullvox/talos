#include <stdlib.h>

#include <Arduino.h>

#include "Print.h"
#include "Pins.h"
#include "Display.h"

namespace ts {

unsigned char Voltage_Frame_7IN5_V2[]={
	0x6, 0x3F, 0x3F, 0x11, 0x24, 0x7, 0x17,
};

unsigned char LUT_VCOM_7IN5_V2[]={	
	0x0,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x0,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};						

unsigned char LUT_WW_7IN5_V2[]={	
	0x10,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x20,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};

unsigned char LUT_BW_7IN5_V2[]={	
	0x10,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x20,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};

unsigned char LUT_WB_7IN5_V2[]={	
	0x80,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x40,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};

unsigned char LUT_BB_7IN5_V2[]={	
	0x80,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x40,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};


Display::Display()
    : _csPin(-1)
    , _rstPin(-1)
    , _dcPin(-1)
    , _busyPin(-1)
    , _pwrPin(-1)
    , _pSpi(nullptr)
    , width(800)
    , height(480)
{
}

Extent2i Display::extent()
{
    return Extent2i{width, height};
}

bool Display::begin(uint16_t csPin, uint16_t rstPin, uint16_t dcPin, uint16_t busyPin, uint16_t pwrPin, SPIClass& spi)
{
    _csPin = csPin;
    _rstPin = rstPin;
    _dcPin = dcPin;
    _busyPin = busyPin;
    _pwrPin = pwrPin;
    _pSpi = &spi;

    pinMode(_csPin, OUTPUT);
    pinMode(_rstPin, OUTPUT);
    pinMode(_dcPin, OUTPUT);
    pinMode(_busyPin, INPUT); 
    pinMode(_pwrPin, OUTPUT);
    digitalWrite(_pwrPin, 1);

    _pSpi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

    reset();

    // sendCommand(0x01); 
    // sendData(0x07);
    // sendData(0x07);
    // sendData(0x3f);
    // sendData(0x3f);

    // sendCommand(0x04);
    // delayMs(100);
    // waitForIdle();
    
    // sendCommand(0X00);			//PANNEL SETTING
    // sendData(0x1F);   //KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f

    // sendCommand(0x61);        	//tres
    // sendData(0x03);		//source 800
    // sendData(0x20);
    // sendData(0x01);		//gate 480
    // sendData(0xE0);

    // sendCommand(0X15);
    // sendData(0x00);

    // sendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
    // sendData(0x10);
    // sendData(0x07);

    // sendCommand(0X60);			//TCON SETTING
    // sendData(0x22);

	sendCommand(0x01);  // power setting
	sendData(0x17);  // 1-0=11: internal power
	sendData(*(Voltage_Frame_7IN5_V2+6));  // VGH&VGL
	sendData(*(Voltage_Frame_7IN5_V2+1));  // VSH
	sendData(*(Voltage_Frame_7IN5_V2+2));  //  VSL
	sendData(*(Voltage_Frame_7IN5_V2+3));  //  VSHR
	
	sendCommand(0x82);  // VCOM DC Setting
	sendData(*(Voltage_Frame_7IN5_V2+4));  // VCOM

	sendCommand(0x06);  // Booster Setting
	sendData(0x27);
	sendData(0x27);
	sendData(0x2F);
	sendData(0x17);
	
	sendCommand(0x30);   // OSC Setting
	sendData(*(Voltage_Frame_7IN5_V2+0));  // 2-0=100: N=4  ; 5-3=111: M=7  ;  3C=50Hz     3A=100HZ

    sendCommand(0x04); //POWER ON
    delayMs(100);
    waitForIdle();

    sendCommand(0X00);			//PANNEL SETTING
    sendData(0x3F);   //KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f

    sendCommand(0x61);        	//tres
    sendData(0x03);		//source 800
    sendData(0x20);
    sendData(0x01);		//gate 480
    sendData(0xE0);

    sendCommand(0X15);
    sendData(0x00);

    sendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
    sendData(0x10);
    sendData(0x00);

    sendCommand(0X60);			//TCON SETTING
    sendData(0x22);

    sendCommand(0x65);  // Resolution setting
    sendData(0x00);
    sendData(0x00);//800*480
    sendData(0x00);
    sendData(0x00);

    setLutByHost(LUT_VCOM_7IN5_V2, LUT_WW_7IN5_V2, LUT_BW_7IN5_V2, LUT_WB_7IN5_V2, LUT_BB_7IN5_V2);

    return true;
}

void Display::delayMs(unsigned int delaytime) {
    delay(delaytime);
}

void Display::spiTransfer(unsigned char data) {
    digitalWrite(_csPin, LOW);
    _pSpi->transfer(data);
    digitalWrite(_csPin, HIGH);
}


/**
 *  @brief: basic function for sending commands
 */
void Display::sendCommand(unsigned char command) {
    digitalWrite(_dcPin, LOW);
    spiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Display::sendData(unsigned char data) {
    digitalWrite(_dcPin, HIGH);
    spiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Display::waitForIdle(void) {
    unsigned char busy;
    TS_INFO("e-Paper Busy\n");
    do {
        sendCommand(0x71);
        busy = digitalRead(_busyPin);
    } while (busy == 0);
    TS_INFO("e-Paper Busy Release\n");
    delayMs(20);
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Display::Sleep();
 */
void Display::reset(void)
{
    digitalWrite(_rstPin, HIGH);
    delayMs(20); 
    digitalWrite(_rstPin, LOW);                //module reset    
    delayMs(4);
    digitalWrite(_rstPin, HIGH);
    delayMs(20);    
}

void Display::present(const uint8_t* buffer) 
{    
    sendCommand(0x13);
    Extent2i e = extent();
    for (unsigned long j = 0; j < height; j++) {
        for (unsigned long i = 0; i < width/8; i++) {
            sendData(~buffer[i + j * width/8]);
        }
    }

    sendCommand(0x12);
    delayMs(100);
    waitForIdle();
}

void Display::setLutByHost(unsigned char* lut_vcom,  unsigned char* lut_ww, unsigned char* lut_bw, unsigned char* lut_wb, unsigned char* lut_bb)
{
	unsigned char count;

	sendCommand(0x20); //VCOM	
	for(count=0; count<42; count++)
		sendData(lut_vcom[count]);

	sendCommand(0x21); //LUTBW
	for(count=0; count<42; count++)
		sendData(lut_ww[count]);

	sendCommand(0x22); //LUTBW
	for(count=0; count<42; count++)
		sendData(lut_bw[count]);

	sendCommand(0x23); //LUTWB
	for(count=0; count<42; count++)
		sendData(lut_wb[count]);

	sendCommand(0x24); //LUTBB
	for(count=0; count<42; count++)
		sendData(lut_bb[count]);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5. 
 *          You can use EPD_Reset() to awaken
 */
void Display::sleep() 
{
    sendCommand(0X02);
    waitForIdle();
    sendCommand(0X07);
    sendData(0xA5);
}

void Display::clear() 
{
    sendCommand(0x13);
    for (unsigned long i = 0; i < height * width; i++)	{
        sendData(0x00);
    }
    sendCommand(0x12);
    delayMs(100);
    waitForIdle();
}

} /* namespace ts */
