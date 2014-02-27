/*
 *  fos_simulation.h
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef __INCLUDED_FOS_SIMULATION_H__
#define __INCLUDED_FOS_SIMULATION_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <locale>
#include <limits>
#include <ios> 
#include <iomanip>
#include <numeric>
#include <fstream>


#include "fos_model.h"
#include "fos_candidate.h"
#include "arff_parser.h"
#include "arff_attr.h"
#include "muParser.h"
#include "fos_config.h"
#include "fos_result.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;
using namespace mu;

class FosSimulation{
	public:
	FosSimulation(string,string,string,vector<string>);
	~FosSimulation();
	 	
	FosResult* run(int );
		
	private: 
	void init_simulation();
	value_type inline time_average(value_type *, int );
	value_type inline multiply_time_average(value_type*,value_type*, int);
	value_type calculate_mean_squared_error(value_type*, value_type*, int);
	value_type calculate_relative_mean_squared_error(value_type*, value_type*, int);
	value_type calculate_average_absolute_error(value_type*, value_type*, int);
	value_type calculate_correlation_coefficient(value_type*, value_type*, int);
	
	//equation parsing functions.
	void read_equations();
	void load_data(string);
	void free_data();
	void train_candidates();
	void delete_candidates();
	void free_ground_truth();
	FosModel* train(int);
	value_type variance(value_type *, int );
	
	string system_output;
	string equ_path;
	string train_path;
	vector<string> test_path;
	int num_attributes; 
	int num_instances;
	mu::Parser equ_parser;
	vector<string> equations;
	vector<string> variables;
	vector <FosCandidate*> candidates;
	
	//input base feature array.
	value_type **x;
	
	//ground truth array
	value_type *y;
	
	FosResult * result;
	int result_returned;
};

#endif