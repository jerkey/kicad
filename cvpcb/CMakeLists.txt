add_definitions( -DCVPCB )

###
# Includes
###

include_directories( BEFORE ${INC_BEFORE} )
include_directories(
    ./dialogs
    ../3d-viewer
    ../pcbnew
    ../pcbnew/dialogs
    ../polygon
    ../common
    ${INC_AFTER}
    )

###
# Sources
###
set( CVPCB_DIALOGS
    dialogs/dialog_cvpcb_config.cpp
    dialogs/dialog_cvpcb_config_fbp.cpp
    dialogs/dialog_display_options.cpp
    dialogs/dialog_display_options_base.cpp
    ../pcbnew/dialogs/dialog_fp_lib_table.cpp
    ../pcbnew/dialogs/dialog_fp_lib_table_base.cpp
    ../pcbnew/dialogs/dialog_fp_plugin_options.cpp
    ../pcbnew/dialogs/dialog_fp_plugin_options_base.cpp
    )

set( CVPCB_SRCS
    ../common/base_units.cpp
    ../pcbnew/board_items_to_polygon_shape_transform.cpp
    ../pcbnew/class_drc_item.cpp
    autosel.cpp
    cfg.cpp
    class_components_listbox.cpp
    class_DisplayFootprintsFrame.cpp
    class_footprints_listbox.cpp
    class_library_listbox.cpp
    cvframe.cpp
    cvpcb.cpp
    listboxes.cpp
    menubar.cpp
    readwrite_dlgs.cpp
    tool_cvpcb.cpp
    )

###
# Windows resource file
###
if( WIN32 )
    if( MINGW )
        # CVPCB_RESOURCES variable is set by the macro.
        mingw_resource_compiler( cvpcb )
    else()
        set( CVPCB_RESOURCES cvpcb.rc )
    endif()
endif()

###
# Apple resource files
###
if( APPLE )
    set( CVPCB_RESOURCES cvpcb.icns cvpcb_doc.icns )

    set_source_files_properties( "${CMAKE_CURRENT_SOURCE_DIR}/cvpcb.icns"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

    set_source_files_properties( "${CMAKE_CURRENT_SOURCE_DIR}/cvpcb_doc.icns"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

    set( MACOSX_BUNDLE_ICON_FILE cvpcb.icns )
    set( MACOSX_BUNDLE_GUI_IDENTIFIER org.kicad-eda.cvpcb )
endif()

###
# Create the cvpcb executable
###
add_executable( cvpcb WIN32 MACOSX_BUNDLE
    ${CVPCB_SRCS}
    ${CVPCB_DIALOGS}
    ${CVPCB_RESOURCES}
    )

###
# Set properties for APPLE on cvpcb target
###
if( APPLE )
    set_target_properties( cvpcb PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist )
endif()

###
# Link executable target cvpcb with correct libraries
###
target_link_libraries( cvpcb
    3d-viewer
    pcbcommon
    pcad2kicadpcb
    common
    bitmaps
    polygon
    gal
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GDI_PLUS_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${PIXMAN_LIBRARY}
    )

# Only for win32 cross compilation using MXE
if( WIN32 AND MSYS AND CMAKE_CROSSCOMPILING )
target_link_libraries(cvpcb
    opengl32
    glu32
    pixman-1
    fontconfig
    freetype
    bz2
    )
endif()


if( BUILD_GITHUB_PLUGIN )
    target_link_libraries( cvpcb github_plugin )
endif()

# Must follow github_plugin
target_link_libraries( cvpcb ${Boost_LIBRARIES} )


###
# Add cvpcb as install target
###
install( TARGETS cvpcb
    DESTINATION ${KICAD_BIN}
    COMPONENT binary
    )
