import rclpy
from rclpy.node import Node
from interface.srv import FaceDetector   # 导入接口
from ament_index_python.packages import get_package_share_directory
from cv_bridge import CvBridge     # ROS图片转换为opencv图片工具
import face_recognition
import time
import os
import cv2

class FaceDetectNode(Node):
    def __init__(self):
        super().__init__("face_detection_node")
        self.bridge = CvBridge()
        self.service = self.create_service(FaceDetector, "/face_detect", self.detectFace_callback)
        self.default_imge = os.path.join(get_package_share_directory("demo_pothon_service"), "resource/zidane.jpg") 
        self.get_logger().info("default image: {self.default_imge}")
        self.upsample_times = 1
        self.model = "hog"
        
    def detectFace_callback(self, request, response):
        # 若没有图片，则采用默认的图片进行识别
        if request.image.data:
            cv_image = self.bridge.imgmsg_to_cv2(request.image)
        else:
            cv_image = cv2.imread(self.default_imge)
            
        self.get_logger().info("图片加载完成，开始人脸检测")
        start_time = time.time()
        face_locations = face_recognition.face_locations(cv_image, 
                                                         number_of_times_to_upsample=self.upsample_times,
                                                         model= self.model)
        end_time = time.time()
        self.get_logger().info("检测完成，耗时: {end_time - start_time}")
        
        response.number = len(face_locations)
        response.use_time = end_time - start_time
        for top, right, bottom, left in face_locations:
            response.top.append(top)
            response.right.append(right)
            response.bottom.append(bottom)
            response.left.append(left)
        
        return response
    
def main(args = None):
    rclpy.init(args=args)
    node = FaceDetectNode()
    rclpy.spin(node)
    rclpy.shutdown()