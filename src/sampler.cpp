#include "sampler.h"


namespace ctrler
{
  ImgSampler::ImgSampler(const cv::Rect2i& rstruct, const cv::Size& szcell,
		   const cv::Size& szgap)
  {
    cv::Point origin = rstruct.tl();
    this->_cell_per_col = rstruct.height;
    this->_cell_per_row = rstruct.width;

    this->_sample_rois.resize(_cell_per_row * _cell_per_col);
      
    int row_index, col_index;
    for(row_index = 0; row_index < _cell_per_col; ++row_index)
      {
	for(col_index = 0; col_index < _cell_per_row; ++col_index)
	  _sample_rois[row_index * _cell_per_row + col_index] =
	    cv::Rect(cv::Point(origin.x + col_index * (szgap.width + szcell.width),
			       origin.y + row_index * (szgap.height + szcell.height)),
		     szcell);
      }
  }

  void ImgSampler::sample(const cv::Mat& img, cv::Mat& sampleimg)
  {
    assert(_sample_rois.size() != 0);
    assert(_sample_rois[0].width != 0 && _sample_rois[0].height != 0);

    
    std::vector<cv::Mat> subimgs;
    subimgs.resize(_sample_rois.size());

    int cell_width = _sample_rois[0].width;
    int cell_height = _sample_rois[0].height;
    sampleimg = cv::Mat(cell_height * _cell_per_col, cell_width * _cell_per_row, img.type());
    for(size_t index = 0; index < _sample_rois.size(); ++index)
      {
	//Construct subimage
	subimgs[index] = cv::Mat(img, _sample_rois[index]);
	//Copy this subimage to one big image, sampleimg
	subimgs[index].copyTo(sampleimg(cv::Rect((index % _cell_per_row) * cell_width,
						 (index / _cell_per_row) * cell_height,
						 cell_width, cell_height)));
      }
  }

  void ImgSampler::drawSampleROI(cv::Mat& img, const cv::Scalar& color,
				 int thickness, int lineType)
  {
    for(cv::Rect& roi : _sample_rois)
      cv::rectangle(img, roi, color, thickness, lineType);
  }
}
