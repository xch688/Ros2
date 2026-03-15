#include "rclcpp/rclcpp.hpp"
#include <QApplication>
#include <QLabel>
#include <QString>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QLabel* label = new QLabel("Hello Qt!");
    label->show();
    app.exec();

    return 0;
}