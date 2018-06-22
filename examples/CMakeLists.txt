set (CMAKE_CXX_STANDARD 17)
file (GLOB regist_files docker_service/*.cpp)
add_executable (registration_service ${regist_files})
target_link_libraries (registration_service PRIVATE
  cpp_rest_servlets
  cpprest
  ${Boost_LIBRARIES}
)
