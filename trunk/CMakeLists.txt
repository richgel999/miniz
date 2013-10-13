PROJECT(miniz)
cmake_minimum_required(VERSION 2.8)
option(BUILD_X64 "build 64-bit" TRUE)

message("Initial BUILD_X64=${BUILD_X64}")
message("Initial CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

message( ${PROJECT_NAME} " build type: " ${CMAKE_BUILD_TYPE} )

if (BUILD_X64)
	message("Building 64-bit")
else()
	message("Building 32-bit")
endif(BUILD_X64)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(EXAMPLE1_SRC_LIST ${COMMON_SRC_LIST} example1.c)
set(EXAMPLE2_SRC_LIST ${COMMON_SRC_LIST} example2.c)
set(EXAMPLE3_SRC_LIST ${COMMON_SRC_LIST} example3.c)
set(EXAMPLE4_SRC_LIST ${COMMON_SRC_LIST} example4.c)
set(EXAMPLE5_SRC_LIST ${COMMON_SRC_LIST} example5.c)
set(EXAMPLE6_SRC_LIST ${COMMON_SRC_LIST} example6.c)
set(MINIZ_TESTER_SRC_LIST ${COMMON_SRC_LIST} miniz_tester.cpp miniz.c timer.cpp timer.h)

# -fno-strict-aliasing is probably not required to build miniz.c (I've been testing with it not defined for a while), but it's what I'm used to from working with Visual Studio.
set(GCC_COMPILE_FLAGS "-fno-strict-aliasing -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64")

if (NOT BUILD_X64)
	set(GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -m32")
endif()

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} ${GCC_COMPILE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GCC_COMPILE_FLAGS} -D_DEBUG")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_COMPILE_FLAGS} -D_DEBUG")

include_directories(${PROJECT_SOURCE_DIR}/.)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin_linux)

add_executable(example1 ${EXAMPLE1_SRC_LIST})
add_executable(example2 ${EXAMPLE2_SRC_LIST})
add_executable(example3 ${EXAMPLE3_SRC_LIST})
add_executable(example4 ${EXAMPLE4_SRC_LIST})
add_executable(example5 ${EXAMPLE5_SRC_LIST})

add_executable(example6 ${EXAMPLE6_SRC_LIST})
target_link_libraries(example6 m)

add_executable(miniz_tester ${MINIZ_TESTER_SRC_LIST})
