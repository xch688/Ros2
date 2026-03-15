import face_recognition
import cv2
import os
from ament_index_python.packages import get_package_share_directory


def main():
    image_path = os.path.join(get_package_share_directory("demo_pothon_service"), "resource/zidane.jpg")
    print(f"image path: {image_path}")
    image = cv2.imread(image_path)
    if image is None:
        raise FileNotFoundError(f"Failed to load image: {image_path}")

    # face_recognition expects RGB input; HOG is much lighter than CNN for demos.
    rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    face_locations = face_recognition.face_locations(
        rgb_image,
        number_of_times_to_upsample=2,
        model="hog",
    )

    print(f"detected faces: {len(face_locations)}")

    for top, right, bottom, left in face_locations:
        cv2.rectangle(image, (left, top), (right, bottom), (255, 0, 0), 4)
    cv2.imshow("face_detect", image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
