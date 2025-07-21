#include "display.h"

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"

unsigned char voltage_frame_7in5_v2[] = {
    0x6, 0x3f, 0x3f, 0x11, 0x24, 0x7, 0x17,
};

unsigned char lut_vcom_7in5_v2[] = {
    0x0,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x0,	0xf,	0x1,	0xf,	0x1,	0x2,
    0x0,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

unsigned char lut_ww_7in5_v2[] = {
    0x10,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x84,	0xf,	0x1,	0xf,	0x1,	0x2,
    0x20,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

unsigned char lut_bw_7in5_v2[] = {
    0x10,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x84,	0xf,	0x1,	0xf,	0x1,	0x2,
    0x20,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

unsigned char lut_wb_7in5_v2[] = {
    0x80,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x84,	0xf,	0x1,	0xf,	0x1,	0x2,
    0x40,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

unsigned char lut_bb_7in5_v2[] = {
    0x80,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x84,	0xf,	0x1,	0xf,	0x1,	0x2,
    0x40,	0xf,	0xf,	0x0,	0x0,	0x1,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

#define TALOS_DISPLAY_WIDTH 800
#define TALOS_DISPLAY_HEIGHT 480

#define TALOS_PACKED_UINT32(lo, hi) (((uint32_t)(hi)) << 8) | ((uint16_t)(lo))
#define TALOS_PACKED_UINT32_VP(lo, hi) ((void*)(intptr_t)(TALOS_PACKED_UINT32(lo, hi)))
#define TALOS_PACKED_UINT32_LO(n) (uint16_t)(n)
#define TALOS_PACKED_UINT32_HI(n) (uint16_t)((n) >> 16)

#define TALOS_PACKED_PIN_LEVEL(pin, level) (TALOS_PACKED_UINT32_VP(pin, level))

#define TALOS_PACKED_PIN_PRE_POST(pin, pre_level, post_level) (((uint32_t)(post_level) << 16) | (((uint32_t)(pre_level)) << 8) | (uint32_t)pin)

#define TALOS_PACKED_PIN(x) (uint8_t)(x)
#define TALOS_PACKED_PRE_LEVEL(x) (uint8_t)(x >> 8)
#define TALOS_PACKED_POST_LEVEL(x) (uint8_t)(x >> 16)

void display_spi_pre_transmit_callback(spi_transaction_t* t)
{
    uint16_t pin = TALOS_PACKED_PIN((intptr_t)t->user);
    uint16_t level = TALOS_PACKED_PRE_LEVEL((intptr_t)t->user);
    gpio_set_level(pin, level);
}

void display_spi_post_transmit_callback(spi_transaction_t* t)
{
    uint16_t pin = TALOS_PACKED_PIN((intptr_t)t->user);
    uint16_t level = TALOS_PACKED_POST_LEVEL((intptr_t)t->user);
    // gpio_set_level(pin, level);
}

talos_res talos_display_init(talos_display* display)
{
    // Configure the display output pins.
    gpio_config_t io_config = {0};
    io_config.pin_bit_mask = ((1ULL << display->cs_pin) | (1ULL << display->dc_pin) | (1ULL << display->reset_pin) | (1ULL << display->power_pin));
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&io_config));

    // Configure the display input pin.
    io_config.pin_bit_mask = (1ULL << display->busy_pin);
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;

    ESP_ERROR_CHECK(gpio_config(&io_config));

    // For heavy transactions and data processing for uploading bitmaps.
    // We will use the DMA.

    spi_bus_config_t bus_config = {0};
    bus_config.miso_io_num = -1;
    bus_config.mosi_io_num = display->dc_pin;
    bus_config.sclk_io_num = -1;
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.max_transfer_sz = (TALOS_DISPLAY_WIDTH * TALOS_DISPLAY_HEIGHT) / 8;

    spi_device_interface_config_t device_config = {0};
    device_config.clock_speed_hz = 2000000;
    device_config.mode = 0;
    device_config.spics_io_num = display->cs_pin;
    device_config.queue_size = 8;
    device_config.pre_cb = display_spi_pre_transmit_callback;
    device_config.post_cb = display_spi_post_transmit_callback;

    ESP_ERROR_CHECK(spi_bus_initialize(display->host, &bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(display->host, &device_config, &display->handle));

    talos_display_reset(display);

    return true;
}

void display_transmit_command(talos_display* display, uint8_t cmd, bool keep_cs_active) 
{
    spi_transaction_t t = {0};
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)(intptr_t)TALOS_PACKED_PIN_PRE_POST(display->dc_pin, 0, -1);

    if (keep_cs_active)
    {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;
    }

    ESP_ERROR_CHECK(spi_device_polling_transmit(display->handle, &t));
}

void display_transmit_data(talos_display* display, const uint8_t* data, uint32_t length)
{
    spi_transaction_t t = {0};
    t.length = 8 * length;
    t.tx_buffer = data;
    t.user = (void*)(intptr_t)TALOS_PACKED_PIN_LEVEL(display->dc_pin, 1);

    ESP_ERROR_CHECK(spi_device_polling_transmit(display->handle, &t));
}

void talos_delay_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void display_wait_idle(talos_display* display)
{
    uint8_t busy;
    ESP_LOGI("display", "e-paper busy");

    do {
        display_transmit_command(display, 0x71, true);
        busy = gpio_get_level(display->busy_pin);
    } while (busy == 0);

    ESP_LOGI("display", "e-paper release");
    talos_delay_ms(20);
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Display::Sleep();
 */
void talos_display_reset(talos_display* display)
{
    gpio_set_level(display->reset_pin, 1);
    talos_delay_ms(20);

    gpio_set_level(display->reset_pin, 0); // module reset
    talos_delay_ms(4);

    gpio_set_level(display->reset_pin, 1);
    talos_delay_ms(20);

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

    display_transmit_command(display, 0x01, false);  // power setting
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

}

void Display::present(const uint8_t* buffer) 
{    
    sendCommand(0x13);
    Extent2i e = extent();
    for (unsigned long j = 0; j < extent().y; j++) {
        for (unsigned long i = 0; i < extent().x/8; i++) {
            sendData(~buffer[i + j * extent().x/8]);
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
void talos_display_sleep()
{
    sendCommand(0X02);
    waitForIdle();
    sendCommand(0X07);
    sendData(0xA5);
}

void talos_display_clear()
{
    sendCommand(0x13);
    for (unsigned long i = 0; i < extent().y * extent().x; i++)	{
        sendData(0x00);
    }
    sendCommand(0x12);
    delayMs(100);
    waitForIdle();
}
