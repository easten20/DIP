//============================================================================
// Name        : Dip4.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : 
//============================================================================

#include "Dip4.h"

// Performes a circular shift in (dx,dy) direction
/*
in       :  input matrix
dx       :  shift in x-direction
dy       :  shift in y-direction
return   :  circular shifted matrix
*/
Mat Dip4::circShift(Mat& in, int dx, int dy){

	Mat tmp = Mat::zeros(in.rows, in.cols, in.type());
	int x, y, new_x, new_y;

	for (y = 0; y<in.rows; y++){
		// calulate new y-coordinate
		new_y = y + dy;
		if (new_y<0)
			new_y = new_y + in.rows;
		if (new_y >= in.rows)
			new_y = new_y - in.rows;

		for (x = 0; x<in.cols; x++){

			// calculate new x-coordinate
			new_x = x + dx;
			if (new_x<0)
				new_x = new_x + in.cols;
			if (new_x >= in.cols)
				new_x = new_x - in.cols;

			tmp.at<float>(new_y, new_x) = in.at<float>(y, x);

		}
	}

	return tmp;
}

// Function applies inverse filter to restorate a degraded image
/*
degraded :  degraded input image
filter   :  filter which caused degradation
return   :  restorated output image
*/
Mat Dip4::inverseFilter(Mat& degraded, Mat& filter){

   // TODO !!!
   
   return degraded;
}

// Function applies wiener filter to restorate a degraded image
/*
degraded :  degraded input image
filter   :  filter which caused degradation
snr      :  signal to noise ratio of the input image
return   :   restorated output image
*/
Mat Dip4::wienerFilter(Mat& degraded, Mat& filter, double snr){
   
   
   // TODO !!!
   Mat freq_deg, freq_kernel, result;

  
    //copy kernel to large matrix (the size of input image)
    cv::Rect roi(cv::Point(0,0),filter.size());
    Mat destinationROI = Mat::zeros(degraded.size(), degraded.type());
    filter.copyTo(destinationROI(roi));

    //perform circ shift on kernel 
    Mat circ_kernel = circShift(destinationROI, -filter.cols/2, -filter.rows/2);
   
    //convert to frequency domains
    dft(degraded, freq_deg, DFT_COMPLEX_OUTPUT);
    dft(circ_kernel, freq_kernel, DFT_COMPLEX_OUTPUT);

    //split kernel into real and imaginary part
    Mat filter_planes[2];
    split(freq_kernel, filter_planes);   

   //|P|²
    Mat mag_filter;
    magnitude(filter_planes[0], filter_planes[1], mag_filter);
    multiply(mag_filter, mag_filter, mag_filter);
   // 

   //|P|² + 1/SNR²
    Mat denominator = mag_filter + 1 / (snr * snr);
    //Mat denominator = mag_filter;
   //

   //P*
   for (int x = 0; x < freq_kernel.cols; x++) {
   	for (int y = 0; y < freq_kernel.rows; y++) {		
		//imaginary part
		filter_planes[1].at<float>(y,x) = -1*filter_planes[1].at<float>(y,x);
	}
   }
   
   //Q = P* / ( |P|² + 1/SNR² )
     divide(filter_planes[0], denominator, filter_planes[0]);
     divide(filter_planes[1], denominator, filter_planes[1]);
     merge(filter_planes, 2, freq_kernel);
   // 
   
   
   //multiplication in freq domains
   mulSpectrums(freq_deg, freq_kernel, freq_deg,0);
   
   
    //convert to spatial domain
    dft(freq_deg, result, DFT_INVERSE | DFT_REAL_OUTPUT + DFT_SCALE);
    
    //cout << result << endl;
    threshold(result, result,255, 0, THRESH_TRUNC);
 
   return result;

}

/* *****************************
  GIVEN FUNCTIONS
***************************** */

// function calls processing function
/*
in                   :  input image
restorationType     :  integer defining which restoration function is used
kernel               :  kernel used during restoration
snr                  :  signal-to-noise ratio (only used by wieder filter)
return               :  restorated image
*/
Mat Dip4::run(Mat& in, string restorationType, Mat& kernel, double snr){

   if (restorationType.compare("wiener")==0){
      return wienerFilter(in, kernel, snr);
   }else{
      return inverseFilter(in, kernel);
   }

}

// Function degrades a given image with gaussian blur and additive gaussian noise
/*
img         :  input image
degradedImg :  degraded output image
filterDev   :  standard deviation of kernel for gaussian blur
snr         :  signal to noise ratio for additive gaussian noise
return      :  the used gaussian kernel
*/
Mat Dip4::degradeImage(Mat& img, Mat& degradedImg, double filterDev, double snr){

    int kSize = round(filterDev*3)*2 - 1;
   
    Mat gaussKernel = getGaussianKernel(kSize, filterDev, CV_32FC1);
    gaussKernel = gaussKernel * gaussKernel.t();
    filter2D(img, degradedImg, -1, gaussKernel);

    Mat mean, stddev;
    meanStdDev(img, mean, stddev);

    Mat noise = Mat::zeros(img.rows, img.cols, CV_32FC1);
    randn(noise, 0, stddev.at<double>(0)/snr);
    degradedImg = degradedImg + noise;
    threshold(degradedImg, degradedImg, 255, 255, CV_THRESH_TRUNC);
    threshold(degradedImg, degradedImg, 0, 0, CV_THRESH_TOZERO);

    return gaussKernel;
}

// Function displays image (after proper normalization)
/*
win   :  Window name
img   :  Image that shall be displayed
cut   :  whether to cut or scale values outside of [0,255] range
*/
void Dip4::showImage(const char* win, Mat img, bool cut){

   Mat tmp = img.clone();

   if (tmp.channels() == 1){
      if (cut){
         threshold(tmp, tmp, 255, 255, CV_THRESH_TRUNC);
         threshold(tmp, tmp, 0, 0, CV_THRESH_TOZERO);
      }else
         normalize(tmp, tmp, 0, 255, CV_MINMAX);
         
      tmp.convertTo(tmp, CV_8UC1);
   }else{
      tmp.convertTo(tmp, CV_8UC3);
   }
   imshow(win, tmp);
}

// function calls some basic testing routines to test individual functions for correctness
void Dip4::test(void){

   test_circShift();
   cout << "Press enter to continue"  << endl;
   cin.get();

}

void Dip4::test_circShift(void){
   
   Mat in = Mat::zeros(3,3,CV_32FC1);
   in.at<float>(0,0) = 1;
   in.at<float>(0,1) = 2;
   in.at<float>(1,0) = 3;
   in.at<float>(1,1) = 4;
   Mat ref = Mat::zeros(3,3,CV_32FC1);
   ref.at<float>(0,0) = 4;
   ref.at<float>(0,2) = 3;
   ref.at<float>(2,0) = 2;
   ref.at<float>(2,2) = 1;
   
   if (sum((circShift(in, -1, -1) == ref)).val[0]/255 != 9){
      cout << "ERROR: Dip4::circShift(): Result of circshift seems to be wrong!" << endl;
      return;
   }
   cout << "Message: Dip4::circShift() seems to be correct" << endl;
}
