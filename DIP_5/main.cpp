//============================================================================
// Name        : main.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : only calls processing and test routines
//============================================================================

#include <iostream>

#include "Dip5.h"

using namespace std;

// usage: path to image in argv[1], sigma in argv[2]
// main function. loads image, calls processing routines, shows keypoints
int main(int argc, char** argv) {

   // check if enough arguments are defined
   if (argc < 2){
      cout << "Usage:\n\tdip5 path_to_original [sigma]"  << endl;
      cout << "Press enter to exit"  << endl;
      cin.get();
      return -1;
   }
   
   // load image, path in argv[1]
   cout << "load image" << endl;
   Mat img = imread(argv[1], 0);
   if (!img.data){
      cout << "ERROR: original image not specified"  << endl;
      cout << "Press enter to exit"  << endl;
      cin.get();
      return -1;
   }
   // convert U8 to 32F
   img.convertTo(img, CV_32FC1);
   cout << " > done" << endl;

   // define standard deviation of directional gradient
   double sigma;
   if (argc < 3)
      sigma = 0.5;
   else
      sigma = atof(argv[2]);

   // construct processing object
   Dip5 dip5(sigma);
   
   // show and safe gray-scale version of original image
   dip5.showImage( img, "original.png", 0, true, true);

   // calculate interest points
   vector<KeyPoint> points;
   dip5.run(img, points);
    
   cout << "Number of detected interest points:\t" << points.size() << endl;
   
   // plot result
   img = imread(argv[1]);
   drawKeypoints(img, points, img, Scalar(0,0,255), DrawMatchesFlags::DRAW_OVER_OUTIMG + DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
   dip5.showImage(img, "keypoints", 0, true, true);
   
   return 0;
} 
