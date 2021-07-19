#include "ssd1306.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Menu items to display
static char menuItems[NUM_MENU_ITEMS][MAX_KNOB_LABEL_CHARS + 1] = { "Load", "Save", "Delete", "TX Init" };

// Screen object
static SSD1306_t SSD1306;

// Used to prevent display updates when in menus
bool isDisplaysLocked = false;

//  Send a byte to the command register
static uint8_t ssd1306_WriteCommand(uint8_t command) {
    return HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}

//  Initialize the oled screen
uint8_t ssd1306_Init(Knob *k) {
    // Wait for the screen to boot
    int status = 0;

    // Select the screen to init
    ssd1306_Select(k);

    // Init LCD
    status += ssd1306_WriteCommand(0xAE);   // Display off
    status += ssd1306_WriteCommand(0x20);   // Set Memory Addressing Mode
    status += ssd1306_WriteCommand(0x10);   // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    status += ssd1306_WriteCommand(0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
    status += ssd1306_WriteCommand(0xC8);   // Set COM Output Scan Direction
    status += ssd1306_WriteCommand(0x00);   // Set low column address
    status += ssd1306_WriteCommand(0x10);   // Set high column address
    status += ssd1306_WriteCommand(0x40);   // Set start line address
    status += ssd1306_WriteCommand(0x81);   // set contrast control register
    status += ssd1306_WriteCommand(0x40);
    status += ssd1306_WriteCommand(0xA1);   // Set segment re-map 0 to 127
    status += ssd1306_WriteCommand(0xA6);   // Set normal display

    status += ssd1306_WriteCommand(0xA8);   // Set multiplex ratio(1 to 64)
    status += ssd1306_WriteCommand(SSD1306_HEIGHT - 1);

    status += ssd1306_WriteCommand(0xA4);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    status += ssd1306_WriteCommand(0xD3);   // Set display offset
    status += ssd1306_WriteCommand(0x00);   // No offset
    status += ssd1306_WriteCommand(0xD5);   // Set display clock divide ratio/oscillator frequency
    status += ssd1306_WriteCommand(0xF0);   // Set divide ratio
    status += ssd1306_WriteCommand(0xD9);   // Set pre-charge period
    status += ssd1306_WriteCommand(0x22);

    status += ssd1306_WriteCommand(0xDA);   // Set com pins hardware configuration
#ifdef SSD1306_COM_LR_REMAP
    status += ssd1306_WriteCommand(0x32);   // Enable COM left/right remap
#else
    status += ssd1306_WriteCommand(0x12);   // Do not use COM left/right remap
#endif

    status += ssd1306_WriteCommand(0xDB);   // Set vcomh
    status += ssd1306_WriteCommand(0x20);   // 0x20,0.77xVcc
    status += ssd1306_WriteCommand(0x8D);   // Set DC-DC enable
    status += ssd1306_WriteCommand(0x14);   //
    status += ssd1306_WriteCommand(0xAF);   // Turn on SSD1306 panel

    if (status != 0) {
        return 1;
    }

    ssd1306_Fill(Black);
    ssd1306_AdjustBrightness(0x63);
    ssd1306_UpdateScreen(&hi2c1);

    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    SSD1306.Initialized = 1;

    return 0;
}

uint8_t ssd1306_Init2() {
    // Wait for the screen to boot
    int status = 0;

    // Select the screen to init

    // Init LCD
    status += ssd1306_WriteCommand(0xAE);   // Display off
    status += ssd1306_WriteCommand(0x20);   // Set Memory Addressing Mode
    status += ssd1306_WriteCommand(0x10);   // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    status += ssd1306_WriteCommand(0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
    status += ssd1306_WriteCommand(0xC8);   // Set COM Output Scan Direction
    status += ssd1306_WriteCommand(0x00);   // Set low column address
    status += ssd1306_WriteCommand(0x10);   // Set high column address
    status += ssd1306_WriteCommand(0x40);   // Set start line address
    status += ssd1306_WriteCommand(0x81);   // set contrast control register
    status += ssd1306_WriteCommand(0x40);
    status += ssd1306_WriteCommand(0xA1);   // Set segment re-map 0 to 127
    status += ssd1306_WriteCommand(0xA6);   // Set normal display

    status += ssd1306_WriteCommand(0xA8);   // Set multiplex ratio(1 to 64)
    status += ssd1306_WriteCommand(SSD1306_HEIGHT - 1);

    status += ssd1306_WriteCommand(0xA4);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    status += ssd1306_WriteCommand(0xD3);   // Set display offset
    status += ssd1306_WriteCommand(0x00);   // No offset
    status += ssd1306_WriteCommand(0xD5);   // Set display clock divide ratio/oscillator frequency
    status += ssd1306_WriteCommand(0xF0);   // Set divide ratio
    status += ssd1306_WriteCommand(0xD9);   // Set pre-charge period
    status += ssd1306_WriteCommand(0x22);

    status += ssd1306_WriteCommand(0xDA);   // Set com pins hardware configuration
#ifdef SSD1306_COM_LR_REMAP
    status += ssd1306_WriteCommand(0x32);   // Enable COM left/right remap
#else
    status += ssd1306_WriteCommand(0x12);   // Do not use COM left/right remap
#endif

    status += ssd1306_WriteCommand(0xDB);   // Set vcomh
    status += ssd1306_WriteCommand(0x20);   // 0x20,0.77xVcc
    status += ssd1306_WriteCommand(0x8D);   // Set DC-DC enable
    status += ssd1306_WriteCommand(0x14);   //
    status += ssd1306_WriteCommand(0xAF);   // Turn on SSD1306 panel

    if (status != 0) {
        return 1;
    }

    ssd1306_Fill(Black);
    ssd1306_AdjustBrightness(0x63);
    ssd1306_UpdateScreen(&hi2c1);

    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    SSD1306.Initialized = 1;

    return 0;
}

//  Fill the whole screen with the given color
void ssd1306_Fill(SSD1306_COLOR color) {
    // Fill screenbuffer with a constant value (color)
    uint32_t i;

    for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

void ssd1306_FillAll(SSD1306_COLOR color) {
    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        ssd1306_Select(&knobs[Knob_Index(i)]);
        ssd1306_Fill(color);
        ssd1306_UpdateScreen();
    }
}

//  Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen() {
    for (uint8_t i = 0; i < 8; i++) {
        ssd1306_WriteCommand(0xB0 + i);
        ssd1306_WriteCommand(0x00);
        ssd1306_WriteCommand(0x10);
        HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH, 100);
    }
}

void ssd1306_WriteAllKnobs() {
    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        ssd1306_WriteKnob(&knobs[Knob_Index(i)]);
    }
}

// Write a knob's values to the screen
void ssd1306_WriteKnob(Knob *k) {
    ssd1306_Select(k);
    ssd1306_Fill(Black);

    //ssd1306_DrawBorder(White);

    // Draw top line
    char channel_string[3];
    char cc_string[4];
    char init_indicator_string[16] = "       @       ";
    char value_string[4];

    snprintf(channel_string, sizeof(channel_string) / sizeof(channel_string[0]), "%.2d", (int) k->channel);
    snprintf(cc_string, sizeof(cc_string) / sizeof(cc_string[0]), "%.3d", (int) k->cc);
    snprintf(init_indicator_string, strlen(init_indicator_string), "%s", update_init_indicator(k));
    snprintf(value_string, sizeof(value_string) / sizeof(value_string[0]), "%.3d",
            (int) Knob_Map(k, k->value, k->max_range));

    ssd1306_SetCursor(2, 2);
    ssd1306_WriteString(channel_string, NumFont_5x7, White);

    ssd1306_SetCursor(2, NumFont_5x7.FontHeight + 3);
    ssd1306_WriteString(cc_string, NumFont_5x7, White);

    ssd1306_SetCursor((SSD1306_WIDTH - strlen(init_indicator_string) * NumFont_5x7.FontWidth) / 2 - 2, (NumFont_5x7.FontHeight + 1) / 2 + 2);
    ssd1306_WriteString(init_indicator_string, NumFont_5x7, White);

    ssd1306_SetCursor(SSD1306_WIDTH - sizeof(value_string) / sizeof(value_string[0]) * NumFont_5x7.FontWidth + 2, (NumFont_5x7.FontHeight + 1) / 2 + 2);
    ssd1306_WriteString(value_string, NumFont_5x7, White);

    // Draw top line separator
    uint8_t y_topline = SSD1306.CurrentY + NumFont_5x7.FontHeight + 3;
    ssd1306_DrawLine(0, y_topline, SSD1306_WIDTH - 1, y_topline, White);

    uint8_t len_label = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t y_remaining = SSD1306_HEIGHT - NumFont_5x7.FontHeight;

    // Draw main label
    uint8_t y_remaining_div = (k->num_sl > 0) ? 3 : 2;
    len_label = strlen(k->label);
    x = (SSD1306_WIDTH - len_label * Font_8x14.FontWidth) / 2;
    y = (float) y_remaining / y_remaining_div + (Font_8x14.FontHeight / 4) - 2;
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(k->label, Font_8x14, White);

    // If the max number of values is restricted, we want to use
    // sub labels for each choice (e.g. osc. wave selection)
    if (k->num_sl > 0) {
        uint8_t sl_index = (k->max_values == MIDI_MAX + 1) ? 0 :
                           (k->isLocked) ? k->lock_value : k->value;
        //uint8_t sl_index = (k->max_values < MIDI_MAX + 1) ? k->value : 0;
        len_label = strlen(k->sub_labels[sl_index]);
        x = (SSD1306_WIDTH - len_label * Font_8x14.FontWidth) / 2;
        y = SSD1306_HEIGHT - Font_8x14.FontHeight - 5;
        ssd1306_SetCursor(x, y);
        ssd1306_WriteString(k->sub_labels[sl_index], Font_8x14, White);
    }

    ssd1306_UpdateScreen();
}

void ssd1306_WriteMainMenu() {
    ssd1306_FillAll(Black);
    for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++) {
        uint8_t ki = i * NUM_COLS;
        ssd1306_Select(&knobs[Knob_Index(ki)]);
        ssd1306_Fill(Black);
        ssd1306_DrawBorder(White);
        uint8_t x = (SSD1306_WIDTH - strlen(menuItems[i]) * Font_8x14.FontWidth) / 2;
        uint8_t y = SSD1306_HEIGHT / 2 - Font_8x14.FontHeight / 2;
        ssd1306_SetCursor(x, y);
        ssd1306_WriteString(menuItems[i], Font_8x14, White);

        ssd1306_UpdateScreen();
    }
}

void ssd1306_WritePresets() {
    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        ssd1306_Select(&knobs[Knob_Index(i)]);
        ssd1306_Fill(Black);

        // Draw number of pages
        char page_num[2];
        snprintf(page_num, sizeof(page_num) / sizeof(page_num[0]), "%d", 3);
        ssd1306_SetCursor(SSD1306_WIDTH - sizeof(page_num) / sizeof(page_num[0]) * NumFont_5x7.FontWidth - 1, (NumFont_5x7.FontHeight + 1) / 2);
        ssd1306_WriteString(page_num, NumFont_5x7, White);

        uint8_t len_name = strlen(presets[i].name);
        uint8_t len_sl = strlen(presets[i].sub_label);

        // Draw preset name
        uint8_t x = (SSD1306_WIDTH - len_name * Font_8x14.FontWidth) / 2;
        uint8_t y_div = (len_sl == 0) ? 2 : 3;
        uint8_t y = SSD1306_HEIGHT / y_div - Font_8x14.FontHeight / 2;
        ssd1306_SetCursor(x, y);
        ssd1306_WriteString(presets[i].name, Font_8x14, White);

        // Draw preset label (if exists)
        if (len_sl > 0) {
            x = (SSD1306_WIDTH - len_sl * Font_8x14.FontWidth) / 2;
            //y = 2 * SSD1306_HEIGHT / y_div - Font_8x14.FontHeight / 2;
            y = SSD1306_HEIGHT - Font_8x14.FontHeight - 1;
            ssd1306_SetCursor(x, y);
            ssd1306_WriteString(presets[i].sub_label, Font_8x14, White);
        }

        ssd1306_UpdateScreen();
    }
}

void ssd1306_DrawBorder(SSD1306_COLOR color) {
    ssd1306_DrawRectangle(0, 0, SSD1306_WIDTH - 1, 0, White);
    ssd1306_DrawRectangle(SSD1306_WIDTH - 1, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, White);
    ssd1306_DrawRectangle(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 0, SSD1306_HEIGHT - 1, White);
    ssd1306_DrawRectangle(0, SSD1306_HEIGHT - 1, 0, 0, White);
}

//Draw rectangle
void ssd1306_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    ssd1306_DrawLine(x1, y1, x2, y1, color);
    ssd1306_DrawLine(x2, y1, x2, y2, color);
    ssd1306_DrawLine(x2, y2, x1, y2, color);
    ssd1306_DrawLine(x1, y2, x1, y1, color);

    return;
}

// Draw line by Bresenhem's algorithm
void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;

    ssd1306_DrawPixel(x2, y2, color);
    while ((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(x1, y1, color);
        error2 = error * 2;
        if (error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        else {
            /*nothing to do*/
        }

        if (error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
        else {
            /*nothing to do*/
        }
    }
    return;
}

//  Draw one pixel in the screenbuffer
//  X => X Coordinate
//  Y => Y Coordinate
//  color => Pixel color
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

//  Draw 1 char to the screen buffer
//  ch      => Character to write
//  Font    => Font to use
//  color   => Black or White
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

//  Write full string to screenbuffer
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

void ssd1306_WriteErrorCode(char *label, uint8_t y, uint8_t err_code) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, y);
    ssd1306_WriteString("label: %s\n", Font_7x10, White);

    char err_str[16];
    snprintf(err_str, sizeof(err_str) / sizeof(err_str[0]), "%d", err_code);
    ssd1306_SetCursor(2 * SSD1306_WIDTH / 3, y);
    ssd1306_WriteString(err_str, Font_7x10, White);

    ssd1306_UpdateScreen();
}

//  Invert background/foreground colors
void ssd1306_InvertColors(void) {
    SSD1306.Inverted = !SSD1306.Inverted;
}

//  Set cursor position
void ssd1306_SetCursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

// Adjust brightness
void ssd1306_AdjustBrightness(uint8_t brightness) {
    uint8_t contrast_cmd = 0x81;
    uint8_t contrast_level = brightness;
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x00, 1, &contrast_cmd, 1, 10);
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, 0x00, 1, &contrast_level, 1, 10);
}

// Select a display
void ssd1306_Select(Knob *k) {
    /*
     uint8_t row_mux = (k->col < MAX(NUM_COLS / 2, 1)) ? k->row : k->row + NUM_ROWS;
     uint8_t col_mux = (k->col < MAX(NUM_COLS / 2, 1)) ? k->col : k->col - NUM_COLS / 2;
     i2c_Select(I2C_MUX_MASTER_ADDR, row_mux);
     i2c_Select(I2C_MUX_SLAVE_ADDR, col_mux);
     */
    uint8_t masterMux;
    uint8_t slaveMux;
    i2c_Select(I2C_MUX_MASTER_ADDR, k->row);

    i2c_Select(I2C_MUX_SLAVE_ADDR, k->col);


}

// Select an output in an i2c mux
void i2c_Select(uint8_t mux_addr, uint8_t i) {
    if (i > 7) return;
    unsigned char temp[1];
    temp[0] = 1 << i;
    HAL_I2C_Master_Transmit(&hi2c1, mux_addr, temp, 1, 100);
}

// Update the init value closeness indicator
char* update_init_indicator(Knob *k) {
    int8_t init_diff = Knob_Map(k, k->lock_value, MIDI_MAX) - Knob_Map(k, k->value, MIDI_MAX);
    uint8_t init_pct = 1.0f * abs(init_diff) / MIDI_MAX * 100;

    if (init_diff == 0) return "       @       ";

    if (init_pct < 15) {
        if (init_diff > 0) return "      >        ";
        else return "        <      ";
    } else if (init_pct >= 15 && init_pct < 30) {
        if (init_diff > 0) return "     >>        ";
        else return "        <<     ";
    } else if (init_pct >= 30 && init_pct < 45) {
        if (init_diff > 0) return "    >>>        ";
        else return "        <<<    ";
    } else if (init_pct >= 45 && init_pct < 61) {
        if (init_diff > 0) return "   >>>>        ";
        else return "        <<<<   ";
    } else if (init_pct >= 61 && init_pct < 77) {
        if (init_diff > 0) return "  >>>>>        ";
        else return "        <<<<<  ";
    } else if (init_pct >= 77 && init_pct < 93) {
        if (init_diff > 0) return " >>>>>>        ";
        else return "        <<<<<< ";
    } else if (init_pct >= 93) {
        if (init_diff > 0) return ">>>>>>>        ";
        else return "        <<<<<<<";
    } else {
        return " ";
    }
}

