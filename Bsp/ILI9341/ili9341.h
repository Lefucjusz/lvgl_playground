/*
 * ili9341.h
 *
 *  Created on: Oct 13, 2023
 *      Author: lefucjusz
 */

#pragma once

#include "stm32f4xx_hal.h"

/* Constants */
#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

/* Config */
#define ILI9341_RESET_GPIO GPIOC
#define ILI9341_RESET_PIN GPIO_PIN_5
#define ILI9341_CS_GPIO GPIOB
#define ILI9341_CS_PIN GPIO_PIN_8
#define ILI9341_DC_GPIO GPIOC
#define ILI9341_DC_PIN GPIO_PIN_4

#define ILI9341_SPI_PORT hspi1

#define ILI9341_SPI_TIMEOUT_MS 100
#define ILI9341_SPI_TRANSFER_SIZE 32768 // Has to be below 64KiB

/* Default orientation */
//#define ILI9341_WIDTH  240
//#define ILI9341_HEIGHT 320
//#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)

/* Rotate right */
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)

/* Rotate left */
//#define ILI9341_WIDTH  320
//#define ILI9341_HEIGHT 240
//#define ILI9341_ROTATION (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)

/* Upside down */
//#define ILI9341_WIDTH  240
//#define ILI9341_HEIGHT 320
//#define ILI9341_ROTATION (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR)

/* Variables */
extern SPI_HandleTypeDef ILI9341_SPI_PORT;

/* Functions */
void ili9341_select(void);
void ili9341_unselect(void);

void ili9341_init(void);

void ili9341_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ili9341_write_data(const void *data, size_t size);
void ili9341_write_data_dma(const void *data, size_t size);

void ili9341_set_dma_tx_complete_callback(void (*on_dma_tx_complete)(void));
