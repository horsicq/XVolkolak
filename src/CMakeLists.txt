if(TARGET Qt6::Core OR TARGET Qt6::Widgets)
    set(XVOLKOLAK_QT_MAJOR 6)
elseif(TARGET Qt5::Core OR TARGET Qt5::Widgets)
    set(XVOLKOLAK_QT_MAJOR 5)
else()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
    set(XVOLKOLAK_QT_MAJOR ${QT_VERSION_MAJOR})
endif()

set(XVOLKOLAK_QT_COMPONENTS
    Core
    Concurrent
    Widgets
    PrintSupport
    Network
    Svg
    LinguistTools
)

if(XVOLKOLAK_QT_MAJOR EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS ${XVOLKOLAK_QT_COMPONENTS})
elseif(XVOLKOLAK_QT_MAJOR GREATER_EQUAL 6)
    find_package(Qt${XVOLKOLAK_QT_MAJOR} REQUIRED COMPONENTS ${XVOLKOLAK_QT_COMPONENTS})

    if(NOT TARGET Qt5::Widgets)
        add_library(Qt5::Widgets INTERFACE IMPORTED)
        set_property(TARGET Qt5::Widgets PROPERTY
            INTERFACE_LINK_LIBRARIES Qt${XVOLKOLAK_QT_MAJOR}::Widgets
        )
    endif()
endif()

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../release_version.txt" X_PROJECT_VERSION)
project(XVolkolak VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(X_COMPANYNAME "XVolkolak")
set(X_PROJECTNAME "XVolkolak")
set(X_MAINTAINER "horsicq@gmail.com")
set(X_DESCRIPTION "XVolkolak emulator-unpacker")
set(X_HOMEPAGE "https:/ntinfo.biz")
set(X_ORIGINAL_FILENAME "xvolkolak")

if(NOT DEFINED MACOSX_BUNDLE_GUI_IDENTIFIER OR "${MACOSX_BUNDLE_GUI_IDENTIFIER}" STREQUAL "")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "ntinfo.xvolkolak.app")
endif()
set(BUNDLE_ID_OPTION "${MACOSX_BUNDLE_GUI_IDENTIFIER}")

set(X_RESOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../res")
set(X_WINDOWS_ICON "${X_RESOURCE_DIR}/windows/main.ico")
set(X_WINDOWS_RESOURCE_RC_IN "${X_RESOURCE_DIR}/windows/resource.rc.in")
set(X_WINDOWS_RESOURCE_ICON_RC_IN "${X_RESOURCE_DIR}/windows/resource_icon.rc.in")
set(X_WINDOWS_RESOURCE_RC "${X_RESOURCE_DIR}/windows/resource.rc")
set(X_WINDOWS_RESOURCE_ICON_RC "${X_RESOURCE_DIR}/windows/resource_icon.rc")
set(X_MACOS_ICON "${X_RESOURCE_DIR}/macos/main.icns")
set(X_MACOS_INFO_PLIST_IN "${X_RESOURCE_DIR}/macos/Info.plist.in")
set(X_MACOS_INFO_PLIST "${X_RESOURCE_DIR}/macos/Info.plist")

set(XVOLKOLAK_DEPLOY_INIT "${CMAKE_CURRENT_LIST_DIR}/../dep/build_tools/cmake/deploy_init.cmake")
if(EXISTS "${XVOLKOLAK_DEPLOY_INIT}"
   AND EXISTS "${X_WINDOWS_RESOURCE_RC_IN}"
   AND EXISTS "${X_WINDOWS_RESOURCE_ICON_RC_IN}"
   AND EXISTS "${X_MACOS_ICON}"
   AND EXISTS "${X_MACOS_INFO_PLIST_IN}")
    include("${XVOLKOLAK_DEPLOY_INIT}")
else()
    message(STATUS "Skipping deploy_init.cmake because packaging resource templates are missing")
endif()

set(XVOLKOLAK_TRANSLATION_HELPER "${CMAKE_CURRENT_LIST_DIR}/../dep/build_tools/cmake/deploy_translation.cmake")
if(EXISTS "${XVOLKOLAK_TRANSLATION_HELPER}")
    include("${XVOLKOLAK_TRANSLATION_HELPER}")

    set(XVOLKOLAK_TS_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_ar.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_bn.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_de.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_es.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_fa.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_fr.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_he.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_id.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_it.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_ja.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_ko.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_pl.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_pt_BR.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_pt_PT.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_ru.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_sv.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_tr.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_uk.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_vi.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_sq.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_zh.ts"
        "${CMAKE_CURRENT_LIST_DIR}/../translations/xvolkolak_zh_TW.ts"
    )

    deploy_add_translations(
        TARGET_NAME xvolkolak_translations
        ADD_TO_ALL
        SOURCE_DIRS "${CMAKE_CURRENT_LIST_DIR}"
        TS_FILES ${XVOLKOLAK_TS_FILES}
        INSTALL_DESTINATION lang
        WINDOWS_APPDATA_SUBDIR "${X_PROJECTNAME}"
    )
endif()

# Dependency targets for the NFD scan engine (auto packer detection). These provide the
# capstone / xsimd / compression static libraries the DIE detection engine links against.
set(XVOLKOLAK_MYLIBS "${CMAKE_CURRENT_LIST_DIR}/../dep")
add_subdirectory("${XVOLKOLAK_MYLIBS}/XCapstone" XCapstone)
add_subdirectory("${XVOLKOLAK_MYLIBS}/XArchive" XArchive)
add_subdirectory("${XVOLKOLAK_MYLIBS}/Formats/xsimd" XSIMD)

add_subdirectory(gui)
add_subdirectory(console)
