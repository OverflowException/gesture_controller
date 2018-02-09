#include "histgenerator.h"
#include <algorithm>

namespace ctrler
{
  HistGenerator::HistGenerator(const std::vector<int>& channels, const std::vector<int>& binsizes,
			       const std::vector<float>& ranges)
  {
    //see if channels, binsizes and ranges are of the same size
    assert(ranges.size() == binsizes.size() && ranges.size() == channels.size());

    _histdims = channels.size();

    //Initialize _channels array
    this->_channels = new int[_histdims];
    std::copy(channels.begin(), channels.end(), _channels);
    
    //Initialize _binsize array
    this->_binsizes = new int[_histdims];
    std::copy(binsizes.begin(), binsizes.end(), _binsizes);
    
    //Initialize _ranges array
    this->_ranges = new const float*[_histdims];
    for(int index = 0; index < _histdims; ++index)
      {
	_ranges_temp.emplace_back(new float[2]);
	_ranges_temp[index][0] = 0;
	_ranges_temp[index][1] = ranges[index];
	_ranges[index] = _ranges_temp[index];
      }
  }

  HistGenerator::~HistGenerator()
  {
    delete[] _channels;
    delete[] _ranges;
    delete[] _binsizes;
  }

  void HistGenerator::gen(const cv::Mat& img, cv::Mat& hist)
  {
    //see if image has enough channels to calculate histogram
    assert(img.channels() >= this->_histdims);

    cv::calcHist(&img, 1, _channels, cv::Mat(),
     		 hist, _histdims, _binsizes, _ranges,
     		 true, false);
  }


  
  void HistGenerator::accumulate(const cv::Mat& img, cv::Mat& hist)
  {
    //See if histogram is canonical
    assert(hist.type() == CV_32FC1);
    
    cv::Mat temp_hist;
    gen(img, temp_hist);
    hist += temp_hist;
  }

  
  void HistGenerator::backProject(const cv::Mat& img, const cv::Mat& hist,
		   cv::Mat& backproj)
  {
    assert(hist.type() == CV_8UC1);
      
    backproj = cv::Mat(img.size(), hist.type());
    int row_index, col_index;
    int binspan1 = _ranges[0][1] / _binsizes[0];
    int binspan2 = _ranges[1][1] / _binsizes[1];
    for(row_index = 0; row_index < img.rows; ++row_index)
      for(col_index = 0; col_index < img.cols; ++col_index)
	{
	  backproj.at<uchar>(row_index, col_index) =
	    hist.at<uchar>( img.at<cv::Vec3b>(row_index, col_index)[_channels[0]] / binspan1,
			    img.at<cv::Vec3b>(row_index, col_index)[_channels[1]] / binspan2);
	}
  }

  
  void HistGenerator::histMap(const cv::Mat& hist, cv::Mat& map, const cv::Size& eleSize)
  {
    assert(hist.channels() == 1 && hist.dims < 3);
    
    cv::Mat hist_norm;
    cv::normalize(hist, hist_norm, 255, 0, cv::NORM_MINMAX, CV_8U);
    cv::Size hist_size = hist_norm.size();
    map = cv::Mat(hist_size.height * eleSize.height,
		  hist_size.width * eleSize.width,
		  hist_norm.type());
  
    int dim1_index, dim2_index;
    //Traverse histogram element
    cv::Point left_up, right_down;
    for(dim1_index = 0; dim1_index < hist_size.width; ++dim1_index)
      for(dim2_index = 0; dim2_index < hist_size.height; ++dim2_index)
	{
	  left_up = {dim1_index * eleSize.width, dim2_index * eleSize.height};
	  right_down = {left_up.x + eleSize.width - 1, left_up.y + eleSize.height - 1};
	  cv::rectangle(map, left_up, right_down,
			cv::Scalar_<uchar>::all(hist_norm.at<uchar>(dim2_index, dim1_index)),
			cv::FILLED);
	}
  }
  
}
