cmake_minimum_required(VERSION 3.12)
project(formosana)

set(CMAKE_CXX_STANDARD 14)

#### from https://github.com/google/googletest/blob/master/googletest/README.md

include(ExternalProject)
ExternalProject_Add(googletest
        GIT_REPOSITORY    https://github.com/google/googletest.git
        GIT_TAG           master
        SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
        )

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
${CMAKE_CURRENT_BINARY_DIR}/googletest-build
EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
include_directories("${gtest_SOURCE_DIR}/include")
endif()

#### end of googletest setup fragment

include_directories(Headers)
include_directories(Headers/TaiwaneseRomanization)

add_executable(TaiwaneseRomanizationTest
        Headers/TaiwaneseRomanization/TaiwaneseRomanization.h
        Headers/TaiwaneseRomanization/VowelHelper.h
        Source/TaiwaneseRomanization/TaiwaneseRomanization.cpp
        Source/TaiwaneseRomanization/VowelHelper.cpp
        Tests/TaiwaneseRomanizationTest.cpp
)

target_link_libraries(TaiwaneseRomanizationTest gtest_main)
add_test(NAME TaiwaneseRomanizationTest COMMAND TaiwaneseRomanizationTest)


