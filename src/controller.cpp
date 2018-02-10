#include <iostream>
#include <fstream>
#include <string>
#include "histgenerator.h"
#include "platformcam.h"
#include "gestanalyzer.h"

int main(int argc, char** argv)
{
  if(argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " [histogram file name]" << std::endl;
      return 0;
    }

  //Read in historgram data from file
  std::ifstream ifs;
  ifs.open(argv[1]);
  if(!ifs.is_open())
    {
      std::cerr << "Cannot open file " << argv[1] << "!" << std::endl;
      return 0;
    }

  std::string line_buf;
  getline(ifs, line_buf);
  if(line_buf != "#hist_data")
    {
      std::cerr << "File " << argv[1] << " header mismatch!" << std::endl;
      return 0;
    }

  
  std::vector<int> channels(2, 0), binsizes(2, 0);
  std::vector<float> ranges(2, 0);
  float value_buf = 0;
    
  //Read histogram info
  ifs >> channels[0]; ifs >> channels[1];
  ifs >> binsizes[0]; ifs >> binsizes[1];
  ifs >> ranges[0]; ifs >> ranges[1];
  cv::Mat hist_ref(binsizes[0], binsizes[1], CV_32FC1);
  //Read histogram data
  for(cv::MatIterator_<float> hist_it = hist_ref.begin<float>();
      hist_it != hist_ref.end<float>(); ++hist_it)
    {
      ifs >> value_buf;
      *hist_it = value_buf;
    }
  
  //Normalize histogram
  cv::Mat hist_ref_norm;
  cv::normalize(hist_ref, hist_ref_norm, 255, 0, cv::NORM_MINMAX, CV_8U);

  //Initialize histogram generator
  ctrler::HistGenerator hg(channels, binsizes, ranges);
  //Initialize gesture analyzer
  ctrler::GestAnalyzer ga;
  
  //Declare all kinds of matrices
  cv::Mat img_bgr, img_hsv, backproj;
  //Open camera
  cam_t s_vid;
  s_vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  s_vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  //open camera
  if(!OPEN_CAM(s_vid))
    {
      std::cerr << "Cannot open camera!" << std::endl;
      return 0;
    }

  int prev_gcode = -1, curr_gcode = -1;
  std::string gesture_name;
  std::vector<cv::Point> cluster_centers;
  //Read time images processing
  for(;;)
    {
      CAM_CAP_FRAME(s_vid, img_bgr);
      cv::cvtColor(img_bgr, img_hsv, cv::COLOR_BGR2HSV);
      hg.backProject(img_hsv, hist_ref_norm, backproj);

      //Morphology open. Denoise.
      cv::morphologyEx(backproj, backproj, cv::MORPH_CLOSE,
      		       cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

      //Binarize back projection, auto thresholding
      cv::threshold(backproj, backproj, 0, 255, cv::THRESH_OTSU);


      curr_gcode = ga.analyze(backproj, cluster_centers);
      gesture_name = ctrler::GestAnalyzer::gestureName(curr_gcode);
      
      if(curr_gcode < 0)
	std::cout << "Invalid gesture! Error code = "  << curr_gcode << std::endl;
      
      for(cv::Point center : cluster_centers)
	cv::circle(img_bgr, center, 10, cv::Scalar_<uchar>(255, 255, 0), 3, cv::FILLED);
      
      cv::putText(img_bgr, gesture_name, cv::Point(100, 100),
		  cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar_<uchar>(0, 255, 0),
		  5, cv::FILLED);

      cv::imshow("Back projection", backproj);
      cv::imshow("Capture result", img_bgr);
      if(cv::waitKey(1) == 'q')
	break;
    }

  
  s_vid.release();
  cv::destroyAllWindows();

  return 0;
}
