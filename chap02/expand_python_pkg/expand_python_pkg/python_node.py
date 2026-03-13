import rclpy
from rclpy.node import Node

class ExpandNode(Node):
    def __init__(self, node_name):
        rclpy.init()
        super().__init__(node_name)

    def start(self):
        self.get_logger().info("hello world")
        rclpy.spin(self)
        self.destroy_node()
        rclpy.shutdown()

def main(args=None):
    node = ExpandNode("python_node")
    node.start()

if __name__ == "__main__":
    main()
