//============================================================================
// Name        : Dip5.h
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : header file for fifth DIP assignment
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Dip5{

   public:
      // constructor
      Dip5(void){};
      Dip5(double s){this->sigma = s;};
      // destructor
      ~Dip5(void){};
        
      // processing routines
      // start keypoint detection
      void run(Mat& in, vector<KeyPoint>& points);
      // function headers of given functions
      void showImage(Mat& img, const char* win, int wait, bool show, bool save);

   private:
      // function headers of functions to be implemented
      // --> edit ONLY these functions!
      Mat createFstDevKernel(double sigma);
	  void getInterestPoints(Mat& img, double sigma, vector<KeyPoint>& points);
	  
	  // function headers of given functions
	  Mat nonMaxSuppression(Mat& img);
	  
      double sigma;
};
