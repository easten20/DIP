//============================================================================
// Name        : Dip2.h
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : header file for second DIP assignment
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Dip2{

   public:
      // constructor
      Dip2(void){};
      // destructor
      ~Dip2(void){};
		
      // processing routines
      // to create noise images
      void generateNoisyImages(string);
      // for noise suppression
      void run(void);
      // testing routine
      void test(void);

   private:
      // function headers of functions to be implemented
      // --> edit ONLY these functions!
      // performs spatial convolution of image and filter kernel
      Mat spatialConvolution(Mat&, Mat&);
      // moving average filter (aka box filter)
      Mat averageFilter(Mat& src, int kSize);
      // median filter
      Mat medianFilter(Mat& src, int kSize);
      // adaptive average filter
      Mat adaptiveFilter(Mat& src, int kSize, double threshold);
      // bilateral filer
      Mat bilateralFilter(Mat& src, int kSize, double sigma);

      // function headers of given functions
      // performs noise reduction
      Mat noiseReduction(Mat&, string, int, double=0);

      // test functions
      void test_spatialConvolution(void);
      void test_averageFilter(void);
      void test_medianFilter(void);
      void test_adaptiveFilter(void);
};
