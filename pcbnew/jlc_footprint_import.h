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

#ifndef JLC_FOOTPRINT_IMPORT_H
#define JLC_FOOTPRINT_IMPORT_H

#include <wx/string.h>

/**
 * Convert an EasyEDA footprint document into a footprint of a KiCad library.
 *
 * Exported to the schematic editor through KIFACE_IMPORT_EASYEDA_FOOTPRINT.
 *
 * @param aSourceFile is the EasyEDA footprint JSON.  The OBJ mesh of its 3D model, if any, is
 *                    expected in an EASYEDA_MODELS folder beside it so the model can be placed.
 * @param aLibraryPath is the .pretty folder to save into; it is created when missing.
 * @param aModelDirUri is where the footprint should look for its 3D model, for example
 *                     "${KIPRJMOD}/JLC-PARTS/JLC.3dshapes".
 * @param aFootprintName receives the name the footprint was saved under.
 */
bool ImportEasyedaFootprint( const wxString& aSourceFile, const wxString& aLibraryPath,
                             const wxString& aModelDirUri, wxString& aFootprintName, wxString& aError );

#endif // JLC_FOOTPRINT_IMPORT_H
