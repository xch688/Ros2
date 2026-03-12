import rclpy


def main():
    rclpy.init()
    node = rclpy.create_node("python_node")
    node.get_logger().info("Hello World")
    print("Hello World")
    rclpy.spin(node)
    rclpy.shutdown()