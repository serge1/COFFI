cmake_minimum_required(VERSION 3.10)

# Detect if we are the top level CMakeLists.txt or are we included in some
# other project
if(NOT DEFINED PROJECT_NAME)
    set(IS_TOP_PROJECT TRUE)
endif()

if(IS_TOP_PROJECT)
    # Turn this on in order to build coffi examples
    option(COFFI_BUILD_EXAMPLES "Build COFFI examples" ON)

    # Turns this on in order to build tests
    option(COFFI_BUILD_TESTS "Build COFFI tests" OFF)

    # Generate output of compile commands during generation
    set(CMAKE_EXPORT_COMPILE_COMMANDS, ON)
endif()

# Read version from header file
set(version_header "coffi/coffi_version.hpp")
file(READ ${version_header} ver)
string(REGEX MATCH "#define COFFI_VERSION \"([0-9\.]+)\"" _ ${ver})

if(NOT CMAKE_MATCH_1)
    message(FATAL_ERROR "Unable to parse version from ${version_header}")
endif()

set(version ${CMAKE_MATCH_1})

# Use configure_file to make configure step depend on coffi_version.hpp
configure_file(${version_header} ${CMAKE_CURRENT_BINARY_DIR}/coffi_version.hpp.copy COPYONLY)

project(coffi VERSION ${version} LANGUAGES C CXX)

include(GNUInstallDirs)

# Create a header only CMake target for coffi
add_library(coffi INTERFACE)
add_library(coffi::coffi ALIAS coffi)

target_include_directories(
    coffi
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# If this is the top level project, add in logic to install coffi
if(IS_TOP_PROJECT)
    # Enable C++17 for examples and tests
    if(COFFI_BUILD_EXAMPLES OR COFFI_BUILD_TESTS)
        set(CMAKE_CXX_STANDARD 17)
    endif()

    if(COFFI_BUILD_EXAMPLES)
        # set (CMAKE_CXX_FLAGS "-Wall")
        add_subdirectory(examples)
    endif()

    if(COFFI_BUILD_TESTS)
        # set (CMAKE_CXX_FLAGS "-Wall")
        enable_testing()
        add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} USES_TERMINAL)
        add_subdirectory(tests)
    endif()

    include(CMakePackageConfigHelpers)

    # Create a file that includes the current project version. This will be
    # installed with the coffi CMake package.
    write_basic_package_version_file(
        "${PROJECT_NAME}ConfigVersion.cmake"
        VERSION
        ${PROJECT_VERSION}
        COMPATIBILITY
        SameMajorVersion)

    # Create the default ${PROJECT_NAME}Config.cmake file which will be
    # installed and found by calls to `find_package(coffi)`.
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

    # Install the previously generated "config" and "version" files
    install(
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

    # Install the entire local `coffi` directory to the include directory
    install(
        DIRECTORY
        coffi
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR})

    # Create a ${PROJECT_NAME}Targets.cmake file that is referenced by the
    # ${PROJECT_NAME}Config.cmake file and includes the target information
    # needed to compile/link against all targets exported under the
    # ${PROJECT_NAME}_Targets export
    install(
        EXPORT
        ${PROJECT_NAME}_Targets
        FILE
        ${PROJECT_NAME}Targets.cmake
        NAMESPACE
        ${PROJECT_NAME}::
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

    # Add the coffi target to the ${PROJECT_NAME}_Targets export
    install(
        TARGETS
        coffi
        EXPORT
        ${PROJECT_NAME}_Targets)

    # cmake -B build; cmake --build build; cpack --config build/CPackSourceConfig.cmake
    # cmake -B build -D COFFI_BUILD_EXAMPLES=ON -DCOFFI_BUILD_TESTS=ON -G Ninja; cmake --build build; cpack --config build/CPackSourceConfig.cmake
    set(_fmt TGZ)

    if(WIN32)
        set(_fmt ZIP)
    endif()

    set(CPACK_GENERATOR ${_fmt})
    set(CPACK_SOURCE_GENERATOR ${_fmt})
    set(CPACK_PACKAGE_VENDOR "COFFI")
    set(CPACK_PACKAGE_CONTACT "Serge Lamikhov-Center")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/package")
    set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    string(TOLOWER ${CMAKE_SYSTEM_NAME} _sys)
    string(TOLOWER ${PROJECT_NAME} _project_lower)
    set(CPACK_PACKAGE_FILE_NAME "${_project_lower}-${_sys}")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${_project_lower}-${PROJECT_VERSION}")

    set(CPACK_SOURCE_IGNORE_FILES "/.git.*;/.vs.*;/build;/.clang-format;/doc/site;/doc/coffi.docx;/doc/images/callouts/;/doc/images/colorsvg/;/doc/images/res2/;/doc/images/.*\.svg;/doc/images/.*\.gif;/doc/images/[^/]*\.png$;/doc/images/.*\.tif;/examples/sudo_gdb.sh;/tests")

    install(FILES ${CPACK_RESOURCE_FILE_README} ${CPACK_RESOURCE_FILE_LICENSE}
        DESTINATION share/docs/${PROJECT_NAME})

    include(CPack)

    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
        add_custom_target( docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif()
