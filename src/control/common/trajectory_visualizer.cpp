#include "control/common/trajectory_visualizer.h"

#include <algorithm>

#include "common/macro.h"

namespace autoagric {
namespace control {
namespace common {

using autoware_msgs::LaneConstPtr;
using visualization_msgs::Marker;
using visualization_msgs::MarkerArray;
using MarkerType = std::pair<MarkerArray, MarkerArray>;

TrajectoryVisualizer::TrajectoryVisualizer(ros::NodeHandle& nh) {
  nh_queue_.emplace_back(nh);
}

TrajectoryVisualizer::TrajectoryVisualizer(
    ros::NodeHandle& nh, const std::vector<std::string>& names) {
  for (auto& name : names) {
    nh_queue_.emplace_back(ros::NodeHandle(nh, name));
  }
}

TrajectoryVisualizer::~TrajectoryVisualizer() {
  nh_queue_.clear();
  publisher_queue_.clear();
}

void TrajectoryVisualizer::Init() {
  for (auto& nh : nh_queue_) {
    publisher_queue_.emplace_back(
        std::make_pair<ros::Publisher, ros::Publisher>(
            nh.advertise<MarkerArray>("arrows", 10),
            nh.advertise<MarkerArray>("points_and_line", 10)));
  }
}

void TrajectoryVisualizer::Proc(const std::vector<MarkerType>& markers) {
  int visualizer_num = std::min<int>(markers.size(), publisher_queue_.size());

  AERROR_IF(markers.size() == publisher_queue_.size(),
            "numbers of markers and publishers are not equal");

  for (int i = 0; i < visualizer_num; i++) {
    auto& publisher = publisher_queue_[i];
    publisher.first.publish(markers[i].first);
    publisher.second.publish(markers[i].second);
  }
}

MarkerType TrajectoryVisualizer::LaneToMarkerArray(
    const autoware_msgs::Lane& msg, const geometry_msgs::Vector3& scale,
    const std_msgs::ColorRGBA& color) {
  autoware_msgs::LaneConstPtr lane(new autoware_msgs::Lane(msg));
  Marker points, line_strip;
  MarkerArray arrows, points_and_line;

  points.header.frame_id = line_strip.header.frame_id = lane->header.frame_id;
  points.header.stamp = line_strip.header.stamp = lane->header.stamp;
  points.ns = line_strip.ns = "points_and_lines";
  points.id = 0;
  line_strip.id = 1;

  points.type = Marker::POINTS;
  line_strip.type = Marker::LINE_STRIP;

  points.scale = line_strip.scale = scale;
  points.color = line_strip.color = color;
  // points.lifetime = line_strip.lifetime = ros::Duration(0);

  for (uint32_t i = 0; i < lane->waypoints.size(); ++i) {
    geometry_msgs::Point p = lane->waypoints[i].pose.pose.position;

    points.points.push_back(p);
    line_strip.points.push_back(p);

    Marker arrow;
    arrow.pose = lane->waypoints[i].pose.pose;
    arrow.header.frame_id = lane->header.frame_id;
    arrow.id = i;
    arrow.header.stamp = lane->header.stamp;
    arrow.type = Marker::ARROW;
    arrow.scale = scale;
    arrow.color = color;
    // arrow.lifetime = ros::Duration(0);
    arrows.markers.push_back(arrow);
  }

  points_and_line.markers.push_back(points);
  points_and_line.markers.push_back(line_strip);

  return std::make_pair(arrows, points_and_line);
}

MarkerType TrajectoryVisualizer::LaneToMarkerArray(
    const autoware_msgs::LaneConstPtr& lane) {
  Marker points, line_strip;
  MarkerArray arrows, points_and_line;

  points.header.frame_id = line_strip.header.frame_id = lane->header.frame_id;
  points.header.stamp = line_strip.header.stamp = lane->header.stamp;
  points.ns = line_strip.ns = "points_and_lines";
  points.id = 0;
  line_strip.id = 1;

  points.type = Marker::POINTS;
  line_strip.type = Marker::LINE_STRIP;

  points.scale.x = 0.2;
  points.scale.y = 0.2;

  line_strip.scale.x = 0.2;
  line_strip.scale.y = 0.2;

  points.color.g = 1.0f;
  points.color.a = 1.0;

  line_strip.color.b = 1.0;
  line_strip.color.a = 1.0;

  for (uint32_t i = 0; i < lane->waypoints.size(); ++i) {
    geometry_msgs::Point p = lane->waypoints[i].pose.pose.position;

    points.points.push_back(p);
    line_strip.points.push_back(p);

    Marker arrow;
    arrow.pose = lane->waypoints[i].pose.pose;
    arrow.header.frame_id = lane->header.frame_id;
    arrow.id = i;
    arrow.header.stamp = lane->header.stamp;
    arrow.type = Marker::ARROW;
    arrow.scale.x = 0.2;
    arrow.scale.y = 0.2;
    arrow.color.g = 0.5;
    arrow.color.b = 0.5;
    arrow.color.a = 1.0;
    arrows.markers.push_back(arrow);
  }

  points_and_line.markers.push_back(points);
  points_and_line.markers.push_back(line_strip);

  return std::make_pair(arrows, points_and_line);
}

}  // namespace common
}  // namespace control
}  // namespace autoagric