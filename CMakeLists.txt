cmake_minimum_required (VERSION 3.0)

project (cpp_rest_servlets CXX)
set (CMAKE_MODULE_PATH             "${PROJECT_SOURCE_DIR}/cmake")
set (CMAKE_CXX_STANDARD            17)
set (${PROJECT_NAME}_VERSION       "1.0.0")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (Boost_USE_DEBUG_LIBS          OFF)
set (Boost_USE_RELEASE_LIBS        ON)
set (Boost_USE_MULTITHREADED       ON)
set (Boost_USE_STATIC_RUNTIME      OFF)
set (Boost_USE_STATIC_LIBS OFF CACHE BOOL "Link against boost statically.")
set (INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries.")
set (INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables.")
set (INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files"
)
set (DEF_INSTALL_CMAKE_DIR lib/CMake/${PROJECT_NAME})
set (INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files"
)
#set (BOOST_ROOT "${PROJECT_SOURCE_DIR}/third_party/boost"
#  CACHE PATH "Root of boost library."
#)

# cpprestsdk ----------------------------------------------------
set (CPPRESTSDK_ROOT  "${PROJECT_SOURCE_DIR}/third_party/cpprestsdk")
set (CPPRESTSDK_SRC   "${CPPRESTSDK_ROOT}/Release")
set (CPPRESTSDK_BUILD "${PROJECT_BINARY_DIR}/cpprestsdk")
set (BUILD_SAMPLES    OFF)
set (BUILD_TESTS      OFF)
file (MAKE_DIRECTORY  "${CPPRESTSDK_BUILD}")

if (NOT EXISTS "${CPPRESTSDK_ROOT}/patch_results.txt")
  execute_process (COMMAND
    patch -p1
    INPUT_FILE "${PROJECT_SOURCE_DIR}/third_party/cpprestsdk.patch"
    WORKING_DIRECTORY "${CPPRESTSDK_ROOT}"
    OUTPUT_FILE "${CPPRESTSDK_ROOT}/patch_results.txt"
  )
endif ()

execute_process (COMMAND
  ${CMAKE_COMMAND}
    -DBUILD_TESTS=OFF -DBUILD_SAMPLES=OFF
    -G "${CMAKE_GENERATOR}" "${CPPRESTSDK_SRC}"
  RESULT_VARIABLE result
  WORKING_DIRECTORY "${CPPRESTSDK_BUILD}"
)
if (result)
  message (FATAL_ERROR "CMake step [generate] for cpprestsdk failed: ${result}")
endif ()

execute_process (COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY "${CPPRESTSDK_BUILD}"
)
if (result)
  message (FATAL_ERROR "CMake step [build] for cpprestsdk failed: ${result}")
endif ()

add_subdirectory (
  "${CPPRESTSDK_SRC}"
  "${CPPRESTSDK_BUILD}"
)
# --------------------------------------------- END cpprestsdk --

# clang-tidy ----------------------------------------------------
find_program (
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
)
if (NOT CLANG_TIDY_EXE)
  message (STATUS "clang-tidy not found.")
else ()
  message (STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  set (DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
    "-checks=*,-readability-redundant-declaration,-fuchsia-*")
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
  message (FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()

execute_process (COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/gtest
)
if (result)
  message (FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()

add_subdirectory(
  ${CMAKE_BINARY_DIR}/gtest-src
  ${CMAKE_BINARY_DIR}/gtest-build
  EXCLUDE_FROM_ALL
)
# ---------------------------------------- END GTEST FRAMEWORK --

find_library (cpprest "${CPPRESTSDK_BUILD}/Binaries")
find_package (OpenSSL  REQUIRED)
find_package (Threads  REQUIRED)
find_package (Python2  REQUIRED
  COMPONENTS
    Interpreter
    Development
)
find_package (Boost    REQUIRED
  COMPONENTS
    system
    date_time
    regex
    python
)

add_definitions(
  "-D_TURN_OFF_PLATFORM_STRING=1"
)

include_directories (
  ${Boost_INCLUDE_DIRS}
  ${Python2_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/third_party/cpprestsdk/Release/include
)

# PROJECT -------------------------------------------------------
file (GLOB py_srcs pyservlet/*.cpp)
add_library (pyservlet MODULE ${py_srcs})
target_link_libraries (pyservlet PRIVATE
  ${Boost_LIBRARIES}
  ${Python2_LIBRARIES}
  cpprest
)
install (TARGETS pyservlet
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
)

file (GLOB src_files src/*.cpp)
file (GLOB hdr_files include/*.hpp)
add_library (${PROJECT_NAME} SHARED ${src_files} ${hdr_files})
target_link_libraries (${PROJECT_NAME} PRIVATE
  ${Boost_LIBRARIES}
  ${Python2_LIBRARIES}
  cpprest
)
if (CLANG_TIDY_EXE)
  set_target_properties (
    ${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif ()
set_target_properties (${PROJECT_NAME}
  PROPERTIES
    PUBLIC_HEADER "${hdr_files}"
)
install (TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)
# ------------------------------------------------ END PROJECT --

# TESTS ---------------------------------------------------------
add_executable(service_test tests/service_test.cpp)
target_link_libraries(service_test PRIVATE
  gtest cpprest
  ${Boost_LIBRARIES}
  ${Python2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${PROJECT_NAME}
)
add_test(NAME test_services COMMAND service_test)
# -------------------------------------------------- END TESTS --

export (TARGETS ${PROJECT_NAME}
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
)
export (PACKAGE ${PROJECT_NAME})
file (RELATIVE_PATH REL_INCLUDE_DIR
  "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}"
  "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}"
)
set (CONF_INCLUDE_DIRS
  "\${${PROJECT_NAME}_CMAKE_DIR}/${REL_INCLUDE_DIR}"
)
configure_file (${PROJECT_NAME}-config.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake" @ONLY
)
configure_file (${PROJECT_NAME}-version.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-version.cmake" @ONLY
)
install (FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
)
install (EXPORT ${PROJECT_NAME}-targets DESTINATION
  "${INSTALL_CMAKE_DIR}"
)
