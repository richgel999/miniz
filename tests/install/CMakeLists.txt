cmake_minimum_required(VERSION 3.5)
project(miniz_install_test C)

find_package(miniz CONFIG REQUIRED)
enable_testing()

foreach(include_style plain subdir)
  add_executable(${include_style} main.c)
  target_link_libraries(${include_style} miniz::miniz)
  add_test(NAME ${include_style} COMMAND ${include_style})
endforeach()
target_compile_definitions(subdir PRIVATE INCLUDE_MINIZ_SUBDIR)
