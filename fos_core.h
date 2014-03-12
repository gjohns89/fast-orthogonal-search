/*
 *  fos_core.h
 *  fast-orthogonal-search
 *
 *  Created by Gregg johns on 14-03-09.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __FOS_CORE_H__
#define __FOS_CORE_H__
#include "fos_model.h"
#include "fos_candidate.h"
#include "arff_parser.h"
#include "arff_attr.h"
#include "muParser.h"
#include "fos_config.h"
#include "fos_result.h"

class FosCore{
	public:
	FosCore();
	~FosCore();
	
	protected:
	FosModel* train(vector<FosCandidate*>,value_type*,int,int);	
	void setSystemOutput(string);
	void setVariables(vector<string>);	
	
	value_type variance(value_type*, int);
		
	//this is the vector of the ground-truth.
	vector<string> variables;
	string system_output;

	value_type inline time_average(value_type *y, int N)
	{
		int n;
		value_type avg = 0.0;
		
		for (n = 0; n < N; n++){
			avg += (y[n]) / (N);
		}
		return avg;
	}
	
	value_type inline multiply_time_average(value_type *x, value_type *y, int N)
	{
		int n;
		value_type avg = 0.0;
		
		for (n = 0; n < N; n++)
		{
			avg += (x[n] * y[n]) / (N);
		}
		
		return avg;
	}
};
#endif