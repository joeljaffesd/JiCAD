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

#include "jlc_footprint_import.h"

#include <footprint.h>
#include <io/io_mgr.h>
#include <ki_exception.h>
#include <pcb_io/pcb_io.h>
#include <pcb_io/pcb_io_mgr.h>
#include <zone.h>

#include <wx/dir.h>
#include <wx/intl.h>


bool ImportEasyedaFootprint( const wxString& aSourceFile, const wxString& aLibraryPath,
                             const wxString& aModelDirUri, wxString& aFootprintName, wxString& aError )
{
    try
    {
        IO_RELEASER<PCB_IO> source( PCB_IO_MGR::FindPlugin( PCB_IO_MGR::EASYEDA ) );
        IO_RELEASER<PCB_IO> target( PCB_IO_MGR::FindPlugin( PCB_IO_MGR::KICAD_SEXP ) );

        if( !source || !target )
        {
            aError = _( "Unable to access the footprint plugins." );
            return false;
        }

        wxArrayString names;
        source->FootprintEnumerate( names, aSourceFile, false );

        if( names.empty() )
        {
            aError = _( "The part has no footprint." );
            return false;
        }

        std::unique_ptr<FOOTPRINT> footprint = source->FootprintLoad( aSourceFile, names[0] );

        if( !footprint )
        {
            aError = _( "Unable to convert the footprint." );
            return false;
        }

        // EasyEDA draws paste apertures and documentation shapes as solid regions, which arrive
        // as zones.  The pads already carry their own paste.
        ZONES zones = footprint->Zones();

        for( ZONE* zone : zones )
        {
            footprint->Remove( zone );
            delete zone;
        }

        for( FP_3DMODEL& model : footprint->Models() )
            model.m_Filename = aModelDirUri + wxS( "/" ) + model.m_Filename.AfterLast( '/' );

        if( !wxDir::Exists( aLibraryPath ) )
            target->CreateLibrary( aLibraryPath );

        target->FootprintSave( aLibraryPath, footprint.get() );

        aFootprintName = footprint->GetFPID().GetUniStringLibItemName();
        return true;
    }
    catch( const IO_ERROR& e )
    {
        aError = e.What();
        return false;
    }
}
