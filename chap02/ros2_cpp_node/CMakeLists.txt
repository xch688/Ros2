cmake_minimum_required(VERSION 3.8)
project(ros2_cpp_node)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_executable(${PROJECT_NAME} ${SRCS})

ament_target_dependencies(${PROJECT_NAME} rclcpp)
