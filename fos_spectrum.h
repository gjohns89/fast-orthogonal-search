/*
 *  fos_spectrum.h
 *  fast-orthogonal-search
 *
 *  Created by Gregg johns on 14-03-12.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __FOS_SPECTRAL_ANALYZER_H__
#define __FOS_SPECTRAL_ANALYZER_H__

#include "fos_core.h"
#include "fos_candidate.h"

#include <math.h>

#ifndef value_type 
#define value_type float
#endif

class FosSpectralAnalyzer : public FosCore{

public:
	FosSpectralAnalyzer(int, value_type, int);
	~FosSpectralAnalyzer();
	void compute_psd(value_type *);


private:
	int f_max;
	value_type resolution;
	int N;
	value_type *psd;
	value_type *f;
	
	int n_bins;
	
	vector <FosCandidate*> candidates;

};

#endif 