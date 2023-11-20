#include <tf2_ros/buffer.h>
#include "tf2_ros/transform_broadcaster.h"
#include "rclcpp/rclcpp.hpp"
#include <std_srvs/srv/set_bool.hpp>

#include <iostream>
#include <memory>
#include <rclcpp/node.hpp>
#include <string>
#include <vector>

class PublisherNode : public rclcpp::Node {
public:
  PublisherNode() : Node("tf_publisher") {
    broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(30), std::bind(&PublisherNode::publishTF, this));

  toggle_service_ = create_service<std_srvs::srv::SetBool>(
    "toggle_publisher",
    std::bind(
      &PublisherNode::toggleCB, this, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3));
  old_time_ = now();

  }

private:
  void publishTF() {
      geometry_msgs::msg::TransformStamped transform_stamped;
    if (enabled_) {
      transform_stamped.header.stamp = now();
      old_time_ = transform_stamped.header.stamp;
      transform_stamped.header.frame_id = "base_frame";
      transform_stamped.child_frame_id = "child_frame";
      transform_stamped.transform.translation.x = 1.0;
      transform_stamped.transform.rotation.w = 1.0;
      broadcaster_->sendTransform(transform_stamped);
    }
    else {
      transform_stamped.header.stamp = old_time_;
      transform_stamped.header.frame_id = "base_frame";
      transform_stamped.child_frame_id = "child_frame";
      transform_stamped.transform.translation.x = 1.0;
      transform_stamped.transform.rotation.w = 1.0;

      broadcaster_->sendTransform(transform_stamped);
    }
  }

  void toggleCB(
      const std::shared_ptr<rmw_request_id_t> request_header,
      const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
      std::shared_ptr<std_srvs::srv::SetBool::Response> response)
  {
  enabled_ = request->data;
  response->success = true;
  if (enabled_) {
    response->message = "TF publisher Enabled";
  } else {
    response->message = "TF publisher Disabled";
  }
  }


  std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr toggle_service_;
  bool enabled_;
  builtin_interfaces::msg::Time old_time_;


};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PublisherNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
