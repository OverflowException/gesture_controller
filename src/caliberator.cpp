#include "opencv2/opencv.hpp"
#include "histgenerator.h"
#include "sampler.h"
#include "platformcam.h"
#include <vector>
#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
  if(argc != 2)
    {
      std::cerr << "Usage: "  << argv[0] << " [histogram file name]" << std::endl;
      return 0;
    }

  //Initialize histogram generator
  std::vector<int> channels = {0, 1};
  std::vector<int> binsizes = {30, 32};
  std::vector<float> ranges = {180, 256};
  ctrler::HistGenerator hg(channels, binsizes, ranges);

  //Initialize image sampler
  cv::Rect2i rstruct(200, 200, 3, 4);
  cv::Size szcell(10, 10);
  cv::Size szgap(10, 10);
  ctrler::ImgSampler sampler(rstruct, szcell, szgap);
  
  //imread default BGR! NOT RGB! FUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUCK!
  cv::Mat img_bgr, img_hsv, img_sub_bgr, img_sub_hsv;
  cv:: Mat hist, hist_norm, hist_accu, hist_map, backproj;
  bool append_mode = false;
  
  cam_t s_vid;
  char key_pressed;
  s_vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  s_vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  //open camera to read real time image
  if(!OPEN_CAM(s_vid))
    {
      std::cerr << "Cannot open camera!" << std::endl;
      return 0;
    }

  do
    {
      for(;;)
	{
	  CAM_CAP_FRAME(s_vid, img_bgr);
	  sampler.sample(img_bgr, img_sub_bgr);
	  sampler.drawSampleROI(img_bgr, cv::Scalar_<uchar>(255, 0, 255), 2);

	  cv::imshow("Sample", img_sub_bgr);
	  cv::imshow("[s] to sample, [q] to quit", img_bgr);

	  key_pressed = cv::waitKey(1);
	  if(key_pressed == 'q' || key_pressed == 's')
	    break;
	}
      cv::destroyAllWindows();
      
      //quit
      if(key_pressed == 'q')
	break;

      //Generate histogram
      cv::cvtColor(img_sub_bgr, img_sub_hsv, cv::COLOR_BGR2HSV);

      //If this loop is working under append mode.
      if(append_mode)
	hg.accumulate(img_sub_hsv, hist);
      else
	hg.gen(img_sub_hsv, hist);
      
      cv::normalize(hist, hist_norm, 255, 0, cv::NORM_MINMAX, CV_8U);
      hg.histMap(hist_norm, hist_map, cv::Size(10, 10));
      cv::imshow("Normalized histogram", hist_map);
      
      for(;;)
	{
	  CAM_CAP_FRAME(s_vid, img_bgr);
	  cv::cvtColor(img_bgr, img_hsv, cv::COLOR_BGR2HSV);
	  hg.backProject(img_hsv, hist_norm, backproj);
	  cv::threshold(backproj, backproj, 0, 255, cv::THRESH_OTSU);
	  cv::imshow("[c] to confirm, [r] to resample, [a] to append, [q] to quit", backproj);
	  
	  
	  key_pressed = cv::waitKey(1);
	  if(key_pressed == 'c' || key_pressed == 'r' || key_pressed == 'a' || key_pressed == 'q')
	    break;
	}
      cv::destroyAllWindows();

      switch(key_pressed)
	{
	case 'a': append_mode = true; break;
	case 'r': append_mode = false; break;
	default: break;
	}
      
      if(key_pressed == 'c' || key_pressed == 'q')
	break;


    }while(true);
  
  s_vid.release();
  cv::destroyAllWindows();

  //If [q] pressed, quit immediately. do not save anything.
  if(key_pressed == 'q')
    return 0;
  
  std::ofstream ofs;
  ofs.open(argv[1]);
  if(!ofs.is_open())
    {
      std::cerr << "Unable to open " << argv[1] << std::endl;
      return 0;
    }

  std::cout << hist << std::endl;
  
  ofs << "#hist_data" << std::endl;
  ofs << channels[0] << "\t" << channels[1] << "\t"
      << binsizes[0] << "\t" << binsizes[1] << "\t"
      << ranges[0] << "\t" << ranges[1] << std::endl;
  for(cv::MatIterator_<float> hist_it = hist.begin<float>();
      hist_it != hist.end<float>(); ++hist_it)
    ofs << *hist_it << "\t";
  ofs << std::endl;
  
  ofs.close();
}
