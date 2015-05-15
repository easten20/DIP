//============================================================================
// Name        : Dip1.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : 
//============================================================================

#include "Dip1.h"

// function that splits an image and mirrors one side
/*
img 		input image
direction	0 - mirror left to right
			1 - mirror right to left
			2 - mirror top to bottom
			3 - mirror bottom to top

referenced to the original image
*/
void Dip1::splitImage(Mat& img, int direction){

  	int yMid = floor(img.rows/2.0);
  	int xMid = floor(img.cols/2.0);
	
  	switch(direction){
  		case 0:
  			for( int y = 0; y < img.rows; y++ ){
		  		for( int x = 0; x <= xMid; x++ ){
						img.at<Vec3b>(y, img.cols-x) = img.at<Vec3b>(y,x);
				}
			}
  			break;
  		case 1:
  			for( int y = 0; y < img.rows; y++ ){
		  		for( int x = img.cols-1; x >= xMid; x-- ){
						img.at<Vec3b>(y, img.cols-x) = img.at<Vec3b>(y,x);
				}
			}
  			break;
  		case 2:
  			for( int y = 0; y <= yMid; y++ ){
		  		for( int x = 0; x < img.cols; x++ ){
						img.at<Vec3b>(img.rows-y, x) = img.at<Vec3b>(y,x);
				}
			}
  			break;
  		case 3:
  			for( int y = img.rows-1; y >= yMid; y-- ){
		  		for( int x = 0; x < img.cols; x++ ){
						img.at<Vec3b>(img.rows-y, x) = img.at<Vec3b>(y,x);
				}
			}
  			break;	
  		default: 
  			cout << "you have to specify a valid direction:" << endl;
			cout << "\t0 - mirror left to right" << endl;
			cout << "\t1 - mirror right to left" << endl;
			cout << "\t2 - mirror top to bottom" << endl;
			cout << "\t3 - mirror bottom to top" << endl;
  	}	
}

// function that performs some kind of (simple) image processing
/*
img	input image
return	output image
*/
Mat Dip1::doSomethingThatMyTutorIsGonnaLike(Mat& img){

  	Mat newImg = img.clone();
  	
  	splitImage(newImg, 0);
  	splitImage(newImg, 3);	

	//increase image contrast 
	/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
	float alpha = 1.2;
	int beta = 50;
	for( int y = 0; y < newImg.rows; y++ ){
		for( int x = 0; x < newImg.cols; x++ ){ 
			for( int c = 0; c < 3; c++ ){
				newImg.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( alpha*( newImg.at<Vec3b>(y,x)[c] ) + beta ); 
			}
		}
	}

  	return newImg;
}

/* *****************************
  GIVEN FUNCTIONS
***************************** */

// function loads input image, calls processing function, and saves result
/*
fname	path to input image
*/
void Dip1::run(string fname){

	// window names
	string win1 = string ("Original image");
	string win2 = string ("Result");
  
	// some images
	Mat inputImage, outputImage;
  
	// load image
	cout << "load image" << endl;
	inputImage = imread( fname );
	cout << "done" << endl;
	
	// check if image can be loaded
	if (!inputImage.data){
	    cout << "ERROR: Cannot read file " << fname << endl;
		cout << "Continue with pressing enter..." << endl;
	    cin.get();
	    exit(-1);
	}
	
	// show input image
	namedWindow( win1.c_str() );
	imshow( win1.c_str(), inputImage );
	
	// do something (reasonable!)
	outputImage = doSomethingThatMyTutorIsGonnaLike( inputImage );
	
	// show result
	namedWindow( win2.c_str() );
	imshow( win2.c_str(), outputImage );
	
	// save result
	imwrite("result.jpg", outputImage);
	
	// wait a bit
	waitKey(0);

}

// function loads input image and calls processing function
// output is tested on "correctness" 
/*
fname	path to input image
*/
void Dip1::test(string fname){

	// some image variables
	Mat inputImage, outputImage;
  
	// load image
	inputImage = imread( fname );

	// check if image can be loaded
	if (!inputImage.data){
	    cout << "ERROR: Cannot read file " << fname << endl;
	    cout << "Continue with pressing enter..." << endl;
	    cin.get();
	    exit(-1);
	}

	// create output
	outputImage = doSomethingThatMyTutorIsGonnaLike( inputImage );
	// test output
	test_doSomethingThatMyTutorIsGonnaLike(inputImage, outputImage);
	
}

// function loads input image and calls processing function
// output is tested on "correctness" 
/*
inputImage	input image as used by doSomethingThatMyTutorIsGonnaLike()
outputImage	output image as created by doSomethingThatMyTutorIsGonnaLike()
*/
void Dip1::test_doSomethingThatMyTutorIsGonnaLike(Mat& inputImage, Mat& outputImage){

	// ensure that input and output have equal number of channels
	if ( (inputImage.channels() == 3) && (outputImage.channels() == 1) )
		cvtColor(inputImage, inputImage, CV_BGR2GRAY);

	// split (multi-channel) image into planes
	vector<Mat> inputPlanes, outputPlanes;
	split( inputImage, inputPlanes );
	split( outputImage, outputPlanes );

	// number of planes (1=grayscale, 3=color)
	int numOfPlanes = inputPlanes.size();

	// calculate and compare image histograms for each plane
	Mat inputHist, outputHist;
	// number of bins
	int histSize = 100;
	float range[] = { 0, 256 } ;
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	double sim = 0;
	for(int p = 0; p < numOfPlanes; p++){
		// calculate histogram
		calcHist( &inputPlanes[p], 1, 0, Mat(), inputHist, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &outputPlanes[p], 1, 0, Mat(), outputHist, 1, &histSize, &histRange, uniform, accumulate );
		// normalize
		inputHist = inputHist / sum(inputHist).val[0];
		outputHist = outputHist / sum(outputHist).val[0];
		// similarity as histogram intersection
		sim += compareHist(inputHist, outputHist, CV_COMP_INTERSECT);
	}
	sim /= numOfPlanes;

	// check whether images are to similar after transformation
	if (sim >= 0.8)
		cout << "The input and output image seem to be quite similar (similarity = " << sim << " ). Are you sure your tutor is gonna like your work?" << endl;

}
