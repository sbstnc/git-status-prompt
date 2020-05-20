cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(git_status_prompt VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

if(UNIX)
  set(IS_UNIX 1)
endif()

configure_file(${PROJECT_SOURCE_DIR}/src/config.h.in ${PROJECT_BINARY_DIR}/config.h)

add_subdirectory(src)
