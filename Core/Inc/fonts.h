#ifndef _FONTS_H
#define _FONTS_H

#include <stdint.h>

typedef struct {
    const uint8_t FontWidth;    /* Font width in pixels */
    uint8_t FontHeight;         /* Font height in pixels */
    const uint16_t *data;       /* Pointer to data font data array */
} FontDef;

extern FontDef Font_1;
extern FontDef Font_2;
extern FontDef Font_3;
extern FontDef Font_4;
extern FontDef Font_5;
extern FontDef Font_6;

extern FontDef NumFont_5x7;
extern FontDef Font_7x10;
extern FontDef Font_8x14;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#endif  // _FONTS_H
