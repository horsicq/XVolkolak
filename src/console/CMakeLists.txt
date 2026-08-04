set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)  # match xuniversalunpacker (engine is built at C++17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_definitions(-DNOMINMAX)
endif()

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
    add_compile_options(/FS)
    add_compile_options($<$<CONFIG:Debug>:/Z7> $<$<CONFIG:RelWithDebInfo>:/Z7>)
endif()

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

add_executable(xvolkolakc
    main_console.cpp
)

target_include_directories(xvolkolakc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(xvolkolakc PRIVATE
    xuniversalunpacker
    Qt${XVOLKOLAK_QT_MAJOR}::Core
)

if(TARGET xvolkolak_translations)
    add_dependencies(xvolkolakc xvolkolak_translations)
endif()

if(UNIX AND NOT APPLE)
    install(TARGETS xvolkolakc RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
else()
    install(TARGETS xvolkolakc RUNTIME DESTINATION ".")
endif()
