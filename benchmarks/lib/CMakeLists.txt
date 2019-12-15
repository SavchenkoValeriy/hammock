add_executable(Benchmarks main.cpp
  insertions.cpp)

target_include_directories(Benchmarks PUBLIC
  "${CMAKE_SOURCE_DIR}/benchmarks/include")
target_link_libraries(Benchmarks PRIVATE
  benchmark::benchmark)

set_property(TARGET Benchmarks PROPERTY CXX_STANDARD 17)

add_custom_target(benchmark
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/Benchmarks"
  COMMENT "Running benchmarks...")
