include(../../dep/build_tools/cmake/cpp_standart_setup.cmake)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

set(XVOLKOLAK_MYLIBS "${CMAKE_CURRENT_LIST_DIR}/../../dep")

# Build the engine library FIRST: xuniversalpacker.cmake pulls the scan engine
# (specabstract.cmake) and defines SPECABSTRACT_SOURCES et al. With those defined, the widget
# cmakes below (nfd_widget's `if(NOT DEFINED SPECABSTRACT_SOURCES)` guard) add ONLY the widget
# layer to this GUI target instead of re-compiling the whole engine into it -- the engine lives
# solely in the xuniversalunpacker library the target links against.
include("${XVOLKOLAK_MYLIBS}/XUniversalUnpacker/xuniversalpacker.cmake")
include("${XVOLKOLAK_MYLIBS}/nfd_widget/nfd_widget.cmake")
include("${XVOLKOLAK_MYLIBS}/XOptions/xoptionswidget.cmake")
include("${XVOLKOLAK_MYLIBS}/XAboutWidget/xaboutwidget.cmake")
include("${XVOLKOLAK_MYLIBS}/XStyles/xstyles.cmake")

set(XVOLKOLAK_GUI_TARGET XVolkolak)

# The widget cmakes overlap (nfd_widget already pulls xscanwidgets -> xoptionswidget, ...), so
# the same source can appear more than once. De-duplicate by canonical path -- a doubly listed
# Q_OBJECT source otherwise gets its meta-object compiled twice (LNK2005).
set(XVOLKOLAK_GUI_WIDGET_SOURCES_RAW
    ${NFD_WIDGET_SOURCES}
    ${XOPTIONSWIDGET_SOURCES}
    ${XABOUTWIDGET_SOURCES}
    ${XSTYLES_SOURCES}
)
set(XVOLKOLAK_GUI_WIDGET_SOURCES "")
foreach(src ${XVOLKOLAK_GUI_WIDGET_SOURCES_RAW})
    get_filename_component(src_abs "${src}" ABSOLUTE)
    list(APPEND XVOLKOLAK_GUI_WIDGET_SOURCES "${src_abs}")
endforeach()
list(REMOVE_DUPLICATES XVOLKOLAK_GUI_WIDGET_SOURCES)

set(XVOLKOLAK_GUI_SOURCES
    ${XVOLKOLAK_GUI_WIDGET_SOURCES}
    dialogabout.cpp
    dialogabout.h
    dialogabout.ui
    dialogoptions.cpp
    dialogoptions.h
    dialogoptions.ui
    dialogunpack.cpp
    dialogunpack.h
    dialogunpack.ui
    guimainwindow.cpp
    guimainwindow.h
    guimainwindow.ui
    unpackworker.cpp
    unpackworker.h
    main_gui.cpp
    "${CMAKE_CURRENT_LIST_DIR}/../../res/resources_gui.qrc"
)

if(WIN32 AND EXISTS "${X_WINDOWS_RESOURCE_ICON_RC}")
    list(APPEND XVOLKOLAK_GUI_SOURCES "${X_WINDOWS_RESOURCE_ICON_RC}")
endif()

if(APPLE AND EXISTS "${X_MACOS_ICON}")
    set_source_files_properties("${X_MACOS_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    list(APPEND XVOLKOLAK_GUI_SOURCES "${X_MACOS_ICON}")
endif()

add_executable(${XVOLKOLAK_GUI_TARGET} WIN32
    ${XVOLKOLAK_GUI_SOURCES}
)

set_target_properties(${XVOLKOLAK_GUI_TARGET} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "${MACOSX_BUNDLE_GUI_IDENTIFIER}"
    OUTPUT_NAME "${X_ORIGINAL_FILENAME}"
)

target_include_directories(${XVOLKOLAK_GUI_TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    # Engine header dirs the scan widget needs (their .cmake includes were pre-empted above,
    # so their include_directories() never ran -- the SOURCES live in xuniversalunpacker).
    "${XVOLKOLAK_MYLIBS}/SpecAbstract"
    "${XVOLKOLAK_MYLIBS}/SpecAbstract/modules"
    "${XVOLKOLAK_MYLIBS}/XScanEngine"
    "${XVOLKOLAK_MYLIBS}/XScanEngine/modules"
    "${XVOLKOLAK_MYLIBS}/Formats"
    "${XVOLKOLAK_MYLIBS}/Formats/exec"
    "${XVOLKOLAK_MYLIBS}/Formats/images"
    "${XVOLKOLAK_MYLIBS}/Formats/video"
    "${XVOLKOLAK_MYLIBS}/Formats/audio"
    "${XVOLKOLAK_MYLIBS}/Formats/texts"
    "${XVOLKOLAK_MYLIBS}/Formats/formats"
    "${XVOLKOLAK_MYLIBS}/Formats/xsimd/src"
    "${XVOLKOLAK_MYLIBS}/XOptions"
    "${XVOLKOLAK_MYLIBS}/XDEX"
    "${XVOLKOLAK_MYLIBS}/XPDF"
    "${XVOLKOLAK_MYLIBS}/XArchive"
    "${XVOLKOLAK_MYLIBS}/XArchive/Algos"
    "${XVOLKOLAK_MYLIBS}/XDisasmCore"
    "${XVOLKOLAK_MYLIBS}/XDisasmCore/Modules"
    "${XVOLKOLAK_MYLIBS}/XStaticUnpacker"
    "${XVOLKOLAK_MYLIBS}/XCapstone"
    "${XVOLKOLAK_MYLIBS}/XCapstone/3rdparty/Capstone/src/include"
)

target_link_libraries(${XVOLKOLAK_GUI_TARGET} PRIVATE
    xuniversalunpacker
    Qt${XVOLKOLAK_QT_MAJOR}::Widgets
    Qt${XVOLKOLAK_QT_MAJOR}::Concurrent
    Qt${XVOLKOLAK_QT_MAJOR}::PrintSupport
)

if(TARGET xvolkolak_translations)
    add_dependencies(${XVOLKOLAK_GUI_TARGET} xvolkolak_translations)
endif()

if(UNIX AND NOT APPLE)
    set(XVOLKOLAK_RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}")
    set(XVOLKOLAK_DOC_DESTINATION "${CMAKE_INSTALL_DOCDIR}/xvolkolak")
    set(XVOLKOLAK_DESKTOP_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/xvolkolak.desktop.in")
    set(XVOLKOLAK_DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/xvolkolak.desktop")

    if(EXISTS "${XVOLKOLAK_DESKTOP_TEMPLATE}")
        configure_file("${XVOLKOLAK_DESKTOP_TEMPLATE}" "${XVOLKOLAK_DESKTOP_FILE}" @ONLY)
    endif()
else()
    set(XVOLKOLAK_RUNTIME_DESTINATION ".")
    set(XVOLKOLAK_DOC_DESTINATION ".")
endif()

install(TARGETS ${XVOLKOLAK_GUI_TARGET}
    RUNTIME DESTINATION ${XVOLKOLAK_RUNTIME_DESTINATION}
    BUNDLE DESTINATION .
)

if(UNIX AND NOT APPLE)
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../README.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../THANKS.md"
        DESTINATION ${XVOLKOLAK_DOC_DESTINATION}
        OPTIONAL
    )

    if(EXISTS "${XVOLKOLAK_DESKTOP_FILE}")
        install(FILES "${XVOLKOLAK_DESKTOP_FILE}"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/hicolor")
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/hicolor/"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor")
    endif()
else()
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE"
        DESTINATION .
        RENAME LICENSE.txt
        OPTIONAL
    )

    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/../../README.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../THANKS.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../res/license.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../res/readme.txt"
        "${X_WINDOWS_ICON}"
        DESTINATION .
        OPTIONAL
    )

    install(CODE [[file(WRITE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/portable" "")]])
endif()

if(APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/macdeployqt.cmake)
endif()

if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/deploy_qt_windows.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/deploy_msvc.cmake)
endif()
