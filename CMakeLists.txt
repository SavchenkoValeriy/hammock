cmake_minimum_required(VERSION 3.4.3)

project(hammock
  ${cmake_3_0_PROJ_VERSION}
  ${cmake_3_0_LANGUAGES}
  CXX)

# Add path to custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  )

include(AddHammock)

# All includes are in include directory
set(HAMMOCK_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Google test
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${HAMMOCK_INCLUDE_DIRS})

# All unit-tests are in 'unittests'
add_subdirectory(unittests)
