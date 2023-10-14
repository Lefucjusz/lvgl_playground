/*
 * xpt2046.h
 *
 *  Created on: Oct 14, 2023
 *      Author: lefucjusz
 */

#pragma once

#include "ili9341.h"

#include <stdbool.h>

/* Typedefs */
enum xpt2046_touch_state_t
{
	XPT2046_RELEASED = 0,
	XPT2046_PRESSED
};

/* Constants */
#define XPT2046_READ_X_AXIS_CMD 0b11010000
#define XPT2046_READ_Y_AXIS_CMD 0b10010000
#define XPT2046_READ_Z_AXIS_CMD 0b10110000

/* Config */
#define XPT2046_CS_GPIO GPIOB
#define XPT2046_CS_PIN GPIO_PIN_11
#define XPT2046_IRQ_GPIO GPIOB
#define XPT2046_IRQ_PIN GPIO_PIN_12

#define XPT2046_SPI_PORT hspi2
#define XPT2046_SPI_TIMEOUT_MS 100

#define XPT2046_AVERAGE_PTS 10

#define XPT2046_RES_X ILI9341_WIDTH
#define XPT2046_RES_Y ILI9341_HEIGHT

#define XPT2046_ROTATED 1

/* Calibration constants - one day I'll add proper calibration algorithm, but for now... */
#define XPT2046_CAL_MIN_X 300
#define XPT2046_CAL_MAX_X 3800
#define XPT2046_CAL_MIN_Y 450
#define XPT2046_CAL_MAX_Y 3900

/* Variables */
extern SPI_HandleTypeDef XPT2046_SPI_PORT;

/* Functions */
void xpt2046_select(void);
void xpt2046_unselect(void);

void xpt2046_init(void);

enum xpt2046_touch_state_t xpt2046_touch_state(void);
void xpt2046_get_coords(uint16_t *x, uint16_t *y);
