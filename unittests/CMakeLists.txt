# Copied from llvm/unittests/CMakeLists.txt
add_custom_target(UnitTests)
set_target_properties(UnitTests PROPERTIES FOLDER "Tests")

# People tend to run the tests _before_ installing, so we don't want the install
# rpath here.
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)

function(add_hammock_unittest test_dirname)
  add_unittest(UnitTests ${test_dirname} ${ARGN})
endfunction()

# all test-related infrastructure is in 'framework'
add_subdirectory(framework)

add_subdirectory(splay)
