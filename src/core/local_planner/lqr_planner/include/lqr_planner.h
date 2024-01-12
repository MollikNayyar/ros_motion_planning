/***********************************************************
 *
 * @file: lqr_planner.h
 * @breif: Contains the linear quadratic regulator (LQR) local planner class
 * @author: Yang Haodong
 * @update: 2024-1-12
 * @version: 1.0
 *
 * Copyright (c) 2024 Yang Haodong
 * All rights reserved.
 * --------------------------------------------------------
 *
 **********************************************************/
#ifndef LQR_PLANNER_H
#define LQR_PLANNER_H

#include <geometry_msgs/PointStamped.h>
#include <tf2/utils.h>

#include "local_planner.h"

namespace lqr_planner
{
/**
 * @brief A class implementing a local planner using the LQR
 */
class LQRPlanner : public nav_core::BaseLocalPlanner, local_planner::LocalPlanner
{
public:
  /**
   * @brief Construct a new LQR planner object
   */
  LQRPlanner();

  /**
   * @brief Construct a new LQR planner object
   */
  LQRPlanner(std::string name, tf2_ros::Buffer* tf, costmap_2d::Costmap2DROS* costmap_ros);

  /**
   * @brief Destroy the LQR planner object
   */
  ~LQRPlanner();

  /**
   * @brief Initialization of the local planner
   * @param name        the name to give this instance of the trajectory planner
   * @param tf          a pointer to a transform listener
   * @param costmap_ros the cost map to use for assigning costs to trajectories
   */
  void initialize(std::string name, tf2_ros::Buffer* tf, costmap_2d::Costmap2DROS* costmap_ros);

  /**
   * @brief Set the plan that the controller is following
   * @param orig_global_plan the plan to pass to the controller
   * @return  true if the plan was updated successfully, else false
   */
  bool setPlan(const std::vector<geometry_msgs::PoseStamped>& orig_global_plan);

  /**
   * @brief  Check if the goal pose has been achieved
   * @return True if achieved, false otherwise
   */
  bool isGoalReached();

  /**
   * @brief Given the current position, orientation, and velocity of the robot, compute the velocity commands
   * @param cmd_vel will be filled with the velocity command to be passed to the robot base
   * @return  true if a valid trajectory was found, else false
   */
  bool computeVelocityCommands(geometry_msgs::Twist& cmd_vel);

protected:
  /**
   * @brief calculate the look-ahead distance with current speed dynamically
   * @param vt  the current speed
   * @return L  the look-ahead distance
   */
  double _getLookAheadDistance(double vt);

  /**
   * @brief find the point on the path that is exactly the lookahead distance away from the robot
   * @param lookahead_dist    the lookahead distance
   * @param robot_pose_global the robot's pose  [global]
   * @param prune_plan        the pruned plan
   * @return point            the lookahead point
   */
  geometry_msgs::PointStamped _getLookAheadPoint(double lookahead_dist, geometry_msgs::PoseStamped robot_pose_global,
                                                 const std::vector<geometry_msgs::PoseStamped>& prune_plan);

  /**
   * @brief Prune the path, removing the waypoints that the robot has already passed and distant waypoints
   * @param robot_pose_global the robot's pose  [global]
   * @return pruned path
   */
  std::vector<geometry_msgs::PoseStamped> _prune(const geometry_msgs::PoseStamped robot_pose_global);

private:
  /**
   * @brief Execute LQR control process
   * @param x   state error vector
   * @param ref reference point
   * @return u  control vector
   */
  Eigen::Vector2d _lqrControl(Eigen::Vector3d x, std::vector<double> ref);

protected:
  double lookahead_time_;      // lookahead time gain
  double min_lookahead_dist_;  // minimum lookahead distance
  double max_lookahead_dist_;  // maximum lookahead distance

private:
  bool initialized_;                       // initialized flag
  bool goal_reached_;                      // goal reached flag
  tf2_ros::Buffer* tf_;                    // transform buffer
  costmap_2d::Costmap2DROS* costmap_ros_;  // costmap(ROS wrapper)

  double d_t_;         // control time interval
  Eigen::Matrix3d Q_;  // state error matrix
  Eigen::Matrix2d R_;  // control error matrix
  int max_iter_;       // maximum iteration for ricatti solution
  double eps_iter_;    // iteration ending threshold

  std::vector<geometry_msgs::PoseStamped> global_plan_;
  ros::Publisher target_pt_pub_, current_pose_pub_;

  // goal parameters
  double goal_x_, goal_y_;
  Eigen::Vector3d goal_rpy_;
};
}  // namespace lqr_planner
#endif