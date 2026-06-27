# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../Libnow")
idf_build_set_property(MINIMAL_BUILD ON)
project(Control)

idf_build_get_property(build_component_targets BUILD_COMPONENT_TARGETS)