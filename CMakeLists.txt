# 2021-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
# See license.md for details.

# Version 3.2 and upwards.
CMAKE_MINIMUM_REQUIRED (VERSION 3.2)

# Setup versioning.
set(BUILD_MAJOR 0)
set(BUILD_MINOR 3)
set(BUILD_PATCH 1)
set(BUILD_VERSION "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")

# Expose version information to config.
set(VERSION_NAME_BUFFER_SIZE 16)
set(VERSION_MAJOR ${BUILD_MAJOR})
set(VERISON_MINOR ${BUILD_MINOR})
set(VERSION_PATCH ${BUILD_PATCH})
set(VERSION_FORMAT "v%d.%d.%d")

# Specify version for this project.
project (kaji VERSION ${BUILD_VERSION})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE PATH "" FORCE)
endif()

if (USE_GEDANKEN)
ADD_DEFINITIONS(-DUSE_GEDANKEN=1)
endif()

string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    IS_STANDALON_PROJECT
)

# .
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# Make sure libraries will be places beside executable.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        ADD_DEFINITIONS(-DUCRTBASEDLL_NAME="ucrtbase.dll")
    else ()
        ADD_DEFINITIONS(-DUCRTBASEDLL_NAME="ucrtbased.dll")
    endif ()
endif()

if (MSVC)
    # warning level 4 and all warnings as errors
    #add_compile_options(/W4 /WX)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/MD)
    else ()
        add_compile_options(/MDd)
    endif ()

    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE=1)    
    add_definitions(-Dstrdup=_strdup)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wunused-result)
endif()

# Setup git submodules.
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Update submodules ...")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# 
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deps/tempora/CMakeLists.txt")
    message(FATAL_ERROR "The submodule 'tempora' could not be fetched! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
add_subdirectory(deps/tempora)

# .
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deps/spirits/CMakeLists.txt")
    message(FATAL_ERROR "The submodule 'spirits' could not be fetched! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
add_subdirectory(deps/spirits)

# Collect source and header files.
file(GLOB_RECURSE SRC
    ${PROJECT_SOURCE_DIR}/src/*.c
)
add_library(kaji ${SRC})
target_include_directories(kaji PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc/>
)
set_property(TARGET kaji PROPERTY C_STANDARD 11)
add_dependencies(kaji tempora spirits)
target_link_libraries(kaji tempora spirits)

# .
add_executable(kaji-example-counter EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/examples/counter.c
)
set_property(TARGET kaji-example-counter PROPERTY C_STANDARD 17)
add_dependencies(kaji-example-counter kaji)
target_link_libraries(kaji-example-counter kaji)

# .
add_executable(kaji-example-gedanken EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/examples/use_gedanken.c
)
set_property(TARGET kaji-example-gedanken PROPERTY C_STANDARD 17)
add_dependencies(kaji-example-gedanken kaji)
target_link_options(kaji-example-gedanken PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>)
target_link_libraries(kaji-example-gedanken kaji)