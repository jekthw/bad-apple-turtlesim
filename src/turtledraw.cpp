#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/set_pen.hpp"

using namespace std::chrono_literals;

class Draw : public rclcpp::Node {
private:
    double currX, currY, currTheta;
    size_t waypointIdx = 0;
    std::vector<std::pair<double,double>> waypoints;
    double goalX = 0.0;
    double goalY = 0.0;
    
    bool activatePen = 0;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub;
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr pen;
    // rclcpp::TimerBase::SharedPtr timer;

    void setPen(bool enabled) {
        if (!pen->wait_for_service(std::chrono::seconds(1))) return;
        auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
        request->r = 255;
        request->g = 255;
        request->b = 255;
        request->width = 3;
        request->off = enabled ? 0 : 1; // 0 = draw, 1 = pen up (don't draw)
        pen->async_send_request(request);
    }
    
    void loadWaypoints(const std::string& filepath) {
        std::ifstream file(filepath);
        double x, y;
        while (file >> x >> y) waypoints.push_back({x, y});
        file.close();
        if (!waypoints.empty()) {
            if (waypoints[0].first < 0.0) {
                setPen(false);      
                activatePen = true; 
                waypointIdx = 1;    
            } else {
                waypointIdx = 0;
            }
            if(waypointIdx<waypoints.size()) {
                goalX = waypoints[waypointIdx].first;
                goalY = waypoints[waypointIdx].second;
            }
        }
    }

    void posCallback(turtlesim::msg::Pose::SharedPtr msg) {
        
        double distX = goalX - msg->x, distY = goalY - msg->y;
        double dist = sqrt(distX*distX + distY*distY);
        geometry_msgs::msg::Twist message;
        if(dist > 0.1) {
            double angle = std::atan2(distY, distX) - msg->theta;
            angle = std::atan2(std::sin(angle), std::cos(angle));
            message.angular.z = 4.5*angle;
            if(fabs(angle) < 0.4) {
                message.linear.x = 3.0*dist;
            }
            pub->publish(message);
        }
        else {
            if(activatePen) {
                setPen(1);
                activatePen = 0;
            }
            waypointIdx++;

            if(waypointIdx < waypoints.size() && waypoints[waypointIdx].first < 0.0) {
                setPen(0); 
                waypointIdx++;
                activatePen = 1;
            }
            if(waypointIdx < waypoints.size()) {
                goalX = waypoints[waypointIdx].first;
                goalY = waypoints[waypointIdx].second;
            }
            else {
                geometry_msgs::msg::Twist stop;
                pub->publish(stop);
                rclcpp::shutdown();
            }
        } 
        
    }

    // void timerCallback() {

    // }

public:
    Draw() : Node("drawer") {
        pub = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        sub = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&Draw::posCallback, this, std::placeholders::_1));
        pen = this->create_client<turtlesim::srv::SetPen>("/turtle1/set_pen");
        loadWaypoints("/home/jekthw/ros2_ws/src/turtledraw/waypoints.txt");
        // timer = this->create_wall_timer(100ms, std::bind(&Draw::timerCallback, this));
    }
    


};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Draw>());
  rclcpp::shutdown();
  return 0;
}
