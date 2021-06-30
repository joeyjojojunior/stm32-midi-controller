#include "ssd1306.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
static SSD1306_t SSD1306;

//
//  Send a byte to the command register
//
static uint8_t ssd1306_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t command) {
	return HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}

//
//  Initialize the oled screen
//
uint8_t ssd1306_Init(I2C_HandleTypeDef *hi2c) {
	// Wait for the screen to boot
	HAL_Delay(100);
	int status = 0;

	// Init LCD
	status += ssd1306_WriteCommand(hi2c, 0xAE);   // Display off
	status += ssd1306_WriteCommand(hi2c, 0x20);   // Set Memory Addressing Mode
	status += ssd1306_WriteCommand(hi2c, 0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	status += ssd1306_WriteCommand(hi2c, 0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
	status += ssd1306_WriteCommand(hi2c, 0xC8);   // Set COM Output Scan Direction
	status += ssd1306_WriteCommand(hi2c, 0x00);   // Set low column address
	status += ssd1306_WriteCommand(hi2c, 0x10);   // Set high column address
	status += ssd1306_WriteCommand(hi2c, 0x40);   // Set start line address
	status += ssd1306_WriteCommand(hi2c, 0x81);   // set contrast control register
	status += ssd1306_WriteCommand(hi2c, 0x40);
	status += ssd1306_WriteCommand(hi2c, 0xA1);   // Set segment re-map 0 to 127
	status += ssd1306_WriteCommand(hi2c, 0xA6);   // Set normal display

	status += ssd1306_WriteCommand(hi2c, 0xA8);   // Set multiplex ratio(1 to 64)
	status += ssd1306_WriteCommand(hi2c, SSD1306_HEIGHT - 1);

	status += ssd1306_WriteCommand(hi2c, 0xA4);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	status += ssd1306_WriteCommand(hi2c, 0xD3);   // Set display offset
	status += ssd1306_WriteCommand(hi2c, 0x00);   // No offset
	status += ssd1306_WriteCommand(hi2c, 0xD5);   // Set display clock divide ratio/oscillator frequency
	status += ssd1306_WriteCommand(hi2c, 0xF0);   // Set divide ratio
	status += ssd1306_WriteCommand(hi2c, 0xD9);   // Set pre-charge period
	status += ssd1306_WriteCommand(hi2c, 0x22);

	status += ssd1306_WriteCommand(hi2c, 0xDA);   // Set com pins hardware configuration
#ifdef SSD1306_COM_LR_REMAP
    status += ssd1306_WriteCommand(hi2c, 0x32);   // Enable COM left/right remap
#else
	status += ssd1306_WriteCommand(hi2c, 0x12);   // Do not use COM left/right remap
#endif // SSD1306_COM_LR_REMAP

	status += ssd1306_WriteCommand(hi2c, 0xDB);   // Set vcomh
	status += ssd1306_WriteCommand(hi2c, 0x20);   // 0x20,0.77xVcc
	status += ssd1306_WriteCommand(hi2c, 0x8D);   // Set DC-DC enable
	status += ssd1306_WriteCommand(hi2c, 0x14);   //
	status += ssd1306_WriteCommand(hi2c, 0xAF);   // Turn on SSD1306 panel

	if (status != 0) {
		return 1;
	}

	// Clear screen
	ssd1306_Fill(Black);

	// Flush buffer to screen
	ssd1306_UpdateScreen(hi2c);

	// Set default values for screen object
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;

	SSD1306.Initialized = 1;

	return 0;
}

//
//  Fill the whole screen with the given color
//
void ssd1306_Fill(SSD1306_COLOR color) {
	// Fill screenbuffer with a constant value (color)
	uint32_t i;

	for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

//
//  Write the screenbuffer with changed to the screen
//
void ssd1306_UpdateScreen(I2C_HandleTypeDef *hi2c) {
	for (uint8_t i = 0; i < 8; i++) {
		ssd1306_WriteCommand(hi2c, 0xB0 + i);
		ssd1306_WriteCommand(hi2c, 0x00);
		ssd1306_WriteCommand(hi2c, 0x10);
		HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH, 100);
	}
}

//
// Write a knob's values to the screen
//
void ssd1306_WriteKnob(I2C_HandleTypeDef *hi2c, Knob *k) {
	char channel_string[3];
	char cc_string[4];
	char init_indicator_string[16] = "       @       ";
	char value_string[4];

	snprintf(channel_string, sizeof(channel_string)/sizeof(channel_string[0]), "%.2d", (int) k->channel + 1);
	snprintf(cc_string, sizeof(cc_string)/sizeof(cc_string[0]), "%.3d", (int) k->cc);
	snprintf(value_string, sizeof(value_string)/sizeof(value_string[0]), "%.3d", (int) KnobMap(k, k->value, k->max_range));

	// Clear buffer
	ssd1306_Fill(Black);

	// Draw top line
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(channel_string, NumFont_5x7, White);

	ssd1306_SetCursor(0, 8);
	ssd1306_WriteString(cc_string, NumFont_5x7, White);

	int8_t init_value_diff = KnobMap(k, k->init_value, 127) - KnobMap(k, k->value, 127);
	uint8_t init_percent_change = (abs(init_value_diff) / (127.0f)) * 100;

	if (init_value_diff == 0) {
		strcpy(init_indicator_string, "       @       ");
		k->isLocked = 0;
	} else {
		if (init_percent_change < 15) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, "      >        ");
			else
				strcpy(init_indicator_string, "        <      ");
		} else if (init_percent_change >= 15 && init_percent_change < 30) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, "     >>        ");
			else
				strcpy(init_indicator_string, "        <<     ");
		} else if (init_percent_change >= 30 && init_percent_change < 45) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, "    >>>        ");
			else
				strcpy(init_indicator_string, "        <<<    ");
		} else if (init_percent_change >= 45 && init_percent_change < 61) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, "   >>>>        ");
			else
				strcpy(init_indicator_string, "        <<<<   ");
		} else  if (init_percent_change >= 61 && init_percent_change < 77) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, "  >>>>>        ");
			else
				strcpy(init_indicator_string, "        <<<<<  ");
		} else  if (init_percent_change >= 77 && init_percent_change < 93) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, " >>>>>>        ");
			else
				strcpy(init_indicator_string, "        <<<<<< ");
		} else  if (init_percent_change >= 93) {
			if (init_value_diff > 0)
				strcpy(init_indicator_string, ">>>>>>>        ");
			else
				strcpy(init_indicator_string, "        <<<<<<<");
		}
	}

	for (int i = 0; i < sizeof(init_indicator_string)/sizeof(init_indicator_string[0]); i++) {

	}

	//ssd1306_SetCursor((SSD1306_WIDTH - sizeof(init_indicator_string)/sizeof(init_indicator_string[0]) * NumFont_5x7.FontWidth) / 2, 4);
	ssd1306_SetCursor((SSD1306_WIDTH - strlen(init_indicator_string) * NumFont_5x7.FontWidth) / 2, 4);
	//ssd1306_SetCursor((SSD1306_WIDTH - 16 * NumFont_5x7.FontWidth) / 2, 4);

	ssd1306_WriteString(init_indicator_string, NumFont_5x7, White);

	ssd1306_SetCursor(SSD1306_WIDTH - sizeof(value_string)/sizeof(value_string[0]) * NumFont_5x7.FontWidth - 1, 4);
	ssd1306_WriteString(value_string, NumFont_5x7, White);

	uint8_t len_label = 0;
	uint8_t x = 0;

	// Draw main label
	len_label = strlen(k->label);
	//x = ((MAX_LABEL_CHARS - len_label) / 2) * Font_10x18.FontWidth;
	x = (SSD1306_WIDTH - len_label * Font_10x18.FontWidth) / 2;
	//if (len_label % 2 != 0) x += 5;
	ssd1306_SetCursor(x, 16);
	ssd1306_WriteString(k->label, Font_10x18, White);

	// If the max number of values is restricted, we want to use
	// sub labels for each choice (e.g. osc. wave selection)
	if (k->max_values < 127) {
		len_label = strlen(k->sub_labels[k->value]);
		x = (SSD1306_WIDTH - len_label * Font_10x18.FontWidth) / 2;

		//x = ((MAX_LABEL_CHARS - len_label) / 2) * Font_10x18.FontWidth;
		if (len_label % 2 != 0) x += 5;
		ssd1306_SetCursor(x, 40);
		ssd1306_WriteString(k->sub_labels[k->value], Font_10x18, White);
	} else {
		// Full-range value - draw single sub-label
		len_label = strlen(k->sub_label);
		x = (SSD1306_WIDTH - len_label * Font_10x18.FontWidth) / 2;

		//x = ((MAX_LABEL_CHARS - len_label) / 2) * Font_10x18.FontWidth;
		if (len_label % 2 != 0) x += 5;
		ssd1306_SetCursor(x, 40);
		ssd1306_WriteString(k->sub_label, Font_10x18, White);
	}

	ssd1306_UpdateScreen(hi2c);
}

//
//  Draw one pixel in the screenbuffer
//  X => X Coordinate
//  Y => Y Coordinate
//  color => Pixel color
//
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		// Don't write outside the buffer
		return;
	}

	// Check if pixel should be inverted
	if (SSD1306.Inverted) {
		color = (SSD1306_COLOR) !color;
	}

	// Draw in the correct color
	if (color == White) {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

//
//  Draw 1 char to the screen buffer
//  ch      => Character to write
//  Font    => Font to use
//  color   => Black or White
//
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
	uint32_t i, b, j;

	// Check remaining space on current line
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
	SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight)) {
		// Not enough space on current line
		return 0;
	}

	// Translate font to screenbuffer
	for (i = 0; i < Font.FontHeight; i++) {
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (j = 0; j < Font.FontWidth; j++) {
			if ((b << j) & 0x8000) {
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			} else {
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) !color);
			}
		}
	}

	// The current space is now taken
	SSD1306.CurrentX += Font.FontWidth;

	// Return written char for validation
	return ch;
}

//
//  Write full string to screenbuffer
//
char ssd1306_WriteString(char *str, FontDef Font, SSD1306_COLOR color) {
	// Write until null-byte
	while (*str) {
		if (ssd1306_WriteChar(*str, Font, color) != *str) {
			// Char could not be written
			return *str;
		}

		// Next char
		str++;
	}

	// Everything ok
	return *str;
}

//
//  Invert background/foreground colors
//
void ssd1306_InvertColors(void) {
	SSD1306.Inverted = !SSD1306.Inverted;
}

//
//  Set cursor position
//
void ssd1306_SetCursor(uint8_t x, uint8_t y) {
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}
