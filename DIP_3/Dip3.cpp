//============================================================================
// Name    : Dip3.cpp
// Author   : Ronny Haensch
// Version    : 2.0
// Copyright   : -
// Description : 
//============================================================================

#include "Dip3.h"

// Generates a gaussian filter kernel of given size
/*
kSize:     kernel size (used to calculate standard deviation)
return:    the generated filter kernel
*/
Mat Dip3::createGaussianKernel(int kSize){

	Mat GaussianK(kSize, kSize, CV_32FC1);

	//we can adjust below
	float sigmaX = kSize / 5.;
	float sigmaY = kSize / 5.;
	float meanX = .0;
	float meanY = .0;
	float sum = .0;

	//cout << "kernel Size:" << kSize << endl;

	for (int x = -kSize / 2; x < kSize / 2 + 1; x++){
		for (int y = -kSize / 2; y < kSize / 2 + 1; y++){
			GaussianK.at<float>(x + kSize / 2, y + kSize / 2) = 1. / (2. * PI*sigmaX*sigmaY)*exp(-1. / 2. * ((x - meanX)*(x - meanX) / sigmaX*sigmaX + (y - meanY)*(y - meanY) / sigmaY*sigmaY));

			sum += GaussianK.at<float>(x + kSize / 2, y + kSize / 2);
		}
	}

	//Normalization
	for (int x = -kSize / 2; x < kSize / 2+1; x++){
		for (int y = -kSize / 2; y < kSize / 2+1; y++){
			GaussianK.at<float>(x + kSize / 2, y + kSize / 2) /= sum;
			//cout << GaussianK.at<float>(x + kSize / 2, y + kSize / 2) << " / ";
		}
		//cout << endl;
	}
	
	return GaussianK;
	//return Mat::zeros(kSize, kSize, CV_32FC1);
	
}


// Performes a circular shift in (dx,dy) direction
/*
in       input matrix
dx       shift in x-direction
dy       shift in y-direction
return   circular shifted matrix
*/
Mat Dip3::circShift(Mat& in, int dx, int dy){
	Mat tmp = Mat::zeros(in.rows, in.cols, in.type());
	int x, y, new_x, new_y;
	
	for(y=0; y<in.rows; y++){
	      // calulate new y-coordinate
	      new_y = y + dy;
	      if (new_y<0)
		  new_y = new_y + in.rows;
	      if (new_y>=in.rows)
		  new_y = new_y - in.rows;
	      
	      for(x=0; x<in.cols; x++){

		  // calculate new x-coordinate
		  new_x = x + dx;
		  if (new_x<0)
			new_x = new_x + in.cols;
		  if (new_x>=in.cols)
			new_x = new_x - in.cols;
 
		  tmp.at<float>(new_y, new_x) = in.at<float>(y, x);
		  
	    }
	}
	
   	return tmp;
}

//Performes a convolution by multiplication in frequency domain
/*
in       input image
kernel   filter kernel
return   output image
*/
Mat Dip3::frequencyConvolution(Mat& in, Mat& kernel){
     
    Mat freq_in, freq_kernel, result;
 
    //copy kernel to large matrix (the size of input image)
    cv::Rect roi(cv::Point(0,0),kernel.size());
    Mat destinationROI = Mat::zeros(in.size(), in.type());
    kernel.copyTo(destinationROI(roi));

    //perform circ shift on kernel 
    Mat circ_kernel = circShift(destinationROI, -kernel.cols/2, -kernel.rows/2);

    //convert to frequency domains
    dft(in, freq_in, 0);
    dft(circ_kernel, freq_kernel, 0);

    //multiplication in freq domains
    mulSpectrums(freq_in, freq_kernel, freq_in,0);
    
    //convert to spatial domain
    dft(freq_in, result, DFT_INVERSE + DFT_SCALE);

    return result;
}

// Performs UnSharp Masking to enhance fine image structures
/*
in       input image
type     integer defining how convolution for smoothing operation is done
         0 <==> spatial domain; 1 <==> frequency domain
size     size of used smoothing kernel
thresh   minimal intensity difference to perform operation
scale    scaling of edge enhancement
return   enhanced image
*/
Mat Dip3::usm(Mat& in, int type, int size, double thresh, double scale){

   // some temporary images 
   Mat tmp(in.rows, in.cols, CV_32FC1);
   
   // calculate edge enhancement

   // 1: smooth original image
   //    save result in tmp for subsequent usage
   switch(type){
      case 0:
         tmp = mySmooth(in, size, true);
         break;
      case 1:
         tmp = mySmooth(in, size, false);
         break;
      default:
         GaussianBlur(in, tmp, Size(floor(size/2)*2+1, floor(size/2)*2+1), size/5., size/5.);
   }

   // TO DO !!!
   //2. subtract smoothed image
   Mat subtractedImage = in.clone();
   subtractedImage -= tmp;
   //subtract(in, tmp, subtractedImage);

   //3. Thresholding
   Mat scaledImage = Mat::zeros(in.size(), CV_32FC1);

   threshold(subtractedImage, scaledImage, thresh, 255, THRESH_TOZERO);
   scaledImage *= scale;
   ////for (int i = 0; i < in.rows; i++){
	  //// for (int j = 0; j < in.cols; j++){
		 ////  if (subtractedImage.at<float>(i, j)>thresh){
			////   scaledImage.at<float>(i, j) = scale*subtractedImage.at<float>(i, j);
		 ////  }
	  //// }
   ////}

   Mat enhancedImage = in.clone();
   enhancedImage += scaledImage;
   //add()
   return enhancedImage;
//   return in;

}

// convolution in spatial domain
/*
src:    input image
kernel:  filter kernel
return:  convolution result
*/
Mat Dip3::spatialConvolution(Mat& src, Mat& kernel){
	
	Mat outputImage = src.clone();
	int border = kernel.cols / 2; //assume it is symmetry		
	Mat src_buf(src.rows + border*2, src.cols + border*2, src.depth()); //prepare for matrix with border
	copyMakeBorder(src, src_buf, border, border,border, border, BORDER_REPLICATE); //extend border by replicating rows and cols
	//flip kernel along x and y axis
	flip(kernel, kernel, -1);
	for (int y = border; y < src_buf.rows-border; y++){
		for (int x = border; x < src_buf.cols-border; x++){
			float convolution = 0.;
			for (int j = 0; j < kernel.rows; j++){
				for (int i = 0; i < kernel.cols; i++){
					convolution += src_buf.at<float>(y - (kernel.rows/2) + j, x - (kernel.cols/2) + i)*kernel.at<float>(j, i);
				}
			}
			outputImage.at<float>(y-border, x-border) = convolution;
		}
	}
	return outputImage;
}

/* *****************************
  GIVEN FUNCTIONS
***************************** */

// function calls processing function
/*
in       input image
type     integer defining how convolution for smoothing operation is done
         0 <==> spatial domain; 1 <==> frequency domain
size     size of used smoothing kernel
thresh   minimal intensity difference to perform operation
scale    scaling of edge enhancement
return   enhanced image
*/
Mat Dip3::run(Mat& in, int smoothType, int size, double thresh, double scale){

   return usm(in, smoothType, size, thresh, scale);

}


// Performes smoothing operation by convolution
/*
in       input image
size     size of filter kernel
spatial  true if convolution shall be performed in spatial domain, false otherwise
return   smoothed image
*/
Mat Dip3::mySmooth(Mat& in, int size, bool spatial){

   // create filter kernel
   Mat kernel = createGaussianKernel(size);
 
   // perform convoltion
   if (spatial)
   return spatialConvolution(in, kernel);
   else
   return frequencyConvolution(in, kernel);
   
}

// function calls some basic testing routines to test individual functions for correctness
void Dip3::test(void){

   test_createGaussianKernel();
   test_circShift();
   test_frequencyConvolution();
   cout << "Press enter to continue"  << endl;
   cin.get();

}

void Dip3::test_createGaussianKernel(void){

   Mat k = createGaussianKernel(11);
   
   if ( abs(sum(k).val[0] - 1) > 0.0001){
      cout << "ERROR: Dip3::createGaussianKernel(): Sum of all kernel elements is not one!" << endl;
      return;
   }
   if (sum(k >= k.at<float>(5,5)).val[0]/255 != 1){
      cout << "ERROR: Dip3::createGaussianKernel(): Seems like kernel is not centered!" << endl;
      return;
   }
   cout << "Message: Dip3::createGaussianKernel() seems to be correct" << endl;
}

void Dip3::test_circShift(void){
   
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
      cout << "ERROR: Dip3::circShift(): Result of circshift seems to be wrong!" << endl;
      return;
   }
   cout << "Message: Dip3::circShift() seems to be correct" << endl;
}

void Dip3::test_frequencyConvolution(void){
   
   Mat input = Mat::ones(9,9, CV_32FC1);
   input.at<float>(4,4) = 255;
   Mat kernel = Mat(3,3, CV_32FC1, 1./9.);

   Mat output = frequencyConvolution(input, kernel);
   
   if ( (sum(output < 0).val[0] > 0) or (sum(output > 255).val[0] > 0) ){
      cout << "ERROR: Dip3::frequencyConvolution(): Convolution result contains too large/small values!" << endl;
      return;
   }
   float ref[9][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 1, 1, (8+255)/9., (8+255)/9., (8+255)/9., 1, 1, 0},
                      {0, 1, 1, (8+255)/9., (8+255)/9., (8+255)/9., 1, 1, 0},
                      {0, 1, 1, (8+255)/9., (8+255)/9., (8+255)/9., 1, 1, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0}};
   for(int y=1; y<8; y++){
      for(int x=1; x<8; x++){
         if (abs(output.at<float>(y,x) - ref[y][x]) > 0.0001){
            cout << "ERROR: Dip3::frequencyConvolution(): Convolution result contains wrong values!" << endl;
            return;
         }
      }
   }
   cout << "Message: Dip3::frequencyConvolution() seems to be correct" << endl;
}
