#include "rclcpp/rclcpp.hpp"
#include "sylar/log.h"

int main(int argc, char** argv)
{
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Hello ROS2 C++ World!";
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("ros2_cpp_node");
    RCLCPP_INFO(node->get_logger(), "Hello ROS2 C++ World!");
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
