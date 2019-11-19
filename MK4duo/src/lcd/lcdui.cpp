/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
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
 * lcdui.cpp
 *
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
 */
 
#include "../../MK4duo.h"
#include "sanitycheck.h"

LcdUI lcdui;

#if !HAS_LCD

void LcdUI::set_status(const char* const message, const bool)         { host_action.action_notify(message); }
void LcdUI::set_status_P(PGM_P const message, int8_t)                 { host_action.action_notify_P(message); }
void LcdUI::status_printf_P(const uint8_t, PGM_P const message, ...)  { host_action.action_notify_P(message); }

#endif
