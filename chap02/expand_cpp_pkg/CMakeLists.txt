cmake_minimum_required(VERSION 3.8)
project(expand_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

aux_source_directory(${CMAKE_SOURCE_DIR}/sylar LOG_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ROS_SRCS)
add_executable(expand_cpp_node ${LOG_SRCS} ${ROS_SRCS})

target_include_directories(expand_cpp_node PUBLIC ${CMAKE_SOURCE_DIR})
# 将会自动包含rclcpp的库文件和头文件
ament_target_dependencies(expand_cpp_node rclcpp)
install(TARGETS expand_cpp_node DESTINATION lib/${PROJECT_NAME})


# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
