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

#ifndef PCAD_VOCABULARY_H
#define PCAD_VOCABULARY_H

#include <string>
#include <kicommon.h>
#include <wx/string.h>

/**
 * WiCAD: P-CAD wording for the user interface.
 *
 * In P-CAD mode every action label and tooltip goes through PCadLabel().  Actions with an exact
 * P-CAD counterpart get P-CAD's own menu name (Component, Copper Pour, Keepout, Wire, Port, ...);
 * everything else gets KiCad's terms swapped for P-CAD's (Footprint -> Pattern, Zone -> Copper Pour,
 * Track -> Trace, Ratsnest -> Connections, Annotate -> Renumber, Lock -> Fix, ...).
 *
 * @param aActionName the action's name, e.g. "pcbnew.InteractiveDrawing.zone"
 * @param aText       the (already translated) KiCad label or tooltip
 * @param aIsLabel    true for a menu label / friendly name, false for a tooltip or description
 */
KICOMMON_API wxString PCadLabel( const std::string& aActionName, const wxString& aText, bool aIsLabel );

/// The fork's name in user-visible text: "KiCad" -> "WiCAD" as a whole word.  Applies in every mode.
KICOMMON_API wxString WiCadBrand( const wxString& aText );

/// Apply only the word substitutions (for text that does not belong to an action).
KICOMMON_API wxString PCadWording( const wxString& aText );

/**
 * P-CAD's prompt-line text for a tool, taken from P-CAD 2006's own string tables.
 *
 * @return the prompt for the tool's first state, or the select-tool prompt if the tool has none.
 */
KICOMMON_API wxString PCadPrompt( const std::string& aActionName );

#endif // PCAD_VOCABULARY_H
