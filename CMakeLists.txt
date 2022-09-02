project(swear_paper_proj)
cmake_minimum_required(VERSION 3.24)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_link_options(-fuse-ld=mold)
include(cmake/conan.cmake)

set(_FLAGS -Wall -Wextra -Wpedantic -Werror -O0 -Wfatal-errors)

find_package(fmt 9.1.0 REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)
find_package(CURL 7.67.0 REQUIRED)
find_package(OpenCV 4.1.2 REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)

add_library(core STATIC 
       src/lib/downloader.cpp
       src/lib/include/downloader.hpp
       src/lib/downloader_delegate.cpp
       src/lib/include/downloader_delegate.hpp
       src/lib/include/interfaces/downloader_delegate_b.hpp
       src/lib/earthporn.cpp
       src/lib/include/earthporn.hpp
       src/lib/image.cpp
       src/lib/include/image.hpp
       src/lib/image_delegate.cpp
       src/lib/include/image_delegate.hpp
       src/lib/include/interfaces/image_delegate_b.hpp
       src/lib/include/image_location.hpp
       src/lib/include/image_size.hpp
       src/lib/json_parse_delegate.cpp
       src/lib/include/json_parse_delegate.hpp
       src/lib/include/interfaces/json_parse_delegate_b.hpp
       src/lib/include/interfaces/program_delegate_b.hpp
       src/lib/program_delegates.cpp
       src/lib/include/program_delegates.hpp
       src/lib/include/interfaces/reddit_interface.hpp)
target_include_directories(core PUBLIC src/lib/include)

target_link_libraries(core PUBLIC 
       fmt::fmt 
       Boost::program_options 
       CURL::libcurl 
       opencv::opencv_core 
       opencv::opencv_imgcodecs
       nlohmann_json::nlohmann_json)

target_compile_features(core PUBLIC cxx_std_17)
target_compile_options(core PRIVATE ${_FLAGS})


add_executable(swear_paper src/main.cpp)
target_link_libraries(swear_paper PRIVATE core)
target_compile_options(swear_paper PRIVATE ${_FLAGS})
