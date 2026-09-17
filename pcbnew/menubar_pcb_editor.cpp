/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2012 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <pcad_mode.h>
#include <advanced_config.h>
#include <bitmaps.h>
#include <file_history.h>
#include <kiface_base.h>
#include <pcb_edit_frame.h>
#include <pcbnew_id.h>
#include <pgm_base.h>
#include <settings/common_settings.h>
#include <tool/action_manager.h>
#include <tool/actions.h>
#include <tool/tool_manager.h>
#include <tools/pcb_actions.h>
#include <tools/pcb_selection_tool.h>
#include <widgets/wx_menubar.h>


void PCB_EDIT_FRAME::doReCreateMenuBar()
{
    PCB_SELECTION_TOOL* selTool = m_toolManager->GetTool<PCB_SELECTION_TOOL>();
    // wxWidgets handles the Mac Application menu behind the scenes, but that means
    // we always have to start from scratch with a new wxMenuBar.
    wxMenuBar*  oldMenuBar = GetMenuBar();
    WX_MENUBAR* menuBar    = new WX_MENUBAR();

    // Recreate all menus:

    //-- File menu -----------------------------------------------------------
    //
    ACTION_MENU*   fileMenu = new ACTION_MENU( false, selTool );
    static ACTION_MENU* openRecentMenu;

    if( Kiface().IsSingle() )   // not when under a project mgr
    {
        FILE_HISTORY& fileHistory = GetFileHistory();

        // Create the menu if it does not exist. Adding a file to/from the history
        // will automatically refresh the menu.
        if( !openRecentMenu )
        {
            openRecentMenu = new ACTION_MENU( false, selTool );
            openRecentMenu->SetIcon( BITMAPS::recent );

            fileHistory.UseMenu( openRecentMenu );
            fileHistory.AddFilesToMenu();
        }

        // Ensure the title is up to date after changing language
        openRecentMenu->SetTitle( _( "Open Recent" ) );
        fileHistory.UpdateClearText( openRecentMenu, _( "Clear Recent Files" ) );

        fileMenu->Add( ACTIONS::doNew );
        fileMenu->Add( ACTIONS::open );

        wxMenuItem* item = fileMenu->Add( openRecentMenu->Clone() );

        // Add the file menu condition here since it needs the item ID for the submenu
        ACTION_CONDITIONS cond;
        cond.Enable( FILE_HISTORY::FileHistoryNotEmpty( fileHistory ) );
        RegisterUIUpdateHandler( item->GetId(), cond );
    }

    fileMenu->Add( PCB_ACTIONS::appendBoard );
    fileMenu->Add( PCB_ACTIONS::compareBoardWithFile );

    wxMenuItem*       historyItem = fileMenu->Add( PCB_ACTIONS::compareBoardWithHistory );
    ACTION_CONDITIONS historyCond;
    historyCond.Enable(
            []( const SELECTION& )
            {
                COMMON_SETTINGS* cs = Pgm().GetCommonSettings();
                return cs && cs->AutosaveUsesLocalHistory();
            } );
    RegisterUIUpdateHandler( historyItem->GetId(), historyCond );

    fileMenu->AppendSeparator();

    fileMenu->Add( ACTIONS::save );

    // Save as menu:
    // under a project mgr we do not want to modify the board filename
    // to keep consistency with the project mgr which expects files names same as prj name
    // for main files
    if( Kiface().IsSingle() )
        fileMenu->Add( ACTIONS::saveAs );
    else
        fileMenu->Add( ACTIONS::saveCopy );

    fileMenu->Add( ACTIONS::revert );

    fileMenu->AppendSeparator();
    fileMenu->Add( PCB_ACTIONS::rescueAutosave );

    // Import submenu
    ACTION_MENU* submenuImport = new ACTION_MENU( false, selTool );
    submenuImport->SetTitle( _( "Import" ) );
    submenuImport->SetIcon( BITMAPS::import );

    submenuImport->Add( PCB_ACTIONS::importNetlist,          ACTION_MENU::NORMAL, _( "Netlist..." ) );
    submenuImport->Add( PCB_ACTIONS::importSpecctraSession,  ACTION_MENU::NORMAL, _( "Specctra Session..." ) );
    submenuImport->Add( PCB_ACTIONS::placeImportedGraphics,  ACTION_MENU::NORMAL, _( "Graphics..." ) );
    submenuImport->Add( PCB_ACTIONS::openNonKicadBoard );

    fileMenu->AppendSeparator();
    fileMenu->Add( submenuImport );

    // Export submenu
    ACTION_MENU* submenuExport = new ACTION_MENU( false, selTool );
    submenuExport->SetTitle( _( "Export" ) );
    submenuExport->SetIcon( BITMAPS::export_file );

    submenuExport->Add( PCB_ACTIONS::exportSpecctraDSN, ACTION_MENU::NORMAL, _( "Specctra DSN..." ) );
    submenuExport->Add( PCB_ACTIONS::exportGenCAD,      ACTION_MENU::NORMAL, _( "GenCAD..." ) );
    submenuExport->Add( PCB_ACTIONS::exportVRML,        ACTION_MENU::NORMAL, _( "VRML..." ) );
    submenuExport->Add( PCB_ACTIONS::exportIDF,         ACTION_MENU::NORMAL, _( "IDFv3..." ) );
    submenuExport->Add( PCB_ACTIONS::exportSTEP,        ACTION_MENU::NORMAL, _( "STEP/GLB/BREP/XAO/PLY/STL..." ) );
    submenuExport->Add( PCB_ACTIONS::exportCmpFile,     ACTION_MENU::NORMAL, _( "Footprint Association (.cmp) File..." ) );
    submenuExport->Add( PCB_ACTIONS::exportHyperlynx,   ACTION_MENU::NORMAL, _( "Hyperlynx..." ) );

    if( ADVANCED_CFG::GetCfg().m_ShowPcbnewExportNetlist && m_exportNetlistAction )
        submenuExport->Add( *m_exportNetlistAction );

    submenuExport->AppendSeparator();
    submenuExport->Add( PCB_ACTIONS::exportFootprints,  ACTION_MENU::NORMAL, _( "Footprints..." ) );

    fileMenu->Add( submenuExport );

    // Fabrication Outputs submenu
    ACTION_MENU* submenuFabOutputs = new ACTION_MENU( false, selTool );
    submenuFabOutputs->SetTitle( _( "Fabrication Outputs" ) );
    submenuFabOutputs->SetIcon( BITMAPS::fabrication );

    submenuFabOutputs->Add( PCB_ACTIONS::generateGerbers );
    submenuFabOutputs->Add( PCB_ACTIONS::generateDrillFiles );
    submenuFabOutputs->Add( PCB_ACTIONS::generateIPC2581File );
    submenuFabOutputs->Add( PCB_ACTIONS::generateODBPPFile );

    submenuFabOutputs->Add( PCB_ACTIONS::generatePosFile );
    submenuFabOutputs->Add( PCB_ACTIONS::generateReportFile );
    submenuFabOutputs->Add( PCB_ACTIONS::generateD356File );
    submenuFabOutputs->Add( PCB_ACTIONS::generateBOM );
    submenuFabOutputs->Add( PCB_ACTIONS::generateBOMLegacy );
    fileMenu->Add( submenuFabOutputs );

    fileMenu->AppendSeparator();
    fileMenu->Add( PCB_ACTIONS::boardSetup );

    fileMenu->AppendSeparator();
    fileMenu->Add( ACTIONS::pageSettings );
    fileMenu->Add( ACTIONS::print );
    fileMenu->Add( ACTIONS::plot );

    fileMenu->AppendSeparator();
    fileMenu->AddQuitOrClose( &Kiface(), _( "PCB Editor" ) );

    //-- Edit menu -----------------------------------------------------------
    //
    ACTION_MENU* editMenu = new ACTION_MENU( false, selTool );

    editMenu->Add( ACTIONS::undo );
    editMenu->Add( ACTIONS::redo );

    editMenu->AppendSeparator();
    editMenu->Add( ACTIONS::cut );
    editMenu->Add( ACTIONS::copy );
    editMenu->Add( ACTIONS::paste );
    editMenu->Add( ACTIONS::pasteSpecial );
    editMenu->Add( ACTIONS::doDelete );

    editMenu->AppendSeparator();

    // Select Submenu
    ACTION_MENU* selectSubMenu = new ACTION_MENU( false, selTool );
    selectSubMenu->SetTitle( _( "&Select" ) );

    selectSubMenu->Add( ACTIONS::selectAll );
    selectSubMenu->Add( ACTIONS::unselectAll );

    editMenu->Add( selectSubMenu );

    editMenu->AppendSeparator();
    editMenu->Add( ACTIONS::find );
    editMenu->Add( PCB_ACTIONS::findByProperties );

    editMenu->AppendSeparator();
    editMenu->Add( PCB_ACTIONS::extendGraphic );
    editMenu->Add( PCB_ACTIONS::trimGraphic );
    editMenu->Add( PCB_ACTIONS::editTracksAndVias );
    editMenu->Add( PCB_ACTIONS::editTextAndGraphics );
    editMenu->Add( PCB_ACTIONS::editTeardrops );
    editMenu->Add( PCB_ACTIONS::changeFootprints );
    editMenu->Add( PCB_ACTIONS::swapLayers );
    editMenu->Add( ACTIONS::gridOrigin );

    editMenu->AppendSeparator();
    editMenu->Add( PCB_ACTIONS::zoneFillAll );
    editMenu->Add( PCB_ACTIONS::zoneUnfillAll );
    editMenu->Add( PCB_ACTIONS::regenerateAllTuning );

    editMenu->AppendSeparator();
    editMenu->Add( ACTIONS::deleteTool );
    editMenu->Add( PCB_ACTIONS::globalDeletions );


    //----- View menu -----------------------------------------------------------
    //
    ACTION_MENU* viewMenu = new ACTION_MENU( false, selTool );

    ACTION_MENU* showHidePanels = new ACTION_MENU( false, selTool );
    showHidePanels->SetTitle( _( "Panels" ) );
    showHidePanels->Add( ACTIONS::showProperties,                 ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showSearch,                 ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showLayersManager,          ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showNetInspector,           ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showConstraintsPanel,       ACTION_MENU::CHECK );

    if( ADVANCED_CFG::GetCfg().m_EnablePcbDesignBlocks )
        showHidePanels->Add( PCB_ACTIONS::showDesignBlockPanel, ACTION_MENU::CHECK, _( "Design Blocks" ) );

    viewMenu->Add( showHidePanels );

    viewMenu->AppendSeparator();
    viewMenu->Add( ACTIONS::showFootprintBrowser );
    viewMenu->Add( ACTIONS::show3DViewer );

    viewMenu->AppendSeparator();
    viewMenu->Add( ACTIONS::zoomInCenter );
    viewMenu->Add( ACTIONS::zoomOutCenter );
    viewMenu->Add( ACTIONS::zoomFitScreen );
    viewMenu->Add( ACTIONS::zoomFitObjects );
    viewMenu->Add( ACTIONS::zoomFitSelection );
    viewMenu->Add( ACTIONS::zoomTool );
    viewMenu->Add( ACTIONS::zoomRedraw );

    viewMenu->AppendSeparator();
    // Drawing Mode Submenu
    ACTION_MENU* drawingModeSubMenu = new ACTION_MENU( false, selTool );
    drawingModeSubMenu->SetTitle( _( "&Drawing Mode" ) );
    drawingModeSubMenu->SetIcon( BITMAPS::add_zone );

    drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayFilled,   ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayOutline,  ACTION_MENU::CHECK );

    if( ADVANCED_CFG::GetCfg().m_ExtraZoneDisplayModes )
    {
        drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayFractured,    ACTION_MENU::CHECK );
        drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayTriangulated, ACTION_MENU::CHECK );
    }

    drawingModeSubMenu->AppendSeparator();
    drawingModeSubMenu->Add( PCB_ACTIONS::padDisplayMode,      ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::viaDisplayMode,      ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::trackDisplayMode,    ACTION_MENU::CHECK );

    drawingModeSubMenu->AppendSeparator();
    drawingModeSubMenu->Add( PCB_ACTIONS::graphicsOutlines,    ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::textOutlines,        ACTION_MENU::CHECK );

    viewMenu->Add( drawingModeSubMenu );

    // Contrast Mode Submenu
    ACTION_MENU* contrastModeSubMenu = new ACTION_MENU( false, selTool );
    contrastModeSubMenu->SetTitle( _( "&Contrast Mode" ) );
    contrastModeSubMenu->SetIcon( BITMAPS::contrast_mode );

    contrastModeSubMenu->Add( ACTIONS::highContrastMode,    ACTION_MENU::CHECK );
    contrastModeSubMenu->Add( PCB_ACTIONS::layerAlphaDec );
    contrastModeSubMenu->Add( PCB_ACTIONS::layerAlphaInc );
    viewMenu->Add( contrastModeSubMenu );

    viewMenu->Add( PCB_ACTIONS::flipBoard,                  ACTION_MENU::CHECK );

#ifdef __APPLE__
    viewMenu->AppendSeparator();
#endif

    //-- Place Menu ----------------------------------------------------------
    //
    ACTION_MENU* placeMenu = new ACTION_MENU( false, selTool );

    placeMenu->Add( PCB_ACTIONS::placeFootprint );
    placeMenu->Add( PCB_ACTIONS::drawVia );
    placeMenu->Add( PCB_ACTIONS::placeViaStack );
    placeMenu->Add( PCB_ACTIONS::makeViaStack );
    placeMenu->Add( PCB_ACTIONS::drawZone );
    placeMenu->Add( PCB_ACTIONS::drawCopperThievingZone );
    placeMenu->Add( PCB_ACTIONS::drawRuleArea );

    ACTION_MENU* muwaveSubmenu = new ACTION_MENU( false, selTool );
    muwaveSubmenu->SetTitle( _( "Draw Microwave Shapes" ) );
    muwaveSubmenu->SetIcon( BITMAPS::mw_add_line );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateLine );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateGap );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateStub );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateStubArc );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateFunctionShape );
    placeMenu->Add( muwaveSubmenu );

    placeMenu->AppendSeparator();
    placeMenu->Add( PCB_ACTIONS::drawLine );
    placeMenu->Add( PCB_ACTIONS::drawArc );
    placeMenu->Add( PCB_ACTIONS::drawRectangle );
    placeMenu->Add( PCB_ACTIONS::drawCircle );
    placeMenu->Add( PCB_ACTIONS::drawEllipse );
    placeMenu->Add( PCB_ACTIONS::drawEllipseArc );
    placeMenu->Add( PCB_ACTIONS::drawPolygon );
    placeMenu->Add( PCB_ACTIONS::drawBezier );
    placeMenu->Add( PCB_ACTIONS::placeReferenceImage );
    placeMenu->Add( PCB_ACTIONS::placeText );
    placeMenu->Add( PCB_ACTIONS::drawTextBox );
    placeMenu->Add( PCB_ACTIONS::drawTable );
    placeMenu->Add( PCB_ACTIONS::placePoint );
    placeMenu->Add( PCB_ACTIONS::placeBarcode );

    placeMenu->AppendSeparator();
    ACTION_MENU* dimensionSubmenu = new ACTION_MENU( false, selTool );
    dimensionSubmenu->SetTitle( _( "Draw Dimensions" ) );
    dimensionSubmenu->SetIcon( BITMAPS::add_aligned_dimension );
    dimensionSubmenu->Add( PCB_ACTIONS::drawOrthogonalDimension );
    dimensionSubmenu->Add( PCB_ACTIONS::drawAlignedDimension );
    dimensionSubmenu->Add( PCB_ACTIONS::drawCenterDimension );
    dimensionSubmenu->Add( PCB_ACTIONS::drawRadialDimension );
    dimensionSubmenu->Add( PCB_ACTIONS::drawLeader );
    placeMenu->Add( dimensionSubmenu );

    // Mirrors the selection tool's Constraints context submenu so the feature is reachable without
    // right-clicking; an unsuitable selection is reported by the tool via the info bar.  The action
    // list is shared (PCB_ACTIONS::ConstraintAddActions) so this menu cannot drift from the context
    // one.  The pane itself is reached from View > Panels, so no toggle is repeated here.
    ACTION_MENU* constraintsSubmenu = new ACTION_MENU( false, selTool );
    constraintsSubmenu->SetTitle( _( "Geometric Constraints" ) );
    constraintsSubmenu->SetIcon( BITMAPS::measurement );

    for( const TOOL_ACTION* action : PCB_ACTIONS::ConstraintAddActions() )
        constraintsSubmenu->Add( *action );

    constraintsSubmenu->AppendSeparator();
    constraintsSubmenu->Add( PCB_ACTIONS::removeConstraints );
    placeMenu->Add( constraintsSubmenu );

    placeMenu->AppendSeparator();
    placeMenu->Add( PCB_ACTIONS::placeCharacteristics );
    placeMenu->Add( PCB_ACTIONS::placeStackup );
    placeMenu->Add( PCB_ACTIONS::placeDrillChart );
    placeMenu->Add( PCB_ACTIONS::placeDrillMap );

    placeMenu->AppendSeparator();
    placeMenu->Add( PCB_ACTIONS::drillOrigin );
    placeMenu->Add( PCB_ACTIONS::drillResetOrigin );
    placeMenu->Add( ACTIONS::gridSetOrigin );
    placeMenu->Add( ACTIONS::gridResetOrigin );
    placeMenu->AppendSeparator();
    placeMenu->Add( PCB_ACTIONS::placeSubGrid );
    placeMenu->AppendSeparator();
    ACTION_MENU* autoplaceSubmenu = new ACTION_MENU( false, selTool );
    autoplaceSubmenu->SetTitle( _( "Auto-Place Footprints" ) );
    autoplaceSubmenu->SetIcon( BITMAPS::mode_module );

    autoplaceSubmenu->Add( PCB_ACTIONS::autoplaceOffboardComponents );
    autoplaceSubmenu->Add( PCB_ACTIONS::autoplaceSelectedComponents );

    placeMenu->Add( autoplaceSubmenu );

    //-- Route Menu ----------------------------------------------------------
    //
    ACTION_MENU* routeMenu = new ACTION_MENU( false, selTool );

    routeMenu->Add( PCB_ACTIONS::selectLayerPair );

    routeMenu->AppendSeparator();
    routeMenu->Add( PCB_ACTIONS::routeSingleTrack );
    routeMenu->Add( PCB_ACTIONS::routeDiffPair );

    routeMenu->AppendSeparator();
    routeMenu->Add( PCB_ACTIONS::tuneSingleTrack );
    routeMenu->Add( PCB_ACTIONS::tuneDiffPair );
    routeMenu->Add( PCB_ACTIONS::tuneSkew );

    routeMenu->AppendSeparator();
    routeMenu->Add( PCB_ACTIONS::routerSettingsDialog );


    //-- Inspect Menu --------------------------------------------------------
    //
    ACTION_MENU* inspectMenu = new ACTION_MENU( false, selTool );

    inspectMenu->Add( PCB_ACTIONS::boardStatistics );
    inspectMenu->Add( ACTIONS::measureTool );

    inspectMenu->AppendSeparator();
    inspectMenu->Add( PCB_ACTIONS::runDRC );
    inspectMenu->Add( ACTIONS::prevMarker );
    inspectMenu->Add( ACTIONS::nextMarker );
    inspectMenu->Add( ACTIONS::excludeMarker );

    inspectMenu->AppendSeparator();
    inspectMenu->Add( PCB_ACTIONS::inspectClearance );
    inspectMenu->Add( PCB_ACTIONS::inspectConstraints );
    inspectMenu->Add( PCB_ACTIONS::showFootprintAssociations );
    inspectMenu->Add( PCB_ACTIONS::diffFootprint );


    //-- Tools menu ----------------------------------------------------------
    //
    ACTION_MENU* toolsMenu = new ACTION_MENU( false, selTool );

    toolsMenu->Add( ACTIONS::updatePcbFromSchematic )->Enable( !Kiface().IsSingle() );
    toolsMenu->Add( PCB_ACTIONS::showEeschema );

    if( !Kiface().IsSingle() )
        toolsMenu->Add( ACTIONS::showProjectManager );

    toolsMenu->Add( ACTIONS::showCalculatorTools );

    toolsMenu->AppendSeparator();
    toolsMenu->Add( PCB_ACTIONS::drcRuleEditor );
    toolsMenu->Add( PCB_ACTIONS::showDrillGroups );

    toolsMenu->AppendSeparator();
    toolsMenu->Add( ACTIONS::showFootprintEditor );
    toolsMenu->Add( PCB_ACTIONS::updateFootprints );
    toolsMenu->Add( PCB_ACTIONS::migrate3DModels );
    toolsMenu->Add( PCB_ACTIONS::editFootprintFields );

    //Zones management
    toolsMenu->AppendSeparator();
    toolsMenu->Add( PCB_ACTIONS::zonesManager );

    if( ADVANCED_CFG::GetCfg().m_EnableGenerators )
    {
        toolsMenu->AppendSeparator();
        toolsMenu->Add( PCB_ACTIONS::generatorsShowManager );
        toolsMenu->Add( PCB_ACTIONS::regenerateAll );
        toolsMenu->Add( PCB_ACTIONS::regenerateSelected );
    }

    toolsMenu->AppendSeparator();
    toolsMenu->Add( PCB_ACTIONS::cleanupTracksAndVias );
    toolsMenu->Add( PCB_ACTIONS::removeUnusedPads );
    toolsMenu->Add( PCB_ACTIONS::cleanupGraphics );
    toolsMenu->Add( PCB_ACTIONS::repairBoard );

    toolsMenu->AppendSeparator();
    toolsMenu->Add( PCB_ACTIONS::collect3DModels );

    toolsMenu->AppendSeparator();
    toolsMenu->Add( PCB_ACTIONS::boardReannotate );
    toolsMenu->Add( ACTIONS::updateSchematicFromPcb )->Enable( !Kiface().IsSingle() );

    ACTION_MENU* multichannelSubmenu = new ACTION_MENU( false, selTool );
    multichannelSubmenu->SetTitle( _( "Multi-Channel" ) );
    multichannelSubmenu->SetIcon( BITMAPS::mode_module );
    multichannelSubmenu->Add( PCB_ACTIONS::generatePlacementRuleAreas );
    multichannelSubmenu->Add( PCB_ACTIONS::repeatLayout );

    toolsMenu->Add( multichannelSubmenu );

    ACTION_MENU* submenuActionPlugins = new ACTION_MENU( false, selTool );
    submenuActionPlugins->SetTitle( _( "External Plugins" ) );
    submenuActionPlugins->SetIcon( BITMAPS::puzzle_piece );

    if( Pgm().GetCommonSettings()->m_Api.enable_server && AddApiPluginMenuItems( submenuActionPlugins ) > 0 )
        submenuActionPlugins->AppendSeparator();

    submenuActionPlugins->Add( ACTIONS::pluginsReload );
    submenuActionPlugins->Add( ACTIONS::pluginsShowFolder );

    toolsMenu->AppendSeparator();
    toolsMenu->Add( submenuActionPlugins );

    //-- Preferences menu ----------------------------------------------------
    //
    ACTION_MENU* prefsMenu = new ACTION_MENU( false, selTool );

    prefsMenu->Add( ACTIONS::configurePaths );
    prefsMenu->Add( ACTIONS::showFootprintLibTable );

    if( ADVANCED_CFG::GetCfg().m_EnablePcbDesignBlocks )
        prefsMenu->Add( ACTIONS::showDesignBlockLibTable );

    prefsMenu->Add( ACTIONS::openPreferences );

    prefsMenu->AppendSeparator();
    AddMenuLanguageList( prefsMenu, selTool );


    //--MenuBar -----------------------------------------------------------
    //
    // WiCAD: in P-CAD mode the menu bar follows P-CAD 2006's PCB editor:
    //   File | Edit | View | Place | Route | Options | Library | Utils | Help
    // Each menu lists P-CAD's commands first, in P-CAD's order and wording (the wording comes from
    // pcad_vocabulary), followed by the KiCad commands P-CAD has no counterpart for, so nothing is lost.
    if( IsPCadMode() )
    {
        //-- Edit ------------------------------------------------------------
        ACTION_MENU* pcadEdit = new ACTION_MENU( false, selTool );
        pcadEdit->Add( ACTIONS::undo );
        pcadEdit->Add( ACTIONS::redo );
        pcadEdit->AppendSeparator();
        pcadEdit->Add( ACTIONS::cut );
        pcadEdit->Add( ACTIONS::copy );
        pcadEdit->Add( ACTIONS::paste );
        pcadEdit->Add( ACTIONS::pasteSpecial );
        pcadEdit->AppendSeparator();
        pcadEdit->Add( PCB_ACTIONS::properties );                   // Properties...
        pcadEdit->Add( ACTIONS::doDelete );                         // Delete
        pcadEdit->Add( PCB_ACTIONS::createArray );                  // Copy Matrix...
        pcadEdit->AppendSeparator();

        ACTION_MENU* componentSub = new ACTION_MENU( false, selTool );
        componentSub->SetTitle( _( "C&omponent" ) );
        componentSub->Add( PCB_ACTIONS::rotateCcw );
        componentSub->Add( PCB_ACTIONS::rotateCw );
        componentSub->Add( PCB_ACTIONS::flip );
        componentSub->Add( PCB_ACTIONS::swap );                     // Swap Components
        componentSub->Add( PCB_ACTIONS::changeFootprints );
        pcadEdit->Add( componentSub );
        pcadEdit->AppendSeparator();

        pcadEdit->Add( ACTIONS::selectAll );
        pcadEdit->Add( ACTIONS::unselectAll );
        pcadEdit->Add( PCB_ACTIONS::clearHighlight );               // Unhighlight All
        pcadEdit->AppendSeparator();

        ACTION_MENU* unrouteSub = new ACTION_MENU( false, selTool );
        unrouteSub->SetTitle( _( "Un&route" ) );
        unrouteSub->Add( PCB_ACTIONS::unrouteSelected );
        unrouteSub->Add( PCB_ACTIONS::globalDeletions );
        pcadEdit->Add( unrouteSub );

        ACTION_MENU* fixSub = new ACTION_MENU( false, selTool );
        fixSub->SetTitle( _( "&Fix" ) );
        fixSub->Add( PCB_ACTIONS::lock );                           // Fix
        fixSub->Add( PCB_ACTIONS::unlock );                         // Unfix
        pcadEdit->Add( fixSub );
        pcadEdit->AppendSeparator();

        pcadEdit->Add( PCB_ACTIONS::editFootprintFields );          // Components...
        pcadEdit->Add( PCB_ACTIONS::showNetInspector );             // Nets...
        pcadEdit->AppendSeparator();
        pcadEdit->Add( ACTIONS::measureTool );                      // Measure
        pcadEdit->Add( ACTIONS::selectionTool );                    // Select
        pcadEdit->AppendSeparator();
        pcadEdit->Add( ACTIONS::find );
        pcadEdit->Add( PCB_ACTIONS::findByProperties );
        pcadEdit->Add( PCB_ACTIONS::editTracksAndVias );
        pcadEdit->Add( PCB_ACTIONS::editTextAndGraphics );
        pcadEdit->Add( PCB_ACTIONS::editTeardrops );
        pcadEdit->Add( PCB_ACTIONS::swapLayers );
        pcadEdit->Add( PCB_ACTIONS::extendGraphic );
        pcadEdit->Add( PCB_ACTIONS::trimGraphic );
        pcadEdit->Add( ACTIONS::deleteTool );

        //-- Place -----------------------------------------------------------
        ACTION_MENU* pcadPlace = new ACTION_MENU( false, selTool );
        pcadPlace->Add( PCB_ACTIONS::placeFootprint );              // Component
        pcadPlace->Add( PCB_ACTIONS::localRatsnestTool );           // Connection
        pcadPlace->Add( PCB_ACTIONS::drawVia );                     // Via
        pcadPlace->AppendSeparator();
        pcadPlace->Add( PCB_ACTIONS::drawLine );                    // Line
        pcadPlace->Add( PCB_ACTIONS::drawArc );                     // Arc
        pcadPlace->Add( PCB_ACTIONS::drawPolygon );                 // Polygon
        pcadPlace->Add( PCB_ACTIONS::placePoint );                  // Point
        pcadPlace->AppendSeparator();
        pcadPlace->Add( PCB_ACTIONS::drawZone );                    // Copper Pour
        pcadPlace->Add( PCB_ACTIONS::drawZoneCutout );              // Cutout
        pcadPlace->Add( PCB_ACTIONS::drawRuleArea );                // Keepout
        pcadPlace->AppendSeparator();
        pcadPlace->Add( PCB_ACTIONS::placeText );                   // Text

        ACTION_MENU* pcadDimension = new ACTION_MENU( false, selTool );
        pcadDimension->SetTitle( _( "&Dimension" ) );
        pcadDimension->SetIcon( BITMAPS::add_aligned_dimension );
        pcadDimension->Add( PCB_ACTIONS::drawOrthogonalDimension );
        pcadDimension->Add( PCB_ACTIONS::drawAlignedDimension );
        pcadDimension->Add( PCB_ACTIONS::drawCenterDimension );
        pcadDimension->Add( PCB_ACTIONS::drawRadialDimension );
        pcadDimension->Add( PCB_ACTIONS::drawLeader );
        pcadPlace->Add( pcadDimension );                            // Dimension
        pcadPlace->AppendSeparator();

        pcadPlace->Add( PCB_ACTIONS::drawRectangle );
        pcadPlace->Add( PCB_ACTIONS::drawCircle );
        pcadPlace->Add( PCB_ACTIONS::drawBezier );
        pcadPlace->Add( PCB_ACTIONS::drawTextBox );
        pcadPlace->Add( PCB_ACTIONS::drawTable );
        pcadPlace->Add( PCB_ACTIONS::placeReferenceImage );
        pcadPlace->Add( PCB_ACTIONS::placeBarcode );
        pcadPlace->Add( PCB_ACTIONS::placeViaStack );
        pcadPlace->Add( PCB_ACTIONS::drawCopperThievingZone );
        pcadPlace->AppendSeparator();
        pcadPlace->Add( PCB_ACTIONS::placeStackup );
        pcadPlace->Add( PCB_ACTIONS::placeDrillChart );
        pcadPlace->Add( PCB_ACTIONS::placeDrillMap );
        pcadPlace->Add( PCB_ACTIONS::placeCharacteristics );
        pcadPlace->AppendSeparator();
        pcadPlace->Add( ACTIONS::gridSetOrigin );
        pcadPlace->Add( ACTIONS::gridResetOrigin );
        pcadPlace->Add( PCB_ACTIONS::drillOrigin );
        pcadPlace->Add( PCB_ACTIONS::drillResetOrigin );

        //-- Route -----------------------------------------------------------
        ACTION_MENU* pcadRoute = new ACTION_MENU( false, selTool );

        ACTION_MENU* autorouterSub = new ACTION_MENU( false, selTool );
        autorouterSub->SetTitle( _( "&Autorouters" ) );
        autorouterSub->Add( PCB_ACTIONS::exportSpecctraDSN,     ACTION_MENU::NORMAL, _( "Export Specctra DSN..." ) );
        autorouterSub->Add( PCB_ACTIONS::importSpecctraSession, ACTION_MENU::NORMAL, _( "Import Specctra SES..." ) );
        pcadRoute->Add( autorouterSub );                            // Autorouters...
        pcadRoute->AppendSeparator();
        pcadRoute->Add( PCB_ACTIONS::routeSingleTrack );            // Manual
        pcadRoute->Add( PCB_ACTIONS::routeDiffPair );               // Differential Pair
        pcadRoute->AppendSeparator();
        pcadRoute->Add( PCB_ACTIONS::tuneSingleTrack );
        pcadRoute->Add( PCB_ACTIONS::tuneDiffPair );
        pcadRoute->Add( PCB_ACTIONS::tuneSkew );
        pcadRoute->AppendSeparator();
        pcadRoute->Add( PCB_ACTIONS::selectLayerPair );             // Layer Pairs...
        pcadRoute->Add( PCB_ACTIONS::routerSettingsDialog );        // Route Options...

        //-- Options ---------------------------------------------------------
        ACTION_MENU* pcadOptions = new ACTION_MENU( false, selTool );
        pcadOptions->Add( ACTIONS::openPreferences );               // Configure...
        pcadOptions->Add( ACTIONS::gridProperties );                // Grids...
        pcadOptions->Add( ACTIONS::gridOrigin );
        pcadOptions->AppendSeparator();
        pcadOptions->Add( PCB_ACTIONS::showLayersManager, ACTION_MENU::CHECK );   // Layers...
        pcadOptions->Add( ACTIONS::showProperties,        ACTION_MENU::CHECK );
        pcadOptions->AppendSeparator();
        pcadOptions->Add( PCB_ACTIONS::boardSetup );                // Design Rules... (also net classes, text and via sizes)
        pcadOptions->Add( PCB_ACTIONS::drcRuleEditor );
        pcadOptions->Add( PCB_ACTIONS::zonesManager );
        pcadOptions->AppendSeparator();
        pcadOptions->Add( ACTIONS::configurePaths );
        pcadOptions->AppendSeparator();
        AddMenuLanguageList( pcadOptions, selTool );

        //-- Library ---------------------------------------------------------
        ACTION_MENU* pcadLibrary = new ACTION_MENU( false, selTool );
        pcadLibrary->Add( ACTIONS::showFootprintLibTable );         // Library Setup (Patterns)...
        pcadLibrary->Add( ACTIONS::showFootprintBrowser );
        pcadLibrary->Add( ACTIONS::showFootprintEditor );           // WiCAD Pattern Editor...
        pcadLibrary->AppendSeparator();
        pcadLibrary->Add( PCB_ACTIONS::updateFootprints );          // Force Update...
        pcadLibrary->Add( PCB_ACTIONS::changeFootprints );
        pcadLibrary->Add( PCB_ACTIONS::exportFootprints );
        pcadLibrary->Add( PCB_ACTIONS::migrate3DModels );
        pcadLibrary->Add( PCB_ACTIONS::collect3DModels );

        //-- Utils -----------------------------------------------------------
        ACTION_MENU* pcadUtils = new ACTION_MENU( false, selTool );
        pcadUtils->Add( PCB_ACTIONS::boardReannotate );             // Renumber Components...
        pcadUtils->AppendSeparator();
        pcadUtils->Add( PCB_ACTIONS::runDRC );                      // DRC...
        pcadUtils->Add( ACTIONS::prevMarker );                      // Find Errors
        pcadUtils->Add( ACTIONS::nextMarker );
        pcadUtils->Add( ACTIONS::excludeMarker );
        pcadUtils->AppendSeparator();
        pcadUtils->Add( ACTIONS::updatePcbFromSchematic )->Enable( !Kiface().IsSingle() );   // Load Netlist / ECO
        pcadUtils->Add( ACTIONS::updateSchematicFromPcb )->Enable( !Kiface().IsSingle() );
        pcadUtils->Add( PCB_ACTIONS::importNetlist );               // Load Netlist...
        pcadUtils->Add( PCB_ACTIONS::cleanupTracksAndVias );        // Trace Clean-up...
        pcadUtils->Add( PCB_ACTIONS::removeUnusedPads );
        pcadUtils->Add( PCB_ACTIONS::cleanupGraphics );
        pcadUtils->Add( PCB_ACTIONS::repairBoard );
        pcadUtils->AppendSeparator();
        pcadUtils->Add( PCB_ACTIONS::zoneFillAll );
        pcadUtils->Add( PCB_ACTIONS::zoneUnfillAll );
        pcadUtils->AppendSeparator();
        pcadUtils->Add( PCB_ACTIONS::boardStatistics );
        pcadUtils->Add( PCB_ACTIONS::inspectClearance );
        pcadUtils->Add( PCB_ACTIONS::inspectConstraints );
        pcadUtils->Add( PCB_ACTIONS::showFootprintAssociations );
        pcadUtils->Add( PCB_ACTIONS::diffFootprint );
        pcadUtils->AppendSeparator();
        pcadUtils->Add( PCB_ACTIONS::showEeschema );                // WiCAD Schematic...

        if( !Kiface().IsSingle() )
            pcadUtils->Add( ACTIONS::showProjectManager );

        pcadUtils->Add( ACTIONS::showCalculatorTools );
        pcadUtils->AppendSeparator();
        // these two already belong to the stock Tools menu, so take copies
        pcadUtils->Add( multichannelSubmenu->Clone() );
        pcadUtils->Add( submenuActionPlugins->Clone() );

        // the stock menus these replace
        delete editMenu;
        delete placeMenu;
        delete routeMenu;
        delete inspectMenu;
        delete toolsMenu;
        delete prefsMenu;

        menuBar->Append( fileMenu,    _( "&File" ) );
        menuBar->Append( pcadEdit,    _( "&Edit" ) );
        menuBar->Append( viewMenu,    _( "&View" ) );
        menuBar->Append( pcadPlace,   _( "&Place" ) );
        menuBar->Append( pcadRoute,   _( "&Route" ) );
        menuBar->Append( pcadOptions, _( "&Options" ) );
        menuBar->Append( pcadLibrary, _( "&Library" ) );
        menuBar->Append( pcadUtils,   _( "&Utils" ) );
    }
    else
    {
    menuBar->Append( fileMenu,    _( "&File" ) );
    menuBar->Append( editMenu,    _( "&Edit" ) );
    menuBar->Append( viewMenu,    _( "&View" ) );
    menuBar->Append( placeMenu,   _( "&Place" ) );
    menuBar->Append( routeMenu,   _( "Ro&ute" ) );
    menuBar->Append( inspectMenu, _( "&Inspect" ) );
    menuBar->Append( toolsMenu,   _( "&Tools" ) );
    menuBar->Append( prefsMenu,   _( "P&references" ) );
    }
    AddStandardHelpMenu( menuBar );

    SetMenuBar( menuBar );
    delete oldMenuBar;

}
