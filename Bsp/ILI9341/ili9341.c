/*
 * ili9341.c
 *
 *  Created on: Oct 13, 2023
 *      Author: lefucjusz
 */

#include "ili9341.h"

#include <stdint.h>

static void (*dma_tx_complete_callback)(void) = NULL;

static void ili9341_reset(void)
{
    HAL_GPIO_WritePin(ILI9341_RESET_GPIO, ILI9341_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ILI9341_RESET_GPIO, ILI9341_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(5);
}

static void ili9341_write_command(uint8_t cmd)
{
	/* Set command mode */
    HAL_GPIO_WritePin(ILI9341_DC_GPIO, ILI9341_DC_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ILI9341_SPI_PORT, &cmd, sizeof(cmd), ILI9341_SPI_TIMEOUT_MS);
}

void ili9341_select(void)
{
	HAL_GPIO_WritePin(ILI9341_CS_GPIO, ILI9341_CS_PIN, GPIO_PIN_RESET);
}

void ili9341_unselect(void)
{
	HAL_GPIO_WritePin(ILI9341_CS_GPIO, ILI9341_CS_PIN, GPIO_PIN_SET);
}

void ili9341_init(void) // TODO check all the commands
{
	ili9341_select();
	ili9341_reset();

	/* SOFTWARE RESET */
	ili9341_write_command(0x01);
	HAL_Delay(1000);

	/* POWER CONTROL A */
	ili9341_write_command(0xCB);
	{
		const uint8_t data[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
		ili9341_write_data(data, sizeof(data));
	}

	/* POWER CONTROL B */
	ili9341_write_command(0xCF);
	{
		const uint8_t data[] = {0x00, 0xC1, 0x30};
		ili9341_write_data(data, sizeof(data));
	}

	/* DRIVER TIMING CONTROL A */
	ili9341_write_command(0xE8);
	{
		const uint8_t data[] = {0x85, 0x00, 0x78};
		ili9341_write_data(data, sizeof(data));
	}

	/* DRIVER TIMING CONTROL B */
	ili9341_write_command(0xEA);
	{
		const uint8_t data[] = {0x00, 0x00};
		ili9341_write_data(data, sizeof(data));
	}

	/* POWER ON SEQUENCE CONTROL */
	ili9341_write_command(0xED);
	{
		const uint8_t data[] = {0x64, 0x03, 0x12, 0x81};
		ili9341_write_data(data, sizeof(data));
	}

	/* PUMP RATIO CONTROL */
	ili9341_write_command(0xF7);
	{
		const uint8_t data[] = {0x20};
		ili9341_write_data(data, sizeof(data));
	}

	/* POWER CONTROL, VRH[5:0] */
	ili9341_write_command(0xC0);
	{
		const uint8_t data[] = {0x23};
		ili9341_write_data(data, sizeof(data));
	}

	/* POWER CONTROL, SAP[2:0]; BT[3:0] */
	ili9341_write_command(0xC1);
	{
		const uint8_t data[] = {0x10};
		ili9341_write_data(data, sizeof(data));
	}

	/* VCM CONTROL */
	ili9341_write_command(0xC5);
	{
		const uint8_t data[] = {0x3E, 0x28};
		ili9341_write_data(data, sizeof(data));
	}

	/* VCM CONTROL 2 */
	ili9341_write_command(0xC7);
	{
		const uint8_t data[] = {0x86};
		ili9341_write_data(data, sizeof(data));
	}

	/* MEMORY ACCESS CONTROL */
	ili9341_write_command(0x36);
	{
		const uint8_t data[] = {0x48};
		ili9341_write_data(data, sizeof(data));
	}

	/* PIXEL FORMAT */
	ili9341_write_command(0x3A);
	{
		const uint8_t data[] = {0x55};
		ili9341_write_data(data, sizeof(data));
	}

	/* FRAME RATIO CONTROL, STANDARD RGB COLOR */
	ili9341_write_command(0xB1);
	{
		const uint8_t data[] = {0x00, 0x18};
		ili9341_write_data(data, sizeof(data));
	}

	/* DISPLAY FUNCTION CONTROL */
	ili9341_write_command(0xB6);
	{
		const uint8_t data[] = {0x08, 0x82, 0x27};
		ili9341_write_data(data, sizeof(data));
	}

	/* 3GAMMA FUNCTION DISABLE */
	ili9341_write_command(0xF2);
	{
		const uint8_t data[] = {0x00};
		ili9341_write_data(data, sizeof(data));
	}

	/* GAMMA CURVE SELECTED */
	ili9341_write_command(0x26);
	{
		const uint8_t data[] = {0x01};
		ili9341_write_data(data, sizeof(data));
	}

	/* POSITIVE GAMMA CORRECTION */
	ili9341_write_command(0xE0);
	{
		const uint8_t data[] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
								0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
		ili9341_write_data(data, sizeof(data));
	}

	/* NEGATIVE GAMMA CORRECTION */
	ili9341_write_command(0xE1);
	{
		const uint8_t data[] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
						   	    0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
		ili9341_write_data(data, sizeof(data));
	}

	/* EXIT SLEEP */
	ili9341_write_command(0x11);
	HAL_Delay(120);

	/* TURN ON DISPLAY */
	ili9341_write_command(0x29);

	// MADCTL
	ili9341_write_command(0x36);
	{
		const uint8_t data[] = {ILI9341_ROTATION};
		ili9341_write_data(data, sizeof(data));
	}

	ili9341_unselect();
}


void ili9341_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	uint8_t buffer[4];

	/* Column address set */
	ili9341_write_command(0x2A); // CASET

	buffer[0] = (x0 >> 8) & 0xFF;
	buffer[1] = x0 & 0xFF;
	buffer[2] = (x1 >> 8) & 0xFF;
	buffer[3] = x1 & 0xFF;
	ili9341_write_data(buffer, sizeof(buffer));

	/* Row address set */
	ili9341_write_command(0x2B); // RASET

	buffer[0] = (y0 >> 8) & 0xFF;
	buffer[1] = y0 & 0xFF;
	buffer[2] = (y1 >> 8) & 0xFF;
	buffer[3] = y1 & 0xFF;
	ili9341_write_data(buffer, sizeof(buffer));

	/* Write to RAM */
	ili9341_write_command(0x2C); // RAMWR
}

void ili9341_write_data(const void *data, size_t size)
{
	/* Set data mode */
	HAL_GPIO_WritePin(ILI9341_DC_GPIO, ILI9341_DC_PIN, GPIO_PIN_SET);

	/* Send data in chunks, as HAL cannot send more than 64KiB at once */
	while (size > 0) {
		const uint16_t chunk_size = (size > ILI9341_SPI_TRANSFER_SIZE) ? ILI9341_SPI_TRANSFER_SIZE : size;
		HAL_SPI_Transmit(&ILI9341_SPI_PORT, (uint8_t *)data, chunk_size, ILI9341_SPI_TIMEOUT_MS);
		data += chunk_size;
		size -= chunk_size;
	}
}

void ili9341_write_data_dma(const void *data, size_t size)
{
	/* Set data mode */
	HAL_GPIO_WritePin(ILI9341_DC_GPIO, ILI9341_DC_PIN, GPIO_PIN_SET);

	/* Send data in chunks, as HAL cannot send more than 64KiB at once */
	while (size > 0) {
		const uint16_t chunk_size = (size > ILI9341_SPI_TRANSFER_SIZE) ? ILI9341_SPI_TRANSFER_SIZE : size;
		HAL_SPI_Transmit_DMA(&ILI9341_SPI_PORT, (uint8_t *)data, chunk_size);
		data += chunk_size;
		size -= chunk_size;
	}
}

void ili9341_set_dma_tx_complete_callback(void (*on_dma_tx_complete)(void))
{
	dma_tx_complete_callback = on_dma_tx_complete;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (dma_tx_complete_callback) {
		dma_tx_complete_callback();
	}
}
