add_library(core STATIC 
       lib/downloader.cpp
       lib/include/downloader.hpp
       lib/downloader_delegate.cpp
       lib/include/downloader_delegate.hpp
       lib/include/interfaces/downloader_delegate_b.hpp
       lib/earthporn.cpp
       lib/include/earthporn.hpp
       lib/image.cpp
       lib/include/image.hpp
       lib/image_delegate.cpp
       lib/include/image_delegate.hpp
       lib/include/interfaces/image_delegate_b.hpp
       lib/include/image_location.hpp
       lib/include/image_size.hpp
       lib/json_parse_delegate.cpp
       lib/include/json_parse_delegate.hpp
       lib/include/interfaces/json_parse_delegate_b.hpp
       lib/include/interfaces/program_delegate_b.hpp
       lib/program_delegates.cpp
       lib/include/program_delegates.hpp
       lib/include/interfaces/reddit_interface.hpp)
target_include_directories(core PUBLIC lib/include)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_autodetect(settings)
conan_cmake_run(CONANFILE ${PROJECT_SOURCE_DIR}/conanfile.txt
                BASIC_SETUP 
                BUILD missing
                SETTINGS ${settings})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

find_package(fmt 9.1.0 REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)
find_package(CURL 7.67.0 REQUIRED)
find_package(OpenCV 4.1.2 REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)
add_executable(swear_paper main.cpp)

target_link_libraries(core PUBLIC 
       fmt::fmt 
       Boost::program_options 
       CURL::libcurl 
       opencv::opencv_core 
       opencv::opencv_imgcodecs
       nlohmann_json::nlohmann_json)

target_link_libraries(swear_paper PRIVATE core)

set(_FLAGS -Wall -Wextra -Wpedantic -Werror -O0 -Wfatal-errors)

target_compile_features(core PUBLIC cxx_std_17)

target_compile_options(core PRIVATE ${_FLAGS})
target_compile_options(swear_paper PRIVATE ${_FLAGS})
