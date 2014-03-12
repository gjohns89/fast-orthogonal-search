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

#include <iostream>
#include <fstream>

using namespace std;
using namespace mu;

class FosModel{
	
	public:
	FosModel(vector <string>, string);
	FosModel(string);

	~FosModel();
	void add_candidate(FosCandidate*);
	vector<FosCandidate*> get_candidates();
	void add_coefficient(value_type );
	vector<value_type> get_coefficients();
	void set_model_order(value_type);
	value_type get_model_order();
	void init_pt_by_pt();
	string get_model_equation();
	string get_system_output();
	void set_system_output(string);
	void save_model(string);
	void load_model(string);
	
	value_type calculate_mean_squared_error(value_type*, value_type*,int);
	value_type calculate_relative_mean_squared_error(value_type*, value_type*,int);
	value_type calculate_correlation_coefficient(value_type *, value_type *,int);
	value_type calculate_average_absolute_error(value_type*, value_type*, int);	
	
	//x is a vector of the base attributes, it must match that width regardless.
	value_type  pt_by_pt(value_type* x_in){
		int N = variables.size();
		int i;
		value_type y = 0;
		
		value_type *v; 
		
		//copy input data to variable bindings.
		for (i=0; i<N; i++){
			x[i] = x_in[i];
		}
		
		//get output from parser - there should be M expressions.
		v = parser.Eval(M);	
		
		//calcualate actual model output :)
		for (i=0; i<M; i++){
			y+= a.at(0)*v[i];
		}
		
		return y;
	}

	
	private:
	
	vector<string> read_file(string );	
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
	int initialized;
};

#endif