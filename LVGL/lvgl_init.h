/*
 * lvgl_init.h
 *
 *  Created on: Oct 13, 2023
 *      Author: lefucjusz
 */

#pragma once

#include "ili9341.h"

/* Config */
#define LVGL_DRAWING_BUFFER_SIZE ((ILI9341_WIDTH * ILI9341_HEIGHT) / 10) // LVGL docs recommend drawing buffer size ~10% screen size

/* Functions */
void lvgl_init(void);
