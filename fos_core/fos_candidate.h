#ifndef __INCLUDED_FOS_CANDIDATE_H__
#define __INCLUDED_FOS_CANDIDATE_H__

/*
 *  fos_candidate.h
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>

#include "muParser.h"

using namespace mu;
using namespace std;


class FosCandidate{
	public:
	FosCandidate(string);
	~FosCandidate();
	
	string get_equ();
	double get_time_average();
	double get_ytime_average();
	double get_q();
	int get_selected();
	void set_selected(int);
	void set_time_average(value_type);
	void set_ytime_average(value_type);
	void set_q(value_type);

	FosCandidate* clone();
	
	void set_training_data(value_type*, int);
	value_type* get_training_data();
	
	private:
	string equ;
	value_type ta;
	value_type y_ta;
	int selected;
	value_type Q;
	value_type *x;
	int N; //this is the length of x
};

#endif