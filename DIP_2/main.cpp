//============================================================================
// Name        : main.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : only calls processing and test routines
//============================================================================

#include <iostream>

#include "Dip2.h"

using namespace std;

// usage: argv[1] == "generate" to generate noisy images, path to original image in argv[2]
// 	    argv[1] == "restorate" to load and restorate noisy images
// main function. only calls processing and test routines
int main(int argc, char** argv) {

   // check if enough arguments are defined
   if (argc < 2){
      cout << "Usage:\n\tdip2 generate path_to_original\n\tdip2 restorate"  << endl;
      cout << "Press enter to exit"  << endl;
      cin.get();
      return -1;
   }

   // construct processing object
   Dip2 dip2;

   // in a first step generate noisy images
   // path of original image is in argv[2]
   if (strcmp(argv[1], "generate") == 0){
      // check whether original image was specified
      if (argc < 3){
         cout << "ERROR: original image not specified"  << endl;
         cout << "Press enter to exit"  << endl;
         cin.get();
         return -2;
      }
      string fname = argv[2];
      dip2.generateNoisyImages(fname);
   }	

   // in a second step try to restorate noisy images
   if (strcmp(argv[1], "restorate") == 0){
      // run some test routines
      dip2.test();
      // start processing
      dip2.run();
   }

	return 0;
} 
