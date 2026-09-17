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

#include <pcad_vocabulary.h>

#include <map>
#include <cctype>
#include <utility>
#include <vector>


// Exact P-CAD 2006 menu names for the actions that have a direct counterpart.
static const std::map<std::string, wxString> s_pcadActionLabels =
        {
            // ---- PCB: Place menu ----
            { "pcbnew.EditorControl.placeFootprint",            wxS( "Component" ) },
            { "pcbnew.Control.localRatsnestTool",               wxS( "Connection" ) },
            { "pcbnew.InteractiveDrawing.via",                  wxS( "Via" ) },
            { "pcbnew.InteractiveDrawing.line",                 wxS( "Line" ) },
            { "pcbnew.InteractiveDrawing.arc",                  wxS( "Arc" ) },
            { "pcbnew.InteractiveDrawing.graphicPolygon",       wxS( "Polygon" ) },
            { "pcbnew.InteractiveDrawing.placePoint",           wxS( "Point" ) },
            { "pcbnew.InteractiveDrawing.zone",                 wxS( "Copper Pour" ) },
            { "pcbnew.InteractiveDrawing.zoneCutout",           wxS( "Cutout" ) },
            { "pcbnew.InteractiveDrawing.copperThievingZone",   wxS( "Copper Thieving Pour" ) },
            { "pcbnew.InteractiveDrawing.ruleArea",             wxS( "Keepout" ) },
            { "pcbnew.InteractiveDrawing.text",                 wxS( "Text" ) },
            // ---- PCB: Route menu ----
            { "pcbnew.InteractiveRouter.SingleTrack",           wxS( "Manual" ) },
            { "pcbnew.InteractiveRouter.DiffPair",              wxS( "Differential Pair" ) },
            { "pcbnew.InteractiveRouter.SettingsDialog",        wxS( "Route Options..." ) },
            { "pcbnew.InteractiveRouter.SelectLayerPair",       wxS( "Layer Pairs..." ) },
            // ---- PCB: Edit / Options / Utils ----
            { "pcbnew.Array.createArray",                       wxS( "Copy Matrix..." ) },
            { "pcbnew.InteractiveEdit.swap",                    wxS( "Swap Components" ) },
            { "pcbnew.EditorControl.lock",                      wxS( "Fix" ) },
            { "pcbnew.EditorControl.unlock",                    wxS( "Unfix" ) },
            { "pcbnew.EditorControl.toggleLock",                wxS( "Toggle Fix" ) },
            { "pcbnew.EditorControl.clearHighlight",            wxS( "Unhighlight All" ) },
            { "pcbnew.Control.showNetInspector",                wxS( "Nets..." ) },
            { "pcbnew.EditorControl.boardSetup",                wxS( "Design Rules..." ) },
            { "pcbnew.Control.showLayersManager",               wxS( "Layers..." ) },
            { "pcbnew.DRCTool.runDRC",                          wxS( "DRC..." ) },
            { "pcbnew.ReannotateTool.ShowReannotateDialog",     wxS( "Renumber Components..." ) },
            { "pcbnew.GlobalEdit.cleanupTracksAndVias",         wxS( "Trace Clean-up..." ) },
            { "pcbnew.GlobalEdit.updateFootprints",             wxS( "Force Update..." ) },
            { "pcbnew.EditorControl.importNetlist",             wxS( "Load Netlist..." ) },
            { "pcbnew.EditorControl.generateGerbers",           wxS( "Gerber..." ) },
            { "pcbnew.EditorControl.generateDrillFiles",        wxS( "N/C Drill..." ) },
            { "pcbnew.EditorControl.generatePosFile",           wxS( "Pick and Place..." ) },
            { "pcbnew.EditorControl.showEeschema",              wxS( "WiCAD Schematic..." ) },
            { "pcbnew.ZoneFiller.zoneFillAll",                  wxS( "Pour All Copper Pours" ) },
            { "pcbnew.ZoneFiller.zoneUnfillAll",                wxS( "Unpour All Copper Pours" ) },

            // ---- Schematic: Place menu ----
            { "eeschema.InteractiveDrawing.placeSymbol",              wxS( "Part" ) },
            { "eeschema.InteractiveDrawingLineWireBus.drawWires",     wxS( "Wire" ) },
            { "eeschema.InteractiveDrawingLineWireBus.drawBuses",     wxS( "Bus" ) },
            { "eeschema.InteractiveDrawing.placePowerSymbol",         wxS( "Port" ) },
            { "eeschema.InteractiveDrawingLineWireBus.drawLines",     wxS( "Line" ) },
            { "eeschema.InteractiveDrawing.drawArc",                  wxS( "Arc" ) },
            { "eeschema.InteractiveDrawing.drawPolygon",              wxS( "Polygon" ) },
            { "eeschema.InteractiveDrawing.placeSchematicText",       wxS( "Text" ) },
            // ---- Schematic: Options / Utils ----
            { "eeschema.EditorControl.annotate",                wxS( "Renumber..." ) },
            { "eeschema.InspectionTool.runERC",                 wxS( "ERC..." ) },
            { "eeschema.EditorControl.schematicSetup",          wxS( "Design Rules..." ) },
            { "eeschema.EditorControl.exportNetlist",           wxS( "Generate Netlist..." ) },
            { "eeschema.EditorControl.clearHighlight",          wxS( "Unhighlight All" ) },
            { "eeschema.EditorControl.showPcbNew",              wxS( "WiCAD PCB..." ) },
            { "eeschema.EditorTool.showHierarchy",              wxS( "Sheets" ) },
            { "eeschema.NavigateTool.enterSheet",               wxS( "Descend" ) },
            { "eeschema.NavigateTool.leaveSheet",               wxS( "Ascend" ) },
            { "eeschema.NavigateTool.up",                       wxS( "Ascend" ) },
            { "eeschema.InteractiveEdit.lock",                  wxS( "Fix" ) },
            { "eeschema.InteractiveEdit.unlock",                wxS( "Unfix" ) },

            // ---- both editors: View / Edit / Options ----
            { "common.Control.zoomRedraw",                      wxS( "Redraw" ) },
            { "common.Control.zoomFitObjects",                  wxS( "Extent" ) },
            { "common.Control.zoomFitScreen",                   wxS( "All" ) },
            { "common.Control.zoomCenter",                      wxS( "Center" ) },
            { "common.Control.zoomTool",                        wxS( "Zoom Window" ) },
            { "common.Control.zoomInCenter",                    wxS( "Zoom In" ) },
            { "common.Control.zoomOutCenter",                   wxS( "Zoom Out" ) },
            { "common.InteractiveSelection.selectionTool",      wxS( "Select" ) },
            { "common.Interactive.measureTool",                 wxS( "Measure" ) },
            { "common.Control.editGrids",                       wxS( "Grids..." ) },
            { "common.SuiteControl.openPreferences",            wxS( "Configure..." ) },
            { "common.Control.pageSettings",                    wxS( "Design Info..." ) },
            { "common.Control.showFootprintEditor",             wxS( "WiCAD Pattern Editor..." ) },
            { "common.Control.showSymbolEditor",                wxS( "WiCAD Symbol Editor..." ) },
            { "common.SuiteControl.showFootprintLibTable",      wxS( "Library Setup (Patterns)..." ) },
            { "common.SuiteControl.showSymbolLibTable",         wxS( "Library Setup (Symbols)..." ) }
        };


// KiCad term -> P-CAD term, applied in order to every other label and tooltip.  Longer phrases first.
static const std::vector<std::pair<wxString, wxString>> s_pcadWords =
        {
            { wxS( "Reference Designators" ), wxS( "RefDes" ) },
            { wxS( "Reference Designator" ),  wxS( "RefDes" ) },
            { wxS( "reference designators" ), wxS( "RefDes" ) },
            { wxS( "reference designator" ),  wxS( "RefDes" ) },
            { wxS( "Power Symbols" ),         wxS( "Ports" ) },
            { wxS( "Power Symbol" ),          wxS( "Port" ) },
            { wxS( "power symbols" ),         wxS( "ports" ) },
            { wxS( "power symbol" ),          wxS( "port" ) },
            { wxS( "Rule Areas" ),            wxS( "Keepouts" ) },
            { wxS( "Rule Area" ),             wxS( "Keepout" ) },
            { wxS( "rule areas" ),            wxS( "keepouts" ) },
            { wxS( "rule area" ),             wxS( "keepout" ) },
            { wxS( "Filled Zones" ),          wxS( "Copper Pours" ) },
            { wxS( "filled zones" ),          wxS( "copper pours" ) },
            { wxS( "Zones" ),                 wxS( "Copper Pours" ) },
            { wxS( "Zone(s)" ),               wxS( "Copper Pour(s)" ) },
            { wxS( "Zone" ),                  wxS( "Copper Pour" ) },
            { wxS( "zones" ),                 wxS( "copper pours" ) },
            { wxS( "zone" ),                  wxS( "copper pour" ) },
            { wxS( "Footprints" ),            wxS( "Patterns" ) },
            { wxS( "Footprint" ),             wxS( "Pattern" ) },
            { wxS( "footprints" ),            wxS( "patterns" ) },
            { wxS( "footprint" ),             wxS( "pattern" ) },
            { wxS( "Ratsnest" ),              wxS( "Connections" ) },
            { wxS( "ratsnest" ),              wxS( "connections" ) },
            { wxS( "Tracks" ),                wxS( "Traces" ) },
            { wxS( "Track(s)" ),              wxS( "Trace(s)" ) },
            { wxS( "Track" ),                 wxS( "Trace" ) },
            { wxS( "tracks" ),                wxS( "traces" ) },
            { wxS( "track" ),                 wxS( "trace" ) },
            { wxS( "Reannotate" ),            wxS( "Renumber" ) },
            { wxS( "Annotations" ),           wxS( "Numbering" ) },
            { wxS( "Annotation" ),            wxS( "Numbering" ) },
            { wxS( "Annotate" ),              wxS( "Renumber" ) },
            { wxS( "annotate" ),              wxS( "renumber" ) },
            { wxS( "Unlock" ),                wxS( "Unfix" ) },
            { wxS( "unlock" ),                wxS( "unfix" ) },
            { wxS( "Locked" ),                wxS( "Fixed" ) },
            { wxS( "locked" ),                wxS( "fixed" ) },
            { wxS( "Lock" ),                  wxS( "Fix" ) },
            { wxS( "lock" ),                  wxS( "fix" ) }
        };


// P-CAD 2006 prompt-line strings (PCadRsc.dll / schrsc.dll string tables) for a tool's first state.
static const wxString s_selectPrompt =
        wxS( "Click <Left> to single Select, <Ctrl><Left> for multiple, or drag for block select." );

static const std::map<std::string, wxString> s_pcadPrompts =
        {
            { "common.InteractiveSelection.selectionTool",        s_selectPrompt },
            { "common.Interactive.measureTool",                   wxS( "Press <Left> or <Space> over the starting point to measure." ) },
            { "common.Control.zoomTool",                          wxS( "Press <Left> or <Space> at one corner of the zoom window." ) },
            { "common.Control.zoomCenter",                        wxS( "Press <Left> or <Space> at the point to be centered on the screen." ) },
            { "common.Control.gridSetOrigin",                     wxS( "Click <Left> or press <Space> to set relative grid origin." ) },

            { "pcbnew.EditorControl.placeFootprint",              wxS( "Press and release <Left> or <Space><Space> to select a component." ) },
            { "pcbnew.Control.localRatsnestTool",                 wxS( "Press <Left> or <Space> over a pad at the connection starting point." ) },
            { "pcbnew.InteractiveDrawing.via",                    wxS( "Press <Left> or <Space> to show the outline of the current via." ) },
            { "pcbnew.InteractiveDrawing.line",                   wxS( "Press <Left> or <Space> at the line starting point." ) },
            { "pcbnew.InteractiveDrawing.arc",                    wxS( "Press <Left> or <Space> at the first arc point." ) },
            { "pcbnew.InteractiveDrawing.graphicPolygon",         wxS( "Press <Left> at the polygon starting point or next vertex, <Right> to complete." ) },
            { "pcbnew.InteractiveDrawing.zone",                   wxS( "Press <Left> at the polygon starting point or next vertex, <Right> to complete." ) },
            { "pcbnew.InteractiveDrawing.zoneCutout",             wxS( "Press <Left> at the polygon starting point or next vertex, <Right> to complete." ) },
            { "pcbnew.InteractiveDrawing.ruleArea",               wxS( "Press <Left> at the polygon starting point or next vertex, <Right> to complete." ) },
            { "pcbnew.InteractiveDrawing.placePoint",             wxS( "Press <Left> or <Space> to show the outline of the point object." ) },
            { "pcbnew.InteractiveDrawing.text",                   wxS( "Press and release <Left> or <Space><Space> to define text location." ) },
            { "pcbnew.InteractiveDrawing.table",                  wxS( "Press and release <Left> or <Space><Space> to define table." ) },
            { "pcbnew.InteractiveDrawing.alignedDimension",       wxS( "Press <Left> or <Space> <Space> to select the first object for the baseline/datum or point to point dimension." ) },
            { "pcbnew.InteractiveDrawing.orthogonalDimension",    wxS( "Press <Left> or <Space> <Space> to select the first object for the baseline/datum or point to point dimension." ) },
            { "pcbnew.InteractiveDrawing.radialDimension",        wxS( "Press <Left> or <Space> <Space> to select an object for the radius or diameter dimension." ) },
            { "pcbnew.InteractiveDrawing.centerDimension",        wxS( "Press <Left> and Hold, or <Space> to select a circular object to place the center mark over." ) },
            { "pcbnew.InteractiveDrawing.leader",                 wxS( "Press <Left> or <Space> <Space> to set the location of the leader arrow." ) },
            { "pcbnew.InteractiveRouter.SingleTrack",             wxS( "Press <Left> or <Space> over a net segment to begin routing." ) },
            { "pcbnew.InteractiveRouter.DiffPair",                wxS( "Press <Left> or <Space> over a net segment to begin routing." ) },
            { "pcbnew.InteractiveMove.move",                      wxS( "<R> to rotate, <F> to flip, or Release <Left> at the new location for the selected items." ) },

            { "eeschema.InteractiveDrawing.placeSymbol",          wxS( "Press and Release <Left> or <Space><Space> to place part." ) },
            { "eeschema.InteractiveDrawing.placePowerSymbol",     wxS( "Click <Left> or <Space><Space> on Net to set Port's net name. Click in clear area to open Port Property dialog" ) },
            { "eeschema.InteractiveDrawingLineWireBus.drawWires", wxS( "Press <Left> or <Space> at the wire starting point." ) },
            { "eeschema.InteractiveDrawingLineWireBus.drawBuses", wxS( "Press <Left> or <Space> at the bus starting point." ) },
            { "eeschema.InteractiveDrawingLineWireBus.drawLines", wxS( "Press <Left> or <Space> at the line starting point." ) },
            { "eeschema.InteractiveDrawing.drawArc",              wxS( "Press <Left> or <Space> at the first arc point." ) },
            { "eeschema.InteractiveDrawing.drawPolygon",          wxS( "Press <Left> at the polygon starting point or next vertex, <Right> to complete." ) },
            { "eeschema.InteractiveDrawing.placeSchematicText",   wxS( "Press and release <Left> or <Space><Space> to define text location." ) },
            { "eeschema.InteractiveDrawing.drawTable",            wxS( "Press and release <Left> or <Space><Space> to define table." ) },
            { "eeschema.SymbolDrawing.placeSymbolPin",            wxS( "Press and Release <Left> or <Space><Space> to place pin." ) },
            { "eeschema.InteractiveMove.move",                    wxS( "Press <R>otate, <F>lip, <Space> or Release <Left> at the new location for the selected part." ) }
        };


// Replace whole words only, so that "Lock" -> "Fix" does not turn "Block" into "BFix".
static void replaceWord( wxString& aText, const wxString& aFrom, const wxString& aTo )
{
    size_t pos = 0;

    while( ( pos = aText.find( aFrom, pos ) ) != wxString::npos )
    {
        size_t end = pos + aFrom.length();
        bool   startOk = pos == 0 || !wxIsalpha( aText[pos - 1] );
        bool   endOk = end >= aText.length() || !wxIsalpha( aText[end] );

        if( startOk && endOk )
        {
            aText.replace( pos, aFrom.length(), aTo );
            pos += aTo.length();
        }
        else
        {
            pos = end;
        }
    }
}


wxString PCadWording( const wxString& aText )
{
    static std::map<wxString, wxString> cache;

    auto it = cache.find( aText );

    if( it != cache.end() )
        return it->second;

    wxString result = aText;

    for( const std::pair<wxString, wxString>& word : s_pcadWords )
        replaceWord( result, word.first, word.second );

    cache[aText] = result;
    return result;
}


wxString PCadLabel( const std::string& aActionName, const wxString& aText, bool aIsLabel )
{
    if( aIsLabel )
    {
        auto it = s_pcadActionLabels.find( aActionName );

        if( it != s_pcadActionLabels.end() )
            return it->second;
    }

    return PCadWording( aText );
}


wxString PCadPrompt( const std::string& aActionName )
{
    auto it = s_pcadPrompts.find( aActionName );

    return it != s_pcadPrompts.end() ? it->second : s_selectPrompt;
}
