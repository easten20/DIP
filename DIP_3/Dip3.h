//============================================================================
// Name        : Dip3.h
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : header file for third DIP assignment
//============================================================================

#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Dip3{

   public:
      // constructor
      Dip3(void){};
      // destructor
      ~Dip3(void){};
        
      // processing routines
      // start unsharp masking
      Mat run(Mat& in, int smoothType, int size, double thresh, double scale);
      // testing routine
      void test(void);

   private:
      // function headers of functions to be implemented
      // --> edit ONLY these functions!
      Mat createGaussianKernel(int kSize);
      Mat circShift(Mat& in, int dx, int dy);
      Mat frequencyConvolution(Mat& in, Mat& kernel);
      Mat usm(Mat& in, int smoothType, int size, double thresh, double scale);
      // function headers of functions to implemented in previous exercises
      // --> re-use your (corrected) code
      Mat spatialConvolution(Mat&, Mat&);
    
      // function headers of given functions
      // Performes smoothing operation by convolution
      Mat mySmooth(Mat& in, int size, bool spatial);
      
      void test_createGaussianKernel(void);
      void test_circShift(void);
      void test_frequencyConvolution(void);
};
