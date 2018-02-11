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
    /**
       Constructor. Construct an array of rectangle sample ROIs.
       @params
       rstruct: sampler structure, 
                upper-left point of this Rect is the upper-left point of the upper-left most ROI
		width of this Rect indicates how many ROIs per row in this sampler
		height of this Rect indicates how many ROIs per column in this sampler
       szcell: size of a single ROI
       szgap: size of gap between individual ROIs.
              width of this Size is the value of horizontal gap
	      height of this Size is the value of vertical gap
     */
    ImgSampler(const cv::Rect2i& rstruct, const cv::Size& szcell,
	    const cv::Size& szgap);
    /**
       Destructor
     */
    ~ImgSampler(){};

    /**
       Take sample. Invoke this method before drawing ROI. Or sample will be contanimated by ROI.
       @params
       img: input image
       sampleimge: output sample image by stitching small subimages in individual ROIs.
       @return
    */
    void sample(const cv::Mat& img, cv::Mat& sampleimg);

    /**
       Draw sample ROIs on the image.
       @params
       img: input image
       color: color of ROI frames
       thickness: thickness of ROI frames
       lineType: line type of ROI frames
       @return
     */
    void drawSampleROI(cv::Mat& img, const cv::Scalar& color,
		       int thickness = -1, int lineType = cv::FILLED);
  
  private:
    std::vector<cv::Rect> _sample_rois;
    int _cell_per_row;
    int _cell_per_col;
};

}

#endif
