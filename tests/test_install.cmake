# Exercise the installed target with a non-default header directory and both
# supported include spellings, without relying on source-tree include paths.
function(run_test_command)
  execute_process(COMMAND ${ARGV}
    WORKING_DIRECTORY "${work_dir}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "${ARGV} failed (${result}):\n${output}\n${error}")
  endif()
endfunction()

if(NOT CONFIG)
  set(CONFIG Release)
endif()

file(REMOVE_RECURSE "${TEST_DIR}")
file(MAKE_DIRECTORY "${TEST_DIR}/build" "${TEST_DIR}/consumer")
set(prefix "${TEST_DIR}/prefix")
set(toolchain_arg)
if(TOOLCHAIN_FILE)
  list(APPEND toolchain_arg "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}")
endif()

set(generator_args -G "${GENERATOR}")
if(GENERATOR_PLATFORM)
  list(APPEND generator_args -A "${GENERATOR_PLATFORM}")
endif()
if(GENERATOR_TOOLSET)
  list(APPEND generator_args -T "${GENERATOR_TOOLSET}")
endif()

set(work_dir "${TEST_DIR}/build")
run_test_command("${CMAKE_COMMAND}" "${SOURCE_DIR}" ${generator_args}
  "-DCMAKE_C_COMPILER=${C_COMPILER}" ${toolchain_arg}
  "-DCMAKE_INSTALL_PREFIX=${prefix}"
  -DCMAKE_INSTALL_INCLUDEDIR=headers
  -DCMAKE_INSTALL_LIBDIR=lib
  -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DINSTALL_PROJECT=ON
  "-DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}"
  "-DAMALGAMATE_SOURCES=${AMALGAMATE_SOURCES}"
  "-DBUILD_HEADER_ONLY=${BUILD_HEADER_ONLY}")
run_test_command("${CMAKE_COMMAND}" --build . --target install --config "${CONFIG}")

set(work_dir "${TEST_DIR}/consumer")
run_test_command("${CMAKE_COMMAND}" "${SOURCE_DIR}/tests/install" ${generator_args}
  "-DCMAKE_C_COMPILER=${C_COMPILER}" ${toolchain_arg}
  "-Dminiz_DIR=${prefix}/lib/cmake/miniz")
run_test_command("${CMAKE_COMMAND}" --build . --config "${CONFIG}")
if(WIN32)
  set(ENV{PATH} "${prefix}/bin;$ENV{PATH}")
endif()
run_test_command("${CMAKE_CTEST_COMMAND}" -C "${CONFIG}" --output-on-failure)
