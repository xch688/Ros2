cmake_minimum_required(VERSION 3.12)
project(ros2_cpp_node)

find_package(rclcpp REQUIRED)

aux_source_directory(${CMAKE_SOURCE_DIR}/sylar LOG_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ROS_SRCS)
message(STATUS ${LOG_SRCS})
add_executable(${PROJECT_NAME} ${LOG_SRCS} ${ROS_SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${rclcpp_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} ${rclcpp_LIBRARIES})
