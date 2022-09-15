project(swear_paper_proj)
cmake_minimum_required(VERSION 3.24)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_link_options(-fuse-ld=mold)
include(cmake/conan.cmake)
include(cmake/cpplint.cmake)
include(cmake/gcov.cmake)
include(cmake/ccache.cmake)

set(_FLAGS -Wall -Wextra -Wpedantic -Werror -O0 -Wfatal-errors)

find_package(fmt 9.1.0 REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)
find_package(CURL 7.67.0 REQUIRED)
find_package(OpenCV 4.1.2 REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)
find_package(spdlog 1.10.0 REQUIRED)

add_library(core STATIC 
    src/lib/include/interfaces/configuration_interface.hpp
    src/lib/include/interfaces/reddit_interface.hpp
    src/lib/include/interfaces/downloader_interface.hpp
    src/lib/include/image_location.hpp
    src/lib/include/cache_or_download.hpp
    src/lib/include/image_size.hpp
    src/lib/include/fmt_optional.hpp
    src/lib/include/downloader.hpp
    src/lib/include/environment_configuration.hpp
    src/lib/environment_configuration.cpp
    src/lib/downloader.cpp
    src/lib/cache_or_download.cpp
    src/lib/include/earthporn.hpp
    src/lib/earthporn.cpp
    src/lib/include/image.hpp
    src/lib/image.cpp
       )
target_include_directories(core PUBLIC src/lib/include)

target_link_libraries(core PUBLIC 
       fmt::fmt 
       Boost::program_options 
       CURL::libcurl 
       opencv::opencv_core 
       opencv::opencv_imgcodecs
       nlohmann_json::nlohmann_json
       spdlog::spdlog)

target_compile_features(core PUBLIC cxx_std_17)
target_compile_options(core PRIVATE ${_FLAGS})


add_executable(swear_paper src/main.cpp)
target_link_libraries(swear_paper PRIVATE core)
target_compile_options(swear_paper PRIVATE ${_FLAGS})

find_package(GTest REQUIRED)
add_executable(swear_paper_test
    test/expected_reply.hpp
    test/earthporn_json_parse_test.cpp
    test/downloader_test.cpp
    test/environment_configuration_test.cpp
    test/cache_or_download_test.cpp
    )
target_compile_options(swear_paper_test PRIVATE ${_FLAGS} -Wno-unused-but-set-variable)
target_link_libraries(swear_paper_test PRIVATE core GTest::gtest_main GTest::gmock)

option(LINT "cpplint" OFF)
if(${LINT})
    message(STATUS "Enabling cpplint")
    add_style_check_target(core)
endif()

option(ANALYSE "clang-tidy" OFF)
if(${ANALYSE})
    message(STATUS "Enabling clang-tidy")
    include(cmake/clang-tidy.cmake)
endif()

option(COVERAGE "gcov html report" OFF)
if(${COVERAGE})
    message(STATUS "Enabling coverage")
    target_compile_options(core PRIVATE ${_FLAGS} --coverage)
    target_link_libraries(core PRIVATE gcov) 
    setup_target_for_coverage_gcovr_html(NAME core_coverage EXECUTABLE swear_paper_test)
endif()
