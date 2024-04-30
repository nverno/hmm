cmake_minimum_required(VERSION 3.11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
  tsmeta
  VERSION 1.0
  LANGUAGES C CXX)

# Output directory.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(tsmeta SHARED src/tsmeta.cc)

set_target_properties(tsmeta PROPERTIES VERSION ${PROJECT_VERSION}
  PREFIX "")
