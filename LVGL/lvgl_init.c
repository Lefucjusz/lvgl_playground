/*
 * lvgl_init.c
 *
 *  Created on: Oct 13, 2023
 *      Author: lefucjusz
 */

#include "lvgl_init.h"
#include "lvgl.h"
#include "xpt2046.h"

#include <stdint.h>

struct lvgl_ctx_t
{
	lv_disp_draw_buf_t disp_buf;
	lv_color_t draw_buf_1[LVGL_DRAWING_BUFFER_SIZE];
	lv_color_t draw_buf_2[LVGL_DRAWING_BUFFER_SIZE];
	lv_disp_drv_t disp_drv;
	lv_indev_drv_t indev_drv;
};

static struct lvgl_ctx_t lvgl_ctx;

static void lvgl_on_dma_tx_complete(void)
{
	ili9341_unselect();
	lv_disp_flush_ready(&lvgl_ctx.disp_drv);
}

static void lvgl_on_display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *px_map)
{
	UNUSED(disp_drv);

	const size_t dx = (area->x2 - area->x1) + 1;
	const size_t dy = (area->y2 - area->y1) + 1;
	const size_t data_size = dx * dy * sizeof(*px_map);

	ili9341_select();
	ili9341_set_window(area->x1, area->y1, area->x2, area->y2);
	ili9341_write_data_dma(px_map, data_size);
}

static void lvgl_on_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
	uint16_t x;
	uint16_t y;

	xpt2046_select();

	xpt2046_get_coords(&x, &y);
	data->point.x = x;
	data->point.y = y;
	data->state = xpt2046_touch_state();

	xpt2046_unselect();
}

static void lvgl_display_init(void)
{
	ili9341_unselect();
	ili9341_init();
	ili9341_set_dma_tx_complete_callback(lvgl_on_dma_tx_complete);
	lv_init();

	/* Initialize display buffer */
	lv_disp_draw_buf_init(&lvgl_ctx.disp_buf, lvgl_ctx.draw_buf_1, lvgl_ctx.draw_buf_2, LVGL_DRAWING_BUFFER_SIZE);

	/* Create display driver */
	lv_disp_drv_init(&lvgl_ctx.disp_drv);
	lvgl_ctx.disp_drv.draw_buf = &lvgl_ctx.disp_buf;
	lvgl_ctx.disp_drv.flush_cb = lvgl_on_display_flush;
	lvgl_ctx.disp_drv.hor_res = ILI9341_WIDTH;
	lvgl_ctx.disp_drv.ver_res = ILI9341_HEIGHT;

	/* Register display driver */
	lv_disp_drv_register(&lvgl_ctx.disp_drv);
}

static void lvgl_touch_init(void)
{
	xpt2046_init();

	/* Create input device driver */
	lv_indev_drv_init(&lvgl_ctx.indev_drv);
	lvgl_ctx.indev_drv.type = LV_INDEV_TYPE_POINTER; // Touchpad
	lvgl_ctx.indev_drv.read_cb = lvgl_on_input_read;

	/* Register input device driver */
	lv_indev_drv_register(&lvgl_ctx.indev_drv);
}

void lvgl_init(void)
{
	lvgl_display_init();
	lvgl_touch_init();
}
