/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "lwip/api.h"
#include "bitmap.h"
#include "iot_lcd.h"
#include "iot_wifi_conn.h"
#include "app_camera.h"

#if CONFIG_USE_LCD
CEspLcd* tft = NULL;

typedef struct {
    uint8_t frame_num;
}camera_evt_t;

QueueHandle_t camera_queue = NULL;
static const char* TAG = "ESP-CAM-LCD";

void queue_send(uint8_t frame_num)
{
    camera_evt_t camera_event;
    camera_event.frame_num = frame_num;
    xQueueSend(camera_queue, &camera_event, portMAX_DELAY);
}

uint8_t queue_receive()
{
    camera_evt_t camera_event;
    xQueueReceive(camera_queue, &camera_event, portMAX_DELAY);
    return camera_event.frame_num;
}

uint8_t queue_available()
{
    return uxQueueSpacesAvailable(camera_queue);
}

void app_lcd_refresh(void *pvParameters)
{
    uint16_t * frame_pointer = (uint16_t *)pvParameters;
    tft->drawBitmap(0, 0, (uint16_t *)frame_pointer, 320, 240, false);
}

void lcd_init_wifi(void)
{
    tft->drawString("Status: Wifi Connecting ...", 5, 58);
}

void lcd_camera_init_complete(void)
{
    tft->drawString("Status: Camera initialize complete.", 5, 44);
}

void lcd_wifi_connect_complete(void)
{
    tft->drawString("Status: Wifi connect complete.", 5, 72);
}

void lcd_http_info(ip4_addr_t s_ip_addr)
{
    char ipadd[50];
    sprintf(ipadd, "open http://" IPSTR "/bmp for bitmap image", IP2STR(&s_ip_addr));
    tft->drawString(ipadd, 5, 86);
    sprintf(ipadd, "open http://" IPSTR "/get for img", IP2STR(&s_ip_addr));
    tft->drawString(ipadd, 5, 100);
}

void app_lcd_init()
{
    lcd_conf_t lcd_pins = {
        .lcd_model = LCD_MOD_ST7789,  //LCD_MOD_ILI9341,//LCD_MOD_ST7789,
        .pin_num_miso = CONFIG_HW_LCD_MISO_GPIO,
        .pin_num_mosi = CONFIG_HW_LCD_MOSI_GPIO,
        .pin_num_clk = CONFIG_HW_LCD_CLK_GPIO,
        .pin_num_cs = CONFIG_HW_LCD_CS_GPIO,
        .pin_num_dc = CONFIG_HW_LCD_DC_GPIO,
        .pin_num_rst = CONFIG_HW_LCD_RESET_GPIO,
        .pin_num_bckl = CONFIG_HW_LCD_BL_GPIO,
        .clk_freq = 10 * 1000 * 1000,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .spi_host = HSPI_HOST,};

    /*Initialize SPI Handler*/
    if (tft == NULL) {
        tft = new CEspLcd(&lcd_pins);
//        camera_queue = xQueueCreate(CAMERA_CACHE_NUM - 2, sizeof(camera_evt_t));
    }

    /*screen initialize*/
    tft->invertDisplay(false);
    tft->setRotation(1);
    tft->fillScreen(COLOR_GREEN);
    tft->drawBitmap(0, 0, esp_logo, 137, 26);
    tft->drawString("Status: Initialize touch ...", 5, 30);
}
#endif
