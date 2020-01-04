add_library(core STATIC downloader.cpp ansi_codes.cpp downloader_delegate.cpp earthporn.cpp image.cpp image_delegate.cpp image_location.cpp image_size.cpp json_parse_delegate.cpp program_delegates.cpp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(swear_paper main.cpp)

message(STATUS "Conan libs = ${CONAN_LIBS}")
target_link_libraries(core PRIVATE ${CONAN_LIBS})
target_link_libraries(swear_paper PRIVATE core ${CONAN_LIBS})

target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_options(swear_paper PRIVATE -Wall -Wextra -Wpedantic -Werror)
