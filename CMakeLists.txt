cmake_minimum_required (VERSION 3.0)

project (cpp_rest_servlets CXX)
set (CMAKE_CXX_STANDARD 17)
set (${PROJECT_NAME}_VERSION_MAJOR 1)
set (${PROJECT_NAME}_VERSION_MINOR 0)
set (${PROJECT_NAME}_VERSION_REVISION 0)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (Boost_USE_STATIC_LIBS         ON)  # only find static libs
set (Boost_USE_DEBUG_LIBS          OFF) # ignore debug libs and
set (Boost_USE_RELEASE_LIBS        ON)  # only find release libs
set (Boost_USE_MULTITHREADED       ON)
set (Boost_USE_STATIC_RUNTIME      OFF)
set (Boost_USE_STATIC_LIBS OFF CACHE BOOL "Link against boost statically.")

# clang-tidy ----------------------------------------------------
find_program (
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
)
if (NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else ()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
    "-checks=-*,clang-analyzer-.*,clang-analyzer-cplusplus-*,google-*,readability-*,-readability-redundant-declaration")
endif ()
# --------------------------------------------- END clang-tidy --

enable_testing ()

# GTEST FRAMEWORK -----------------------------------------------
configure_file (third_party/gtest/CMakeLists.txt.in third_party/gtest/CMakeLists.txt)
execute_process (COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/gtest
)
if (result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process (COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/gtest
)
if (result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
# ----------------------------------------- END GTEST FRAMEWORK --

find_package (Threads REQUIRED)
if (THREADS_HAVE_PTHREAD_ARG)
  add_compile_options (-pthread)
endif()
if (CMAKE_THREAD_LIBS_INIT)
  link_libraries (${CMAKE_THREAD_LIBS_INIT})
endif ()

find_package (Boost REQUIRED
  COMPONENTS
    random
    system
    thread
    filesystem
    chrono
    atomic
    date_time
    regex
)
find_package (cpprestsdk  REQUIRED)
find_package (OpenSSL     REQUIRED)

add_definitions(
  "-D_TURN_OFF_PLATFORM_STRING=1"
)

include_directories (
  ${Boost_INCLUDE_DIRS}
  cpprestsdk::cpprest
  ${PROJECT_SOURCE_DIR}/include
)

# PROJECT -------------------------------------------------------
file (GLOB src_files src/*.cpp)
add_library (${PROJECT_NAME} ${src_files})
target_link_libraries (${PROJECT_NAME}
  Boost::boost
  Boost::random
  Boost::system
  Boost::thread
  Boost::filesystem
  Boost::chrono
  Boost::atomic
  Boost::date_time
  Boost::regex
)
if (CLANG_TIDY_EXE)
  set_target_properties (
    ${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif ()
# ------------------------------------------------ END PROJECT --

# TESTS ---------------------------------------------------------
add_subdirectory(
  ${CMAKE_BINARY_DIR}/gtest-src
  ${CMAKE_BINARY_DIR}/gtest-build
  EXCLUDE_FROM_ALL
)

add_executable(service_test tests/service_test.cpp)
target_link_libraries(service_test
  gtest
  cpprestsdk::cpprest
  ${OPENSSL_LIBRARIES}
  ${PROJECT_NAME}
)
add_test(NAME test_services COMMAND service_test)
# -------------------------------------------------- END TESTS --
