#include <iostream>
#include <fstream>
#include <string>
#include "histgenerator.h"
#include "platformcam.h"
#include "gestanalyzer.h"
#include "tcpclient.h"

int main(int argc, char** argv)
{
  if(argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " [connection config file] [histogram file name]"
		<< std::endl;
      return 0;
    }
  
  std::ifstream ifs_config, ifs_hist;
  std::string line_buf;


  std::string str_ip; //ip address, in string form
  int port; //port number
  //Connection configuration
  ifs_config.open(argv[1]);
  if(!ifs_config.is_open())
    {
      std::cerr << "Cannot open file " << argv[1] << "!" << std::endl;
      return 0;
    }

  //Verify connection configuration file header
  getline(ifs_config, line_buf);
  if(line_buf != "#connection_config")
    {
      std::cerr << "File " << argv[1] << " header mismatch!" << std::endl;
      return 0;
    }

  //Read ip address nd port
  getline(ifs_config, str_ip);
  ifs_config >> port;
  ifs_config.close();

  std::cout << "Connecting to " << str_ip << " " << port << std::endl;
  TCPClient sender;
  sender.Setup(str_ip, port);
  std::cout << "Done!" << std::endl;
  
  //Read in historgram data from file
  ifs_hist.open(argv[2]);
  if(!ifs_hist.is_open())
    {
      std::cerr << "Cannot open file " << argv[2] << "!" << std::endl;
      return 0;
    }

  getline(ifs_hist, line_buf);
  if(line_buf != "#hist_data")
    {
      std::cerr << "File " << argv[2] << " header mismatch!" << std::endl;
      return 0;
    }
  
  std::vector<int> channels(2, 0), binsizes(2, 0);
  std::vector<float> ranges(2, 0);
  float value_buf = 0;
    
  //Read histogram info
  ifs_hist >> channels[0]; ifs_hist >> channels[1];
  ifs_hist >> binsizes[0]; ifs_hist >> binsizes[1];
  ifs_hist >> ranges[0]; ifs_hist >> ranges[1];
  cv::Mat hist_ref(binsizes[0], binsizes[1], CV_32FC1);
  //Read histogram data
  for(cv::MatIterator_<float> hist_it = hist_ref.begin<float>();
      hist_it != hist_ref.end<float>(); ++hist_it)
    {
      ifs_hist >> value_buf;
      *hist_it = value_buf;
    }
  ifs_hist.close();
  
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

  int gcode;
  std::string str_gcode;
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


      gcode = ga.analyze(backproj, cluster_centers);
      gesture_name = ctrler::GestAnalyzer::gestureName(gcode);
      
      if(gcode < 0)
	std::cerr << "Invalid gesture! Error code = "  << gcode << std::endl;

      else
	{
	  str_gcode = std::to_string(gcode);
	  sender.Send(str_gcode);
	  for(cv::Point center : cluster_centers)
	    cv::circle(img_bgr, center, 10, cv::Scalar_<uchar>(255, 255, 0), 3, cv::FILLED);
      
	  cv::putText(img_bgr, gesture_name, cv::Point(100, 100),
		      cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar_<uchar>(0, 255, 0),
		      5, cv::FILLED);

	}
     
      cv::imshow("Back projection", backproj);
      cv::imshow("Capture result", img_bgr);
      if(cv::waitKey(1) == 'q')
	break;
    }

  sender.Exit();
  s_vid.release();
  cv::destroyAllWindows();

  return 0;
}
