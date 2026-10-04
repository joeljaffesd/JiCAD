/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef JLC_PART_IMPORT_H
#define JLC_PART_IMPORT_H

#include <lib_id.h>

#include <wx/string.h>

class SCH_BASE_FRAME;

/**
 * Return the LCSC part number written in \a aText (a "C" followed by digits, such as C2040), or
 * an empty string when the text is anything else.
 */
wxString JlcPartNumberFromText( const wxString& aText );

/**
 * Import a JLCPCB/LCSC part into the project's "JLC" libraries.
 *
 * Fetches the part's symbol, footprint and 3D model from EasyEDA and saves them in a JLC-PARTS
 * folder beside the project as JLC.kicad_sym, JLC.pretty and JLC.3dshapes, adding both libraries to
 * the project's library tables.  The symbol is linked to the footprint and carries the part number
 * in an LCSC field.
 *
 * @param aSymbolId receives the LIB_ID of the imported symbol.
 * @return false with \a aError set when the part could not be imported.
 */
bool ImportJlcPart( SCH_BASE_FRAME* aFrame, const wxString& aPartNumber, LIB_ID& aSymbolId,
                    wxString& aError );

#endif // JLC_PART_IMPORT_H
