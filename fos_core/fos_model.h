#ifndef __INCLUDED_FOS_MODEL_H__
#define __INCLUDED_FOS_MODEL_H__

/*
 *  fos_model.h
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>

#include "fos_candidate.h"


#include "muParser.h"

using namespace std;
using namespace mu;

class FosModel{
	
	public:
	FosModel(vector <string>, string);
	~FosModel();
	void add_candidate(FosCandidate*);
	vector<FosCandidate*> get_candidates();
	void add_coefficient(value_type );
	vector<value_type> get_coefficients();
	void set_model_order(value_type);
	value_type get_model_order();
	void init_pt_by_pt();
	value_type pt_by_pt(value_type *);
	string get_model_equation();
	string get_system_output();
	void set_system_output(string);
	
	private:
	value_type *x; 
	string system_output;
	
	vector<string> variables;
	
	//model term coefficients.
	vector<value_type> a;
	
	//model term functions.
	vector<FosCandidate*>p;
	
	//the order of the model.
	int M; 
	
	mu::Parser parser;	
};

#endif