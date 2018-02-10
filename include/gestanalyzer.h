#ifndef _GESTANALYZER_H
#define _GESTANALYZER_H

#include "opencv2/opencv.hpp"
#include <vector>
#include <array>
#include <string>

namespace ctrler
{
  class GestAnalyzer
  {
  public:
    GestAnalyzer(){};
    ~GestAnalyzer(){};
    
    //Binary back projection image as input
    int analyze(const cv::Mat& backproj, std::vector<cv::Point>& ccenters);
    inline static std::string gestureName(int gcode)
    { if(gcode < 0 || gcode > 3) return ""; return _gesture_names[gcode]; };
    
  private:
    
    template<typename T>
      inline double distance(const T& p1, const T& p2)
      {
	return sqrt(pow(double(p1.x - p2.x), 2) + pow(double(p1.y - p2.y), 2));
      }

    int _determine_gesture(std::vector<cv::Point>& gpoints);
    
    std::vector<cv::Point> _nonzero_pos_i;
    std::vector<cv::Point2f> _nonzero_pos_f;
    std::vector<int> _points_labels;
    cv::Mat _cluster_center_coords;
    
    static std::array<cv::Point2f, 4> _gesture_refpoints;
    static std::vector<std::string> _gesture_names;
  };
}

#endif
