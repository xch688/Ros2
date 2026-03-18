#include "rclcpp/rclcpp.hpp"
#include <memory>

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("ros2_cpp_node");
    RCLCPP_INFO(node->get_logger(), "hello, ROS2...");
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
