#include "gestanalyzer.h"
#include <algorithm>

namespace ctrler
{
  int GestAnalyzer::analyze(const cv::Mat& backproj, std::vector<cv::Point>& ccenters)
  {
    assert(backproj.type() == CV_8UC1);
    
    //Find coordinates of non zero points
    cv::findNonZero(backproj, _nonzero_pos_i);
    _nonzero_pos_f.resize(_nonzero_pos_i.size());
    //Convert coordinates from Point2i to Point2f
    for(size_t index = 0; index < _nonzero_pos_i.size(); ++index)
      _nonzero_pos_f[index] = cv::Point2f(float(_nonzero_pos_i[index].x),
					  float(_nonzero_pos_i[index].y));

    //kmeans, find 3 cluster centers
    cv::kmeans(_nonzero_pos_f, 3, _points_labels,
	       cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.1),
	       3, cv::KMEANS_PP_CENTERS, _cluster_center_coords);


    //Extract center points. from matrix to vector
    ccenters.resize(_cluster_center_coords.rows);
    for(size_t center_index = 0; center_index < ccenters.size(); ++center_index)
      ccenters[center_index] = 
	cv::Point(int(_cluster_center_coords.at<float>(center_index, 0)),
		  int(_cluster_center_coords.at<float>(center_index, 1)));

    return this->_determine_gesture(ccenters);
  }

  int GestAnalyzer::_determine_gesture(std::vector<cv::Point>& gpoints)
  {
    if(gpoints.size() != 3)
      return -1;
  
    //Sort. Ensure gesture points are in a let-to-right order
    std::sort(gpoints.begin(), gpoints.end(), [](cv::Point& a, cv::Point& b)
	      {return  a.x < b.x;});


    //left, right means the left, right in the image
    //Be noted that left, right in image is mirror of actual left, right
    int x_diff_l = gpoints[1].x - gpoints[0].x;
    int x_diff_r = gpoints[1].x - gpoints[2].x;
  
    if(x_diff_r == 0 || x_diff_l == 0)
      return -2;

    //Current gesture is represented by this cv::Point. left slope as x, right slope as y
    cv::Point2f slope_point(float(gpoints[1].y - gpoints[0].y) / x_diff_l,
			    float(gpoints[1].y - gpoints[2].y) / x_diff_r);

    //Determine which gesture reference can describe current gensture most accurately
    size_t min_index, refpoint_index = 0;
    double curr_dist, min_dist = std::numeric_limits<double>::max();
    for(refpoint_index = 0; refpoint_index < _gesture_refpoints.size(); ++refpoint_index)
      if((curr_dist = distance(slope_point, _gesture_refpoints[refpoint_index])) < min_dist)
	{
	  min_dist = curr_dist;
	  min_index = refpoint_index;
	}

    return int(min_index);
  }
  
  std::array<cv::Point2f, 4> GestAnalyzer::_gesture_refpoints =
    {
      cv::Point2f(0, 0),
      cv::Point2f(-1, 1),
      cv::Point2f(0, 1),
      cv::Point2f(-1, 0)
    };

  std::vector<std::string> GestAnalyzer::_gesture_names =
    {
      "UP UP",
      "DOWN DOWN",
      "UP DOWN",
      "DOWN UP"
    };
}
