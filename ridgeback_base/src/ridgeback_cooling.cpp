/**
 *
 *  \file
 *  \brief      Cooling control class for Ridgeback
 *  \author     Tony Baltovski <tbaltovski@clearpathrobotics.com>
 *  \copyright  Copyright (c) 2015, Clearpath Robotics, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Clearpath Robotics, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CLEARPATH ROBOTICS, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Please send comments, questions, or patches to code@clearpathrobotics.com
 *
 */

#include "ridgeback_base/ridgeback_cooling.h"


namespace ridgeback_base
{

RidgebackCooling::RidgebackCooling(ros::NodeHandle* nh) :
  nh_(nh),
  charger_disconnected_(true)
{
  cmd_fans_pub_ = nh_->advertise<ridgeback_msgs::Fans>("mcu/fans", 1);

  status_sub_ = nh_->subscribe("mcu/status", 1, &RidgebackCooling::statusCallback, this);
  cmd_vel_sub_ = nh_->subscribe("cmd_vel", 1, &RidgebackCooling::cmdVelCallback, this);

  for (int i = 0; i < 6; i++)
  {
    cmd_fans_msg_.fans[i] = ridgeback_msgs::Fans::FAN_ON_LOW;
  }
}

void RidgebackCooling::statusCallback(const ridgeback_msgs::Status::ConstPtr& status)
{
  if (status->charger_connected)
  {
    cmd_fans_msg_.fans[ridgeback_msgs::Fans::CHARGER_BAY_INTAKE] = ridgeback_msgs::Fans::FAN_ON_HIGH;
    cmd_fans_msg_.fans[ridgeback_msgs::Fans::CHARGER_BAY_EXHAUST] = ridgeback_msgs::Fans::FAN_ON_HIGH;
    charger_disconnected_ = false;
  }
  else if (!charger_disconnected_)
  {
    cmd_fans_msg_.fans[ridgeback_msgs::Fans::CHARGER_BAY_INTAKE] = ridgeback_msgs::Fans::FAN_ON_LOW;
    cmd_fans_msg_.fans[ridgeback_msgs::Fans::CHARGER_BAY_EXHAUST] = ridgeback_msgs::Fans::FAN_ON_LOW;
    charger_disconnected_ = true;
  }

  cmd_fans_pub_.publish(cmd_fans_msg_);
}
void RidgebackCooling::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& twist)
{
  if (twist->linear.x >= 0.1 ||
      twist->linear.y >= 0.1 ||
      twist->angular.z >= 0.4)
  {
    for (int i = 0; i < 6; i++)
    {
      cmd_fans_msg_.fans[i] = ridgeback_msgs::Fans::FAN_ON_HIGH;
    }
  }
  else
  {
    for (int i = 0; i < 6; i++)
    {
      cmd_fans_msg_.fans[i] = ridgeback_msgs::Fans::FAN_ON_LOW;
    }
  }

  cmd_fans_pub_.publish(cmd_fans_msg_);
}


}  // namespace ridgeback_base
