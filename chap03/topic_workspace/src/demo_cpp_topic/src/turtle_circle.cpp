#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

#include <chrono>

class TurtleCircle : public rclcpp::Node {
public:
    explicit TurtleCircle(const std::string& node_name)
        : Node(node_name)
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), [this]() -> void {
            geometry_msgs::msg::Twist msg;
            msg.linear.x = 1;
            msg.linear.y = 0.5;
            msg.angular.z = 0.5;
            publisher_->publish(msg);
        });
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleCircle>("turtle_circle");
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
