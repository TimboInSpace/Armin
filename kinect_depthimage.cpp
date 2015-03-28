#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/CameraInfo.h>
#include <camera_info_manager/camera_info_manager.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("kinect/image_raw", 1);
  ros::Publisher caminfo_pub = nh.advertise<sensor_msgs::CameraInfo>("/kinect/camera_info",1);
  // Set up OpenCV capture
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cout << "Unable to open camera...";
  }
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 488);

  cv::Mat image;
  cap.read(image);
  cv::waitKey(30);
  std_msgs::Header imheader;
  imheader.frame_id = "kinect_frame";
  sensor_msgs::CameraInfo caminfo;
  camera_info_manager::CameraInfoManager cim(nh);
  cim.loadCameraInfo("file:///home/pi/kinect_calibration.yaml");
  caminfo = cim.getCameraInfo();
  caminfo.header.frame_id = "kinect_frame";
  ros::Rate loop_rate(6);
  
  while (nh.ok()) {
    cap.read(image);
    cv::waitKey(30);
    imheader.stamp = ros::Time::now();
    caminfo.header.stamp = imheader.stamp;
    sensor_msgs::ImagePtr msg = cv_bridge::CvImage(imheader, "mono16", image.rowRange(240,280)).toImageMsg();
    pub.publish(msg);
    caminfo_pub.publish(caminfo);
    ros::spinOnce();
    loop_rate.sleep();
  }
}