add_library(core STATIC downloader.cpp downloader_delegate.cpp earthporn.cpp image.cpp image_delegate.cpp  json_parse_delegate.cpp program_delegates.cpp)


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
find_package(CURL 7.67.0 REQUIRED  )
find_package(OpenCV 4.1.2 REQUIRED  )
find_package(nlohmann_json 3.11.2 REQUIRED  )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(swear_paper main.cpp)

target_link_libraries(core PUBLIC fmt Boost::program_options CURL::libcurl ${OpenCV_LIBS} nlohmann_json::nlohmann_json)
target_link_libraries(swear_paper PRIVATE core)
set(_FLAGS -Wall -Wextra -Wpedantic -Werror -O0 -Wfatal-errors)

target_compile_options(core PRIVATE ${_FLAGS})
target_compile_options(swear_paper PRIVATE ${_FLAGS})
