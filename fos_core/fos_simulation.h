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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;
using namespace mu;


class FosSimulation{
	public:
	FosSimulation(string,string,string,string,string);
	~FosSimulation();
	
	void set_training_arff(string);
	void add_testing_arff(string);
	value_type variance(value_type *, int );

	
	FosModel* train(int);
	void set_output_path(string);
		
	private: 
	void init_simulation();
	string training_arff;
	vector<string> testing_arff;
	double inline time_average(double *, int );
	double inline multiply_time_average(double*,double*, int);

	//equation parsing functions.
	void read_equations();
	void load_data(string);
	void free_data();
	void train_candidates();
	void delete_candidates();
	void free_ground_truth();

	string system_output;
	string equ_path;
	string train_path;
	string test_path;
	string output_path;
	
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

};

#endif