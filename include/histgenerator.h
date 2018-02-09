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
    //ctor
    HistGenerator(const std::vector<int>& channels, const std::vector<int>& binsizes,
		  const std::vector<float>& ranges);
    //dtor    
    ~HistGenerator();

    //generate or regenerate histogram
    void gen(const cv::Mat& img, cv::Mat& hist);
    
    //acculumute histogram
    void accumulate(const cv::Mat& img, cv::Mat& hist);

    //Generate a histogram map, only capable of 2d histogram
    void histMap(const cv::Mat& hist, cv::Mat& map, const cv::Size& eleSize);

    //Obtain back project. Only capable of 2d histogram
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
