# Generic support for adding a unittest.
function(add_unittest test_suite test_name)
  add_executable(${test_name} ${ARGN})

  # even though tests are executables, they should not be in bin directory
  set(outdir ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

  # linking our tests with all test libraries and main function
  target_link_libraries(${test_name} PRIVATE TestMain)
  target_link_libraries(${test_name} PRIVATE ${GTEST_BOTH_LIBRARIES})
  target_link_libraries(${test_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

  target_include_directories(${test_name} PUBLIC
    "${CMAKE_SOURCE_DIR}/unittests/framework/include")

  add_test(${test_name} ${test_name})
  add_dependencies(${test_suite} ${test_name})

  set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 17)

  get_target_property(test_suite_folder ${test_suite} FOLDER)
  if (NOT ${test_suite_folder} STREQUAL "NOTFOUND")
    set_property(TARGET ${test_name} PROPERTY FOLDER "${test_suite_folder}")
  endif ()
endfunction()
