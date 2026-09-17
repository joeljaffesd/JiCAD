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

#ifndef PCAD_HOTKEYS_H
#define PCAD_HOTKEYS_H

#include <map>
#include <string>
#include <utility>
#include <tool/tool_event.h>
#include <wx/defs.h>

/**
 * Default hotkeys in P-CAD mode, keyed by KiCad action name: { primary, alternate }.
 *
 * Taken from a P-CAD 2006 user's Pcb.key and Sch.key (see research/FINDINGS.md section 10 in the
 * WiCAD project).  These replace an action's built-in default; a binding the user made in
 * Preferences > Hotkeys still wins.  P-CAD commands with no KiCad equivalent yet (Abs/Rel grid
 * toggle, Enter Coord, cursor style toggle, route angle cycle, layer sets, macros) are not listed.
 */
static const std::map<std::string, std::pair<int, int>> s_pcadHotKeys =
        {
            // ---- both editors ------------------------------------------------------------------
            { "common.InteractiveSelection.selectionTool", { 'S',              0 } },              // Edit Select
            { "common.Interactive.undo",                   { 'U',              MD_CTRL + 'Z' } },  // Edit Undo
            { "common.Interactive.redo",                   { MD_SHIFT + 'U',   MD_CTRL + 'Y' } },  // Edit Redo
            { "common.Control.gridNext",                   { 'G',              0 } },              // Grid Next
            { "common.Control.gridPrev",                   { MD_SHIFT + 'G',   0 } },              // Grid Previous
            { "common.Control.zoomCenter",                 { 'C',              0 } },              // Zoom Center
            { "common.Control.zoomTool",                   { 'Z',              0 } },              // Zoom Window
            { "common.Control.zoomIn",                     { '=',              '+' } },            // Zoom In
            { "common.Control.zoomOut",                    { '-',              0 } },              // Zoom Out
            { "common.Control.zoomRedraw",                 { WXK_F4,           0 } },              // View Redraw
            { "common.Control.zoomFitScreen",              { WXK_F8,           0 } },              // View All
            { "common.Control.zoomFitObjects",             { WXK_F11,          0 } },              // View Extent
            { "common.Control.save",                       { WXK_F12,          MD_CTRL + 'S' } },  // File Save
            { "common.Control.cursorClick",                { ' ',              WXK_RETURN } },     // Mouse Toggle Left
            { "common.Control.panUp",                      { WXK_PAGEUP,       0 } },              // Scroll Page Up
            { "common.Control.panDown",                    { WXK_PAGEDOWN,     0 } },              // Scroll Page Down
            { "common.Control.panLeft",                    { MD_CTRL + WXK_PAGEUP,   0 } },        // Scroll Page Left
            { "common.Control.panRight",                   { MD_CTRL + WXK_PAGEDOWN, 0 } },        // Scroll Page Right

            // ---- PCB editor --------------------------------------------------------------------
            { "pcbnew.Control.layerNext",                  { 'L',              0 } },              // Layer Next
            { "pcbnew.Control.layerPrev",                  { 'P',              0 } },              // Layer Previous
            { "pcbnew.EditorControl.trackWidthInc",        { 'W',              0 } },              // Line Width Next
            { "pcbnew.EditorControl.trackWidthDec",        { MD_SHIFT + 'W',   0 } },              // Line Width Previous
            { "pcbnew.Control.showLayersManager",          { 'Y',              0 } },              // Options Layers
            { "pcbnew.Control.netColorMode",               { MD_SHIFT + 'N',   0 } },              // Use Net Color Toggle
            { "pcbnew.Control.trackDisplayMode",           { 'Q',              0 } },              // Draft Mode Toggle

            // ---- Schematic editor --------------------------------------------------------------
            { "eeschema.NavigateTool.next",                { 'L',              0 } },              // Sheet Next
            { "eeschema.NavigateTool.previous",            { MD_SHIFT + 'L',   0 } },              // Sheet Previous
            { "eeschema.EditorControl.clearHighlight",     { WXK_F2,           0 } },              // Edit Unhighlight All
            { "eeschema.InteractiveEdit.mirrorH",          { 'F',              'X' } }             // <F>lip while placing
        };

#endif // PCAD_HOTKEYS_H
