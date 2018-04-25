/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * u8g_dev_st7565_64128n_HAL.c (Displaytech)
 *
 * Universal 8bit Graphics Library
 *
 * Copyright (c) 2011, olikraus@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this list
 *    of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice, this
 *    list of conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "../../../MK4duo.h"

#if ENABLED(DOGLCD)

#include <U8glib.h>

#include "HAL_LCD_com_defines.h"

#define WIDTH 128
#define HEIGHT 64
#define PAGE_HEIGHT 8

/* init sequence from https://github.com/adafruit/ST7565-LCD/blob/master/ST7565/ST7565.cpp */
static const uint8_t u8g_dev_st7565_64128n_HAL_init_seq[] PROGMEM = {
   U8G_ESC_CS(0),       // disable chip
    U8G_ESC_ADR(0),     // instruction mode
    U8G_ESC_CS(1),      // enable chip
    U8G_ESC_RST(15),    // do reset low pulse with (15*16)+2 milliseconds (=maximum delay)*/

    0x0A2,              // 0x0a2: LCD bias 1/9 (according to Displaytech 64128N datasheet)
    0x0A0,              // Normal ADC Select (according to Displaytech 64128N datasheet)

    0x0c8,              // common output mode: set scan direction normal operation/SHL Select, 0x0c0 --> SHL = 0, normal, 0x0c8 --> SHL = 1
    0x040,              // Display start line for Displaytech 64128N

    0x028 | 0x04,       // power control: turn on voltage converter
    U8G_ESC_DLY(50),    // delay 50 ms

    0x028 | 0x06,       // power control: turn on voltage regulator
    U8G_ESC_DLY(50),    // delay 50 ms

    0x028 | 0x07,       // power control: turn on voltage follower
    U8G_ESC_DLY(50),    // delay 50 ms

    0x010,              // Set V0 voltage resistor ratio. Setting for controlling brightness of Displaytech 64128N

    0x0a6,              // display normal, bit val 0: LCD pixel off.

    0x081,              // set contrast
    0x01e,              // Contrast value. Setting for controlling brightness of Displaytech 64128N


    0x0af,              // display on

    U8G_ESC_DLY(100),   // delay 100 ms
    0x0a5,              // display all points, ST7565
    U8G_ESC_DLY(100),   // delay 100 ms
    U8G_ESC_DLY(100),   // delay 100 ms
    0x0a4,              // normal display
    U8G_ESC_CS(0),      // disable chip
    U8G_ESC_END         // end of sequence
};

static const uint8_t u8g_dev_st7565_64128n_HAL_data_start[] PROGMEM = {
  U8G_ESC_ADR(0),       // instruction mode
  U8G_ESC_CS(1),        // enable chip
  0x010,                // set upper 4 bit of the col adr to 0x10
  0x000,                // set lower 4 bit of the col adr to 0x00. Changed for DisplayTech 64128N
  U8G_ESC_END           // end of sequence
};

static const uint8_t u8g_dev_st7565_64128n_HAL_sleep_on[] PROGMEM = {
  U8G_ESC_ADR(0),       // instruction mode
  U8G_ESC_CS(1),        // enable chip
  0x0ac,                // static indicator off
  0x000,                // indicator register set (not sure if this is required)
  0x0ae,                // display off
  0x0a5,                // all points on
  U8G_ESC_CS(0),        // disable chip, bugfix 12 nov 2014
  U8G_ESC_END           // end of sequence
  };

static const uint8_t u8g_dev_st7565_64128n_HAL_sleep_off[] PROGMEM = {
  U8G_ESC_ADR(0),       // instruction mode
  U8G_ESC_CS(1),        // enable chip
  0x0a4,                // all points off
  0x0af,                // display on
  U8G_ESC_DLY(50),      // delay 50 ms
  U8G_ESC_CS(0),        // disable chip, bugfix 12 nov 2014
  U8G_ESC_END           // end of sequence
};

uint8_t u8g_dev_st7565_64128n_HAL_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case U8G_DEV_MSG_INIT:
      u8g_InitCom(u8g, dev, U8G_SPI_CLK_CYCLE_400NS);
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_init_seq);
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_NEXT: {
        u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
        u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_data_start);
        u8g_WriteByte(u8g, dev, 0x0b0 | pb->p.page); /* select current page (ST7565R) */
        u8g_SetAddress(u8g, dev, 1);           /* data mode */
        if ( u8g_pb_WriteBuffer(pb, u8g, dev) == 0 )
          return 0;
        u8g_SetChipSelect(u8g, dev, 0);
      }
      break;
    case U8G_DEV_MSG_CONTRAST:
      u8g_SetChipSelect(u8g, dev, 1);
      u8g_SetAddress(u8g, dev, 0);          /* instruction mode */
      u8g_WriteByte(u8g, dev, 0x081);
      u8g_WriteByte(u8g, dev, (*(uint8_t *)arg) >> 2);
      u8g_SetChipSelect(u8g, dev, 0);
      return 1;
    case U8G_DEV_MSG_SLEEP_ON:
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_sleep_on);
      return 1;
    case U8G_DEV_MSG_SLEEP_OFF:
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_sleep_off);
      return 1;
  }
  return u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
}

uint8_t u8g_dev_st7565_64128n_HAL_2x_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case U8G_DEV_MSG_INIT:
      u8g_InitCom(u8g, dev, U8G_SPI_CLK_CYCLE_400NS);
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_init_seq);
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_NEXT: {
        u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);

        u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_data_start);
        u8g_WriteByte(u8g, dev, 0x0b0 | (2*pb->p.page)); /* select current page (ST7565R) */
        u8g_SetAddress(u8g, dev, 1);           /* data mode */
        u8g_WriteSequence(u8g, dev, pb->width, (uint8_t *)pb->buf);
        u8g_SetChipSelect(u8g, dev, 0);

        u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_data_start);
        u8g_WriteByte(u8g, dev, 0x0b0 | (2*pb->p.page+1)); /* select current page (ST7565R) */
        u8g_SetAddress(u8g, dev, 1);           /* data mode */
        u8g_WriteSequence(u8g, dev, pb->width, (uint8_t *)(pb->buf)+pb->width);
        u8g_SetChipSelect(u8g, dev, 0);
      }
      break;
    case U8G_DEV_MSG_CONTRAST:
      u8g_SetChipSelect(u8g, dev, 1);
      u8g_SetAddress(u8g, dev, 0);          /* instruction mode */
      u8g_WriteByte(u8g, dev, 0x081);
      u8g_WriteByte(u8g, dev, (*(uint8_t *)arg) >> 2);
      u8g_SetChipSelect(u8g, dev, 0);
      return 1;
    case U8G_DEV_MSG_SLEEP_ON:
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_sleep_on);
      return 1;
    case U8G_DEV_MSG_SLEEP_OFF:
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_st7565_64128n_HAL_sleep_off);
      return 1;
  }
  return u8g_dev_pb16v1_base_fn(u8g, dev, msg, arg);
}


U8G_PB_DEV(u8g_dev_st7565_64128n_HAL_sw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, u8g_dev_st7565_64128n_HAL_fn, U8G_COM_HAL_SW_SPI_FN);

uint8_t u8g_dev_st7565_64128n_HAL_2x_buf[WIDTH*2] U8G_NOCOMMON ;
u8g_pb_t u8g_dev_st7565_64128n_HAL_2x_pb = { {16, HEIGHT, 0, 0, 0},  WIDTH, u8g_dev_st7565_64128n_HAL_2x_buf};
u8g_dev_t u8g_dev_st7565_64128n_HAL_2x_sw_spi = { u8g_dev_st7565_64128n_HAL_2x_fn, &u8g_dev_st7565_64128n_HAL_2x_pb, U8G_COM_HAL_SW_SPI_FN };


U8G_PB_DEV(u8g_dev_st7565_64128n_HAL_hw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, u8g_dev_st7565_64128n_HAL_fn, U8G_COM_HAL_HW_SPI_FN);
u8g_dev_t u8g_dev_st7565_64128n_HAL_2x_hw_spi = { u8g_dev_st7565_64128n_HAL_2x_fn, &u8g_dev_st7565_64128n_HAL_2x_pb, U8G_COM_HAL_HW_SPI_FN };

#endif // DOGLCD
