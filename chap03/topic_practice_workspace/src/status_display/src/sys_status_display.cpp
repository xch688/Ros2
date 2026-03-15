#include <QApplication>
#include <QLabel>
#include <QString>

#include "rclcpp/rclcpp.hpp"
#include "status_interfaces/msg/system_status.hpp"

class SystemStatusDisplay : public rclcpp::Node {
public:
    SystemStatusDisplay()
        : Node("system_status_display")
    {
        subscription_ = create_subscription<status_interfaces::msg::SystemStatus>(
            "sys_status",
            10,
            [&](const status_interfaces::msg::SystemStatus::SharedPtr msg) -> void {
                label_->setText(get_qStr_from_msg(msg));
            });
        label_ =
            new QLabel(get_qStr_from_msg(std::make_shared<status_interfaces::msg::SystemStatus>()));
        label_->show();
    }

private:
    QString get_qStr_from_msg(const status_interfaces::msg::SystemStatus::SharedPtr msg)
    {
        std::stringstream ss;
        ss << "*********系统状态可视化工具*********\n"
           << "数据时间: " << msg->stamp.sec << "\ts\n"
           << "用户名: " << msg->host_name << "\t\n"
           << "CPU使用率: " << msg->cpu_percent << "\t%\n"
           << "内存使用率: " << msg->memory_percent << "\t%\n"
           << "内存总大小: " << msg->memory_total << "\tMB\n"
           << "剩余有效内存: " << msg->memory_available << "\tMB\n"
           << "网络发送量: " << msg->net_sent << "\tMB\n"
           << "网络接受量: " << msg->net_recv << "\tMB\n"
           << "*************************************";
        return QString::fromStdString(ss.str());
    }

private:
    rclcpp::Subscription<status_interfaces::msg::SystemStatus>::SharedPtr subscription_;
    QLabel* label_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);
    auto node = std::make_shared<SystemStatusDisplay>();
    std::thread spinThread([&]() { rclcpp::spin(node); });
    spinThread.detach();
    app.exec();
    rclcpp::shutdown();

    return 0;
}
