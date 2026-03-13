#include "rclcpp/logger.hpp"
#include "rclcpp/rclcpp.hpp"

class ExpandNode : public rclcpp::Node {
public:
    explicit ExpandNode(const std::string& name)
        : rclcpp::Node(name)
    {}

    void func() { RCLCPP_INFO(get_logger(), "Hello ROS2 C++ World!"); }
};

int main(int argc, char** argv)
{

    rclcpp::init(argc, argv);
    auto node = std::make_shared<ExpandNode>("ros2_cpp_node");
    node->func();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
