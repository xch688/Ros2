import rclpy
from rclpy.node import Node
import requests
from example_interfaces.msg import String
from queue import Queue

class NovelPubNobe(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.queue_ = Queue()
        self.novel_publisher_ = self.create_publisher(String,'novel', 10)
        self.timer_ = self.create_timer(5, self.timer_callback)

    def download_novel(self, url):
        response = requests.get(url)
        response.encoding = "utf-8"
        self.get_logger().info(f"下载完成: P{url}")
        for line in response.text.splitlines():
            self.queue_.put(line)

    
    def timer_callback(self):
        if (self.queue_.qsize() > 0):
            msg = String()
            msg.data = self.queue_.get()
            self.novel_publisher_.publish(msg)
            self.get_logger().info(f"发布了一行小说: {msg.data}")

def main(args = None):
    rclpy.init()
    node = NovelPubNobe("novel_pub_node")
    node.download_novel("http://127.0.0.1:8000/novel1.txt")
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()