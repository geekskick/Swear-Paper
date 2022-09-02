project(swear_paper_proj)
cmake_minimum_required(VERSION 3.24)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_link_options(-fuse-ld=mold)
add_subdirectory(src)

