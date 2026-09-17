/*
 * This program source code file is part of WiCAD, a fork of KiCad.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PCAD_MODE_H
#define PCAD_MODE_H

#include <kicommon.h>

/**
 * WiCAD's P-CAD mode makes the schematic and PCB editors look, feel and behave like P-CAD 2006
 * while keeping all files native KiCad.  The switch lives in the common settings
 * (kicad_common.json, "pcad_mode.enabled"), defaults to on, and is remembered between sessions.
 *
 * Every P-CAD specific behaviour should be guarded by this call so stock KiCad behaviour
 * remains available by turning the mode off.
 *
 * @return true if P-CAD mode is enabled (also true when settings are not loaded yet).
 */
KICOMMON_API bool IsPCadMode();

#endif // PCAD_MODE_H
