/*
 *  fos_spectrum.cpp
 *  fast-orthogonal-search
 *
 *  Created by Gregg johns on 14-03-12.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_spectrum.h"

/*The spectral analyzer can be reused provided the signals are all the same length,
 This is done for speed of computation particularly when the data comes in epochs.*/

FosSpectralAnalyzer::FosSpectralAnalyzer(int f_max, value_type resolution, int N){
	this->f_max = f_max;
	this->resolution = resolution;
	
	//N is the length of the input signal.
	this->N = N;
	int i;
	
	value_type tmp = (float)f_max/resolution;
	
	
	if (floor(tmp) != tmp){
		std::stringstream errMsg;
		errMsg << "f_max/resolution must result in an integer scalar.\n";
		throw errMsg.str();
	}
	
	//we add 1 for a DC constant. 
	n_bins = (int)tmp + 1;
	
	//make a variable for the psd.
	psd = new value_type[n_bins];
	f = new value_type[n_bins];
	
	for (i=0; i<n_bins; i++){
		f[i] = i*resolution;
		std::cout << f[i] << "\n";
	}
}

FosSpectralAnalyzer::~FosSpectralAnalyzer(){
	delete [] psd;
	delete []f;
}

void FosSpectralAnalyzer::compute_psd(value_type *x){

}