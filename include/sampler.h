#ifndef _SAMPLER_H
#define _SAMPLER_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

namespace ctrler
{
  class ImgSampler
  {
  public:
    ImgSampler(const cv::Rect2i& rstruct, const cv::Size& szcell,
	    const cv::Size& szgap);
    ~ImgSampler(){};

    //Take sample. Invoke this method before drawing ROI. Or sample will be contanimated by ROI
    void sample(const cv::Mat& img, cv::Mat& wholeroi);

    //Draw ROI on the image
    void drawSampleROI(cv::Mat& img, const cv::Scalar& color,
		       int thickness = -1, int lineType = cv::FILLED);
  
  private:
    std::vector<cv::Rect> _sample_rois;
    int _cell_per_row;
    int _cell_per_col;
};

}

#endif
