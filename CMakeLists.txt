# Project Settings
cmake_minimum_required(VERSION 3.10)
project(GarlicConfig VERSION 1.0.0 DESCRIPTION "Library primarily used for reading, writing and merging garlic configs.")
include(GNUInstallDirs)
IF(NOT CGET_PREFIX)
    include(cget/cget/cget.cmake)  # Include cget's cmake file to load up all dependencies.
ENDIF()

set(CMAKE_CXX_STANDARD 17)

# Sources.
set(PUBLIC_HEADERS include/garlicconfig.h include/repository.h include/exceptions.h include/layer.h include/encoding.h include/json.h)
set(SOURCE_FILES
        ${PUBLIC_HEADERS} src/repository.cpp src/encoding/json.cpp)

find_package(Boost 1.68.0 REQUIRED COMPONENTS filesystem)

# Library
add_library(${PROJECT_NAME} ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${PUBLIC_HEADERS}")
target_include_directories(
        ${PROJECT_NAME} INTERFACE
        "$<BUILD_INTERFACE:${GarlicConfig_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
include_directories(${PROJECT_NAME} src)
include_directories(${PROJECT_NAME} include)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})


# Installation
include(CMakePackageConfigHelpers)  # To load the write_basic_package_version_file macro.
configure_file(garlicconfig.pc.in garlicconfig.pc @ONLY)
set(PROJECT_TARGET_EXPORT_NAME ${PROJECT_NAME}Targets)  # The project export target name.
set(CMAKE_FILES_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")  # This is where the cmake files would live, after installation.
set(GENERATED ${CMAKE_CURRENT_BINARY_DIR}/generated)  # Generated files live here until installation.
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_TARGET_EXPORT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(EXPORT ${PROJECT_TARGET_EXPORT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_FILES_INSTALL_DIR})
install(FILES ${CMAKE_BINARY_DIR}/garlicconfig.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
set(CONFIG_FILE ${GENERATED}/${PROJECT_NAME}Config.cmake)
configure_package_config_file("cmake/Config.cmake.in"
        "${CONFIG_FILE}" INSTALL_DESTINATION ${CMAKE_FILES_INSTALL_DIR})
set(VERSION_FILE ${GENERATED}/${PROJECT_NAME}ConfigVersion.cmake)
write_basic_package_version_file(
        ${VERSION_FILE}
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)
install(FILES ${VERSION_FILE} ${CONFIG_FILE} DESTINATION ${CMAKE_FILES_INSTALL_DIR})

# Testing
include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
    enable_testing()
    add_test(GarlicConfigTest tests/GarlicConfigTest)
endif ()
