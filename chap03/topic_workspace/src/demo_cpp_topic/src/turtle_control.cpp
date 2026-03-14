#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

class TurtleControl : public rclcpp::Node {
public:
    explicit TurtleControl(const std::string& name)
        : Node(name)
    {
        velocityPublisher_ = create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        velocitySubscriber_ = create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10, [this](const turtlesim::msg::Pose::SharedPtr msg) -> void {
                const double dx = target_x_ - msg->x;
                const double dy = target_y_ - msg->y;
                const double distance = std::hypot(dx, dy);
                const double target_heading = std::atan2(dy, dx);
                const double heading_error = normalize_angle(target_heading - msg->theta);

                auto message = geometry_msgs::msg::Twist();
                if (distance <= distance_tolerance_) {
                    // Reached target: stop and avoid jitter around the goal.
                    velocityPublisher_->publish(message);
                    return;
                }

                message.angular.z =
                    clamp(k_angular_ * heading_error, -max_angular_speed_, max_angular_speed_);

                // Move fast when roughly facing the target; slow down if heading error is large.
                const double heading_gate = std::max(0.0, std::cos(heading_error));
                const double raw_linear = k_linear_ * distance * heading_gate;
                message.linear.x = clamp(raw_linear, min_linear_speed_, max_linear_speed_);

                velocityPublisher_->publish(message);
            });
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocityPublisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr velocitySubscriber_;

    static double normalize_angle(double angle)
    {
        while (angle > M_PI) {
            angle -= 2.0 * M_PI;
        }
        while (angle < -M_PI) {
            angle += 2.0 * M_PI;
        }
        return angle;
    }

    static double clamp(double value, double min_value, double max_value)
    {
        return std::clamp(value, min_value, max_value);
    }

    double target_x_{9.0};
    double target_y_{2.0};
    double k_linear_{1.8};
    double k_angular_{4.0};
    double max_linear_speed_{2.0};
    double min_linear_speed_{0.05};
    double max_angular_speed_{3.0};
    double distance_tolerance_{0.05};
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControl>("turtle_control");
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
