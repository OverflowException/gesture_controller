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
    /**
       Constructor
     */
    GestAnalyzer(){};
    
    /**
       Destructor
     */
    ~GestAnalyzer(){};

    /**
       Analyze gesture with k-means clustering algorithm.
       @params
       backproj: input back projection
       ccenters: output cluster centers
       @return
       Gesture code.
       left-right in image is the mirror of actual left-right
       -1: less than 3 clusters
       0: both hands up
       1: both hands down
       2: in image, left up, right down.
       3: in image, left down, right up
     */
    //Binary back projection image as input
    int analyze(const cv::Mat& backproj, std::vector<cv::Point>& ccenters);

    /**
       Get gesture name
       @params
       gcode: gesture code
       @return
       gesture name
     */
    inline static std::string gestureName(int gcode)
    { if(gcode < 0 || gcode > 3) return ""; return _gesture_names[gcode]; };
    
  private:

    /**
       Calculate euclidian distance of 2 points
     */
    template<typename T>
      inline double distance(const T& p1, const T& p2)
      {
	return sqrt(pow(double(p1.x - p2.x), 2) + pow(double(p1.y - p2.y), 2));
      }

    /**
       Determine gesture with 3 cluster centers
       @params
       gpoints: positions of cluster centers
       @return
       gesture code
     */
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
