#ifndef _HISTGENERATOR_H
#define _HISTGENERATOR_H

#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>

namespace ctrler
{
  class HistGenerator
  {
  public:
    /**
       Constructor
       @params
       channels: indices of channels used to calculate histogram
       binsizes: number fo bins for each channel
       ranges: value ranges of each channel
     */
    HistGenerator(const std::vector<int>& channels, const std::vector<int>& binsizes,
		  const std::vector<float>& ranges);
    /**
       Destructor
    */
    ~HistGenerator();

    /**
       Generate histogram
       @params
       img: input image
       hist: output histogram
       @return
     */
    void gen(const cv::Mat& img, cv::Mat& hist);

    /**
       Accumulate histogram
       @params
       img: input image
       hist: add the histogram of input image to hist, which must be of CV_32FC1 type
       @return
     */
    void accumulate(const cv::Mat& img, cv::Mat& hist);

    /**
       Generate a visualized histogram map. Only capable of 2D histogram.
       @params
       hist: input histogram
       map: output 2D histogram type. CV_8UC1 type
       elesize: size of a single element on histogram map
     */
    void histMap(const cv::Mat& hist, cv::Mat& map, const cv::Size& eleSize);

    /**
       Generate back projection of a image according to histogram
       @params
       img: input image
       hist: reference histogram
       backproj: output backproject
       @return
     */
    void backProject(const cv::Mat& img, const cv::Mat& hist,
		     cv::Mat& backproj);

  private:
    int _histdims;
    int* _channels;
    int* _binsizes;

    std::vector<float*> _ranges_temp;
    const float** _ranges;

  };
}



#endif
