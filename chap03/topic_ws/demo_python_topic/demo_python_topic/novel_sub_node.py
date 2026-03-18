import rclpy
from rclpy.node import Node
from example_interfaces.msg import String
import threading
from queue import Queue
import time

class NovelSubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.novel_subscriber = self.create_subscription(String, 'novel', self.novel_callback, 10)
        
    def novel_callback(self, msg):
        self.get_logger().info(f"{msg.data}")


def main(args = None):
    rclpy.init()
    node = NovelSubNode("novel_sub_node")
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()