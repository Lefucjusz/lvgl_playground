/*
 * xpt2046.c
 *
 *  Created on: Oct 14, 2023
 *      Author: lefucjusz
 */

#include "xpt2046.h"
#include "utils.h"

#include <string.h>

struct xpt2046_ctx_t
{
	uint16_t last_x;
	uint16_t last_y;
};

static struct xpt2046_ctx_t xpt2046_ctx;

static uint16_t xpt2046_get_axis_value(uint8_t axis_cmd)
{
	uint16_t value = 0;
	HAL_SPI_Transmit(&XPT2046_SPI_PORT, &axis_cmd, sizeof(axis_cmd), XPT2046_SPI_TIMEOUT_MS);
	HAL_SPI_Receive(&XPT2046_SPI_PORT, (uint8_t *)&value, sizeof(value), XPT2046_SPI_TIMEOUT_MS);
	return (bswap16(value) >> 3);
}

void xpt2046_select(void)
{
	HAL_GPIO_WritePin(XPT2046_CS_GPIO, XPT2046_CS_PIN, GPIO_PIN_RESET);
}

void xpt2046_unselect(void)
{
	HAL_GPIO_WritePin(XPT2046_CS_GPIO, XPT2046_CS_PIN, GPIO_PIN_SET);
}

void xpt2046_init(void)
{
	memset(&xpt2046_ctx, 0, sizeof(xpt2046_ctx));
	xpt2046_unselect();
}

enum xpt2046_touch_state_t xpt2046_touch_state(void)
{
	return (HAL_GPIO_ReadPin(XPT2046_IRQ_GPIO, XPT2046_IRQ_PIN) == GPIO_PIN_RESET) ? XPT2046_PRESSED : XPT2046_RELEASED;
}

void xpt2046_get_coords(uint16_t *x, uint16_t *y)
{
	uint16_t avg_x = 0;
	uint16_t avg_y = 0;

	/* Acquire and accumulate data */
	for (size_t i = 0; i < XPT2046_AVERAGE_PTS; ++i) {
		if (xpt2046_touch_state() == XPT2046_RELEASED) {
			*x = xpt2046_ctx.last_x;
			*y = xpt2046_ctx.last_y;
			return;
		}

		avg_x += xpt2046_get_axis_value(XPT2046_READ_X_AXIS_CMD);
		avg_y += xpt2046_get_axis_value(XPT2046_READ_Y_AXIS_CMD);
	}

	/* Compute average */
	avg_x /= XPT2046_AVERAGE_PTS;
	avg_y /= XPT2046_AVERAGE_PTS;

	/* Limit */
	avg_x = clamp(avg_x, XPT2046_CAL_MIN_X, XPT2046_CAL_MAX_X);
	avg_y = clamp(avg_y, XPT2046_CAL_MIN_Y, XPT2046_CAL_MAX_Y);

	/* Scale to resolution and set output */
#if XPT2046_ROTATED == 1
	*x = map(avg_y, XPT2046_CAL_MIN_Y, XPT2046_CAL_MAX_Y, 0, XPT2046_RES_X);
	*y = map(avg_x, XPT2046_CAL_MIN_X, XPT2046_CAL_MAX_X, XPT2046_RES_Y, 0);
#else
	*x = map(avg_x, XPT2046_CAL_MIN_X, XPT2046_CAL_MAX_X, 0, XPT2046_RES_X);
	*y = map(avg_y, XPT2046_CAL_MIN_Y, XPT2046_CAL_MAX_Y, 0, XPT2046_RES_Y);
#endif

	/* Update last value */
	xpt2046_ctx.last_x = *x;
	xpt2046_ctx.last_y = *y;
}
