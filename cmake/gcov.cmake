
file(DOWNLOAD https://raw.githubusercontent.com/bilke/cmake-modules/master/CodeCoverage.cmake ${PROJECT_BINARY_DIR}/CMakeModules/CodeCoverage.cmake)

set(CMAKE_MODULE_PATH ${PROJECT_BINARY_DIR}/CMakeModules)
include(CodeCoverage)