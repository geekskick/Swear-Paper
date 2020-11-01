add_library(core STATIC downloader.cpp downloader_delegate.cpp earthporn.cpp image.cpp image_delegate.cpp  json_parse_delegate.cpp program_delegates.cpp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(swear_paper main.cpp)

message(STATUS "Conan libs = ${CONAN_LIBS}")
target_link_libraries(core PRIVATE ${CONAN_LIBS})
target_link_libraries(swear_paper PRIVATE core ${CONAN_LIBS})

target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic -Werror -O3 -fno-rtti)
target_compile_options(swear_paper PRIVATE -Wall -Wextra -Wpedantic -Werror -O3 -fno-rtti)
