cmake_minimum_required(VERSION 3.8)
project(demo_cpp_topic)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)

aux_source_directory(${CMAKE_SOURCE_DIR}/sylar LOG_SRCS)
add_executable(demo_cpp_topic ${LOG_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/src/turtle_circle.cpp)

target_include_directories(demo_cpp_topic PUBLIC ${CMAKE_SOURCE_DIR})
# 将会自动包含rclcpp的库文件和头文件
ament_target_dependencies(demo_cpp_topic rclcpp geometry_msgs turtlesim)
install(TARGETS demo_cpp_topic DESTINATION lib/${PROJECT_NAME})

# 新增turtle_control.cpp
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ROS_SRCS)
add_executable(turtle_control_cpp_topic ${LOG_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/src/turtle_control.cpp)

target_include_directories(turtle_control_cpp_topic PUBLIC ${CMAKE_SOURCE_DIR})
# 将会自动包含rclcpp的库文件和头文件
ament_target_dependencies(turtle_control_cpp_topic rclcpp geometry_msgs turtlesim)
install(TARGETS turtle_control_cpp_topic DESTINATION lib/${PROJECT_NAME})
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if (BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    # the following line skips the linter which checks for copyrights
    # comment the line when a copyright and license is added to all source files
    set(ament_cmake_copyright_FOUND TRUE)
    # the following line skips cpplint (only works in a git repo)
    # comment the line when this package is in a git repo and when
    # a copyright and license is added to all source files
    set(ament_cmake_cpplint_FOUND TRUE)
    ament_lint_auto_find_test_dependencies()
endif ()

ament_package()
