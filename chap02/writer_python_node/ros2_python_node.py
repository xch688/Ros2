import rclpy
from rclpy.node import Node

class WriterNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)

def main():
    rclpy.init()
    node = WriterNode("python_node")
    node.get_logger().info("Hello, Ros2...")
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()