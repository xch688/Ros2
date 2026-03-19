#include <algorithm>
#include <cmath>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"

class TurtleController : public rclcpp::Node {
public:
    TurtleController()
        : Node("turtle_control")
    {
        posSubscriber_ = create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10, [this](const turtlesim::msg::Pose::SharedPtr msg) {
                const double deltaX = targetX_ - msg->x;
                const double deltaY = targetY_ - msg->y;
                const double distance = std::hypot(deltaX, deltaY);

                geometry_msgs::msg::Twist twist;
                if (distance < stopDistance_) {
                    twistPublisher_->publish(twist);
                    RCLCPP_INFO_THROTTLE(get_logger(),
                                         *get_clock(),
                                         1000,
                                         "Target reached at (%.2f, %.2f)",
                                         msg->x,
                                         msg->y);
                    return;
                }

                const double targetTheta = std::atan2(deltaY, deltaX);
                const double angleError = std::atan2(std::sin(targetTheta - msg->theta),
                                                     std::cos(targetTheta - msg->theta));

                twist.linear.x = std::clamp(kLinear_ * distance, 0.0, maxLinearSpeed_);
                twist.angular.z =
                    std::clamp(kAngular_ * angleError, -maxAngularSpeed_, maxAngularSpeed_);
                twistPublisher_->publish(twist);
            });
        twistPublisher_ = create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
    }

private:
    double targetX_{11.0};
    double targetY_{11.0};
    double stopDistance_{0.1};
    double kLinear_{1.5};
    double kAngular_{6.0};
    double maxLinearSpeed_{3.0};
    double maxAngularSpeed_{4.0};
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twistPublisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr posSubscriber_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleController>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
