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

#ifndef PCAD_COLOR_THEME_H
#define PCAD_COLOR_THEME_H

#include <map>
#include <gal/color4d.h>
#include <layer_ids.h>

using KIGFX::COLOR4D;

/**
 * Colours of the built-in "P-CAD" theme, applied on top of the KiCad Classic theme so that every
 * colour slot KiCad knows about is defined.
 *
 * Values come from a long-time P-CAD 2006 user's Pcb.ini and Sch.ini (LayerColorN, BackgroundColor,
 * GridColor, SelectionColor, ...; Windows BGR in the INI, RGB here).  P-CAD colours each layer per
 * object type (via, pad, line, poly, text); KiCad has one colour per layer, so the line colour is
 * used.  P-CAD's Board, Top Silk and Bot Silk layers had no override in the INI and use P-CAD's
 * stock colours.
 */
static const std::map<int, COLOR4D> s_pcadThemeOverrides =
        {
            // ---- PCB editor -------------------------------------------------------------------
            { LAYER_PCB_BACKGROUND,         CSS_COLOR(   0,   0,   0, 1 ) },
            { LAYER_GRID,                   CSS_COLOR(   0,   0, 128, 1 ) },    // GridColor=800000 (navy)
            { LAYER_GRID_AXES,              CSS_COLOR(   0,   0, 128, 1 ) },
            { LAYER_CURSOR,                 CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_RATSNEST,               CSS_COLOR(   0, 255, 255, 1 ) },    // ConnectionColor=ffff00 (cyan)
            { LAYER_SELECT_OVERLAY,         CSS_COLOR(   0, 255, 255, 1 ) },    // SelectionColor=ffff00 (cyan)
            { LAYER_VIA_STITCHING,          CSS_COLOR(   0, 255, 255, 1 ) },
            { LAYER_VIA_HOLES,              CSS_COLOR(   0,   0,   0, 1 ) },
            { LAYER_VIA_HOLEWALLS,          CSS_COLOR(   0, 255, 255, 1 ) },    // vias are cyan on every layer
            { LAYER_PAD_PLATEDHOLES,        CSS_COLOR(   0,   0,   0, 1 ) },
            { LAYER_NON_PLATEDHOLES,        CSS_COLOR( 255, 255,   0, 1 ) },
            { LAYER_DRAWINGSHEET,           CSS_COLOR( 128, 128, 128, 1 ) },

            { F_Cu,                         CSS_COLOR(   0, 255,   0, 1 ) },    // Top: green
            { B_Cu,                         CSS_COLOR( 255,   0,   0, 1 ) },    // Bottom: red
            { F_Mask,                       CSS_COLOR( 128,   0, 128, 0.85 ) }, // Top Mask: purple
            { B_Mask,                       CSS_COLOR( 128, 128,   0, 0.85 ) }, // Bot Mask: olive
            { F_Paste,                      CSS_COLOR( 192, 220, 192, 0.9 ) },  // Top Paste: pale green
            { B_Paste,                      CSS_COLOR( 255,   0, 255, 0.9 ) },  // Bot Paste: magenta
            { F_Fab,                        CSS_COLOR( 255,   0, 255, 1 ) },    // Top Assy: magenta
            { B_Fab,                        CSS_COLOR( 255, 255,   0, 1 ) },    // Bot Assy: yellow
            { F_SilkS,                      CSS_COLOR( 255, 255, 255, 1 ) },    // Top Silk: P-CAD stock white
            { B_SilkS,                      CSS_COLOR( 192, 192, 192, 1 ) },    // Bot Silk
            { Edge_Cuts,                    CSS_COLOR( 255, 255,   0, 1 ) },    // Board: P-CAD stock yellow
            { F_CrtYd,                      CSS_COLOR( 128, 128, 128, 1 ) },
            { B_CrtYd,                      CSS_COLOR(  96,  96,  96, 1 ) },

            // ---- Schematic editor -------------------------------------------------------------
            { LAYER_SCHEMATIC_BACKGROUND,   CSS_COLOR(   0,   0,   0, 1 ) },
            { LAYER_SCHEMATIC_GRID,         CSS_COLOR( 255, 255, 255, 0.5 ) },  // GridColor=ffffff
            { LAYER_SCHEMATIC_GRID_AXES,    CSS_COLOR(   0, 255,   0, 1 ) },    // GridColor10x=ff00
            { LAYER_SCHEMATIC_CURSOR,       CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_SCHEMATIC_DRAWINGSHEET, CSS_COLOR( 128, 128, 128, 1 ) },    // BorderColor
            { LAYER_SCHEMATIC_PAGE_LIMITS,  CSS_COLOR( 128, 128, 128, 1 ) },
            { LAYER_SCHEMATIC_AUX_ITEMS,    CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_SCHEMATIC_ANCHOR,       CSS_COLOR(   0, 255, 255, 1 ) },
            { LAYER_WIRE,                   CSS_COLOR(   0, 255,   0, 1 ) },    // wire: green
            { LAYER_BUS,                    CSS_COLOR( 255, 255, 255, 1 ) },    // bus: white
            { LAYER_JUNCTION,               CSS_COLOR(   0, 255, 255, 1 ) },    // junction: cyan
            { LAYER_BUS_JUNCTION,           CSS_COLOR(   0, 255, 255, 1 ) },
            { LAYER_PIN,                    CSS_COLOR( 255,   0, 255, 1 ) },    // pin: magenta
            { LAYER_PINNAM,                 CSS_COLOR( 255,   0, 255, 1 ) },
            { LAYER_PINNUM,                 CSS_COLOR( 255,   0, 255, 1 ) },
            { LAYER_DEVICE,                 CSS_COLOR( 128, 128, 128, 1 ) },    // PartColor=808080
            { LAYER_DEVICE_BACKGROUND,      CSS_COLOR(   0,   0,   0, 0 ) },    // P-CAD parts are not filled
            { LAYER_REFERENCEPART,          CSS_COLOR(   0, 128, 128, 1 ) },    // PartAttrColor=808000 (teal)
            { LAYER_VALUEPART,              CSS_COLOR(   0, 128, 128, 1 ) },
            { LAYER_FIELDS,                 CSS_COLOR(   0, 128, 128, 1 ) },
            { LAYER_LOCLABEL,               CSS_COLOR(   0, 255,   0, 1 ) },    // WireAttrColor=ff00 (green)
            { LAYER_GLOBLABEL,              CSS_COLOR(   0, 255,   0, 1 ) },
            { LAYER_HIERLABEL,              CSS_COLOR(   0, 255,   0, 1 ) },
            { LAYER_NETCLASS_REFS,          CSS_COLOR(   0, 255,   0, 1 ) },
            { LAYER_NOTES,                  CSS_COLOR( 255, 255, 255, 1 ) },    // line: white
            { LAYER_PRIVATE_NOTES,          CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_SHEET,                  CSS_COLOR(   0, 255, 255, 1 ) },    // ModuleColor=ffff00 (cyan)
            { LAYER_SHEET_BACKGROUND,       CSS_COLOR(   0,   0,   0, 0 ) },
            { LAYER_SHEETNAME,              CSS_COLOR(   0, 255, 255, 1 ) },
            { LAYER_SHEETFILENAME,          CSS_COLOR(   0, 255, 255, 1 ) },
            { LAYER_SHEETFIELDS,            CSS_COLOR(   0, 255, 255, 1 ) },
            { LAYER_SHEETLABEL,             CSS_COLOR(   0, 255,   0, 1 ) },
            { LAYER_NOCONNECT,              CSS_COLOR( 255, 255,   0, 1 ) },    // OpenEndColor=ffff (yellow)
            { LAYER_BRIGHTENED,             CSS_COLOR( 255,   0, 255, 1 ) },    // HiliteColor=ff00ff
            { LAYER_HOVERED,                CSS_COLOR( 255,   0, 255, 1 ) },
            { LAYER_SELECTION_SHADOWS,      CSS_COLOR( 255,   0, 255, 0.6 ) },  // SelectionColor=ff00ff
            { LAYER_HIDDEN,                 CSS_COLOR(  96,  96,  96, 1 ) },
            { LAYER_OP_VOLTAGES,            CSS_COLOR( 255, 255,   0, 1 ) },
            { LAYER_OP_CURRENTS,            CSS_COLOR( 255, 128,   0, 1 ) }
        };

#endif // PCAD_COLOR_THEME_H
