#include "rclcpp/rclcpp.hpp"
#include <memory>

class WriterNode : public rclcpp::Node {
public:
    WriterNode()
        : Node("writer_node")
    {}
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<WriterNode>();
    RCLCPP_INFO(node->get_logger(), "hello, ROS21...");
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
