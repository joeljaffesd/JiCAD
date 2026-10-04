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

#include "jlc_part_import.h"

#include <io/io_mgr.h>
#include <json_common.h>
#include <ki_exception.h>
#include <kicad_curl/kicad_curl_easy.h>
#include <kiface_base.h>
#include <kiface_ids.h>
#include <kiway.h>
#include <lib_symbol.h>
#include <libraries/library_manager.h>
#include <libraries/library_table.h>
#include <libraries/symbol_library_adapter.h>
#include <pgm_base.h>
#include <project.h>
#include <project_sch.h>
#include <sch_base_frame.h>
#include <sch_field.h>
#include <sch_io/sch_io.h>
#include <sch_io/sch_io_mgr.h>
#include <string_utils.h>

#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/regex.h>


static const wxString JLC_LIBRARY = wxS( "JLC" );

/// The folder beside the project file that holds the imported libraries and models
static const wxString JLC_PARTS_DIR = wxS( "JLC-PARTS" );
static const wxString JLC_PARTS_URI = wxS( "${KIPRJMOD}/JLC-PARTS/" );

// EasyEDA's CDN rejects requests that do not look like they come from a browser
static const char* JLC_USER_AGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 "
                                    "(KHTML, like Gecko) Chrome/126.0 Safari/537.36";

static const wxString JLC_PART_URL = wxS( "https://easyeda.com/api/products/%s/components?version=6.4.19.5" );
static const wxString JLC_STEP_URL = wxS( "https://modules.easyeda.com/qAxj6KHrDKw4blvCG8QJPs7Y/%s" );
static const wxString JLC_MESH_URL = wxS( "https://modules.easyeda.com/3dmodel/%s" );
static const wxString JLC_DATASHEET_URL = wxS( "https://www.lcsc.com/datasheet/%s.pdf" );


wxString JlcPartNumberFromText( const wxString& aText )
{
    static const wxRegEx partNumber( wxS( "^[Cc][0-9]{2,}$" ) );

    wxString text = aText;
    text.Trim( true ).Trim( false );

    if( partNumber.Matches( text ) )
        return text.Upper();

    return wxEmptyString;
}


static bool httpGet( const wxString& aUrl, std::string& aBody, wxString& aError )
{
    KICAD_CURL_EASY curl;

    curl.SetURL( aUrl.ToStdString() );
    curl.SetUserAgent( JLC_USER_AGENT );
    curl.SetFollowRedirects( true );
    curl.SetConnectTimeout( 15 );
    curl.SetStallTimeout( 1, 30 );

    int result = curl.Perform();

    if( result != 0 )
    {
        aError = wxString::Format( _( "Could not reach EasyEDA: %s" ), curl.GetErrorText( result ) );
        return false;
    }

    int status = curl.GetResponseStatusCode();

    if( status == 403 )
    {
        aError = _( "EasyEDA refused the request (HTTP 403).  It limits how often an address may "
                    "ask for parts; wait a few minutes and try again." );
        return false;
    }

    if( status != 200 )
    {
        aError = wxString::Format( _( "EasyEDA answered with HTTP %d." ), status );
        return false;
    }

    aBody = curl.GetBuffer();
    return true;
}


static bool writeFile( const wxFileName& aFile, const std::string& aData, wxString& aError )
{
    wxFileName dir = aFile;
    dir.SetFullName( wxEmptyString );

    if( !dir.DirExists() && !dir.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL ) )
    {
        aError = wxString::Format( _( "Unable to create '%s'." ), dir.GetFullPath() );
        return false;
    }

    wxFFile file( aFile.GetFullPath(), wxS( "wb" ) );

    if( !file.IsOpened() || file.Write( aData.data(), aData.size() ) != aData.size() )
    {
        aError = wxString::Format( _( "Unable to write '%s'." ), aFile.GetFullPath() );
        return false;
    }

    return true;
}


/// Add the project library row for one of the JLC libraries, unless it is already there.
static bool ensureProjectLibrary( LIBRARY_TABLE_TYPE aType, const wxString& aUri, wxString& aError )
{
    std::optional<LIBRARY_TABLE*> tableOpt = Pgm().GetLibraryManager().Table( aType, LIBRARY_TABLE_SCOPE::PROJECT );

    if( !tableOpt )
    {
        aError = _( "Unable to access the project library table." );
        return false;
    }

    LIBRARY_TABLE* table = *tableOpt;

    if( table->HasRow( JLC_LIBRARY ) )
    {
        std::optional<LIBRARY_TABLE_ROW*> row = table->Row( JLC_LIBRARY );

        if( row && ( *row )->URI() != aUri )
        {
            aError = wxString::Format( _( "The project already has a library named '%s' at '%s'." ),
                                       JLC_LIBRARY, ( *row )->URI() );
            return false;
        }

        return true;
    }

    LIBRARY_TABLE_ROW& row = table->InsertRow();
    row.SetNickname( JLC_LIBRARY );
    row.SetURI( aUri );
    row.SetType( wxS( "KiCad" ) );
    row.SetOptions( wxString() );
    row.SetDescription( _( "Parts imported from JLCPCB" ) );
    row.SetOk( true );

    if( !table->Save().has_value() )
    {
        aError = _( "Failed to save the project library table." );
        return false;
    }

    return true;
}


/// Give the reference and value somewhere to sit; EasyEDA symbols leave both on the origin.
static void placeFields( LIB_SYMBOL& aSymbol )
{
    BOX2I body = aSymbol.GetBodyBoundingBox( 1, 0, true, false );
    int   gap = schIUScale.MilsToIU( 50 );

    SCH_FIELD& reference = aSymbol.GetReferenceField();
    reference.SetPosition( VECTOR2I( body.GetLeft(), body.GetTop() - gap ) );
    reference.SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );

    SCH_FIELD& value = aSymbol.GetValueField();
    value.SetPosition( VECTOR2I( body.GetLeft(), body.GetBottom() + gap ) );
    value.SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
}


static void setHiddenField( LIB_SYMBOL& aSymbol, const wxString& aName, const wxString& aText )
{
    SCH_FIELD* field = aSymbol.FindFieldCaseInsensitive( aName );

    if( !field )
    {
        field = new SCH_FIELD( &aSymbol, FIELD_T::USER, aName );
        aSymbol.AddField( field );
    }

    field->SetText( aText );
    field->SetVisible( false );
}


bool ImportJlcPart( SCH_BASE_FRAME* aFrame, const wxString& aPartNumber, LIB_ID& aSymbolId, wxString& aError )
{
    PROJECT& project = aFrame->Prj();

    if( project.IsNullProject() || project.GetProjectPath().IsEmpty() )
    {
        aError = _( "Parts are imported into the project's libraries.  Open or save a project first." );
        return false;
    }

    wxFileName partsDir = wxFileName::DirName( project.GetProjectPath() );
    partsDir.AppendDir( JLC_PARTS_DIR );

    if( !partsDir.DirExists() && !partsDir.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL ) )
    {
        aError = wxString::Format( _( "Unable to create '%s'." ), partsDir.GetPath() );
        return false;
    }

    const wxString partsPath = partsDir.GetPath();

    // --- Fetch the part ---
    std::string body;

    if( !httpGet( wxString::Format( JLC_PART_URL, aPartNumber ), body, aError ) )
        return false;

    nlohmann::json doc = nlohmann::json::parse( body, nullptr, false );

    if( doc.is_discarded() || !doc.is_object() || !doc.value( "success", false ) || !doc.contains( "result" )
        || !doc.at( "result" ).is_object() || !doc.at( "result" ).contains( "dataStr" ) )
    {
        aError = wxString::Format( _( "EasyEDA has no symbol for %s." ), aPartNumber );
        return false;
    }

    nlohmann::json& part = doc.at( "result" );

    if( !part.contains( "packageDetail" ) || !part.at( "packageDetail" ).is_object()
        || !part.at( "packageDetail" ).contains( "dataStr" ) )
    {
        aError = wxString::Format( _( "EasyEDA has no footprint for %s." ), aPartNumber );
        return false;
    }

    nlohmann::json footprintDoc = part.at( "packageDetail" ).at( "dataStr" );
    part.erase( "packageDetail" );

    // The footprint names its 3D model in an SVGNODE shape
    wxString modelUuid;
    wxString modelTitle;

    try
    {
        for( const nlohmann::json& shape : footprintDoc.at( "shape" ) )
        {
            std::string text = shape.get<std::string>();

            if( text.rfind( "SVGNODE~", 0 ) != 0 )
                continue;

            nlohmann::json node = nlohmann::json::parse( text.substr( 8 ), nullptr, false );

            if( node.is_discarded() || !node.contains( "attrs" ) )
                continue;

            const nlohmann::json& attrs = node.at( "attrs" );

            if( attrs.value( "c_etype", "" ) == "outline3D" && attrs.contains( "uuid" ) && attrs.contains( "title" ) )
            {
                modelUuid = wxString::FromUTF8( attrs.at( "uuid" ).get<std::string>() );
                modelTitle = wxString::FromUTF8( attrs.at( "title" ).get<std::string>() );
                break;
            }
        }
    }
    catch( const nlohmann::json::exception& )
    {
        aError = wxString::Format( _( "The footprint data for %s could not be read." ), aPartNumber );
        return false;
    }

    // --- Stage the documents where the importers expect them ---
    wxString tempName = wxFileName::CreateTempFileName( wxS( "jlc" ) );

    if( tempName.IsEmpty() )
    {
        aError = _( "Unable to create a temporary folder." );
        return false;
    }

    wxRemoveFile( tempName );

    wxFileName tempDir = wxFileName::DirName( tempName );

    struct CLEANUP
    {
        wxFileName dir;
        ~CLEANUP() { wxFileName::Rmdir( dir.GetFullPath(), wxPATH_RMDIR_RECURSIVE ); }
    } cleanup{ tempDir };

    wxFileName symbolFile( tempDir.GetFullPath(), wxS( "symbol.json" ) );
    wxFileName footprintFile( tempDir.GetFullPath(), wxS( "footprint.json" ) );

    if( !writeFile( symbolFile, part.dump(), aError ) || !writeFile( footprintFile, footprintDoc.dump(), aError ) )
        return false;

    // --- 3D model: the STEP goes into the project; the mesh is only needed to place it ---
    if( !modelUuid.IsEmpty() )
    {
        const wxString modelName = EscapeString( modelTitle, CTX_FILENAME );

        wxFileName stepFile( partsPath, modelName + wxS( ".step" ) );
        stepFile.AppendDir( JLC_LIBRARY + wxS( ".3dshapes" ) );

        if( !stepFile.FileExists() )
        {
            std::string step;

            if( !httpGet( wxString::Format( JLC_STEP_URL, modelUuid ), step, aError )
                || !writeFile( stepFile, step, aError ) )
            {
                return false;
            }
        }

        wxFileName meshFile( tempDir.GetFullPath(), modelName + wxS( ".obj" ) );
        meshFile.AppendDir( wxS( "EASYEDA_MODELS" ) );

        std::string mesh;

        if( !httpGet( wxString::Format( JLC_MESH_URL, modelUuid ), mesh, aError )
            || !writeFile( meshFile, mesh, aError ) )
        {
            return false;
        }
    }

    // --- Footprint, converted by the PCB editor's importer ---
    typedef bool ( *IMPORT_FOOTPRINT )( const wxString&, const wxString&, const wxString&, wxString&, wxString& );

    IMPORT_FOOTPRINT importFootprint = nullptr;

    try
    {
        if( KIFACE* kiface = aFrame->Kiway().KiFACE( KIWAY::FACE_PCB ) )
        {
            importFootprint = reinterpret_cast<IMPORT_FOOTPRINT>(
                    kiface->IfaceOrAddress( KIFACE_IMPORT_EASYEDA_FOOTPRINT ) );
        }
    }
    catch( const IO_ERROR& )
    {
    }

    if( !importFootprint )
    {
        aError = _( "The PCB editor is needed to convert the footprint, and could not be loaded." );
        return false;
    }

    wxFileName footprintLib = wxFileName::DirName( partsPath );
    footprintLib.AppendDir( JLC_LIBRARY + wxS( ".pretty" ) );

    wxString footprintName;

    if( !importFootprint( footprintFile.GetFullPath(), footprintLib.GetPath(),
                          JLC_PARTS_URI + JLC_LIBRARY + wxS( ".3dshapes" ), footprintName, aError ) )
    {
        return false;
    }

    if( !ensureProjectLibrary( LIBRARY_TABLE_TYPE::FOOTPRINT,
                               JLC_PARTS_URI + JLC_LIBRARY + wxS( ".pretty" ), aError ) )
    {
        return false;
    }

    Pgm().GetLibraryManager().ReloadLibraryEntry( LIBRARY_TABLE_TYPE::FOOTPRINT, JLC_LIBRARY,
                                                  LIBRARY_TABLE_SCOPE::PROJECT );

    // --- Symbol ---
    std::unique_ptr<LIB_SYMBOL> symbol;

    try
    {
        IO_RELEASER<SCH_IO> plugin( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_EASYEDA ) );
        wxArrayString       names;

        if( plugin )
            plugin->EnumerateSymbolLib( names, symbolFile.GetFullPath() );

        if( !names.empty() )
            symbol.reset( plugin->LoadSymbol( symbolFile.GetFullPath(), names[0] ) );
    }
    catch( const IO_ERROR& e )
    {
        aError = e.What();
        return false;
    }

    if( !symbol )
    {
        aError = wxString::Format( _( "The symbol for %s could not be converted." ), aPartNumber );
        return false;
    }

    wxString symbolName = LIB_ID::FixIllegalChars( symbol->GetName(), false ).wx_str();

    aSymbolId.SetLibNickname( JLC_LIBRARY );
    aSymbolId.SetLibItemName( symbolName );

    symbol->SetName( symbolName );
    symbol->SetLibId( aSymbolId );
    symbol->SetFootprintProp( JLC_LIBRARY + wxS( ":" ) + footprintName );
    symbol->SetDatasheetProp( wxString::Format( JLC_DATASHEET_URL, aPartNumber ) );
    setHiddenField( *symbol, wxS( "LCSC" ), aPartNumber );
    placeFields( *symbol );

    SYMBOL_LIBRARY_ADAPTER* adapter = PROJECT_SCH::SymbolLibAdapter( &project );

    if( !adapter )
    {
        aError = _( "Unable to access the symbol library manager." );
        return false;
    }

    if( !ensureProjectLibrary( LIBRARY_TABLE_TYPE::SYMBOL,
                               JLC_PARTS_URI + JLC_LIBRARY + wxS( ".kicad_sym" ), aError ) )
    {
        return false;
    }

    wxFileName symbolLib( partsPath, JLC_LIBRARY + wxS( ".kicad_sym" ) );

    // The sexpr plugin refuses to save into a library file that does not exist yet
    if( !symbolLib.FileExists() && !adapter->CreateLibrary( JLC_LIBRARY ) )
    {
        aError = wxString::Format( _( "Unable to create '%s'." ), symbolLib.GetFullPath() );
        return false;
    }

    if( adapter->SaveSymbol( JLC_LIBRARY, std::move( symbol ), true ) != SYMBOL_LIBRARY_ADAPTER::SAVE_OK )
    {
        aError = _( "Unable to save the imported symbol." );
        return false;
    }

    Pgm().GetLibraryManager().ReloadLibraryEntry( LIBRARY_TABLE_TYPE::SYMBOL, JLC_LIBRARY,
                                                  LIBRARY_TABLE_SCOPE::PROJECT );

    return true;
}
