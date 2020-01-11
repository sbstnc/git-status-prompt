cmake_minimum_required(VERSION 3.0)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(git_status_prompt VERSION 0.1.0 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(Libgit2 REQUIRED)

add_executable(git_status_prompt src/gitprompt.c)
target_link_libraries(git_status_prompt PUBLIC LibGit2::LibGit2)

IF(APPLE)
  find_library(COREFOUNDATION CoreFoundation)
  find_library(SECURITY Security)
  target_link_libraries(git_status_prompt PUBLIC iconv z ${COREFOUNDATION} ${SECURITY})
ENDIF(APPLE)
