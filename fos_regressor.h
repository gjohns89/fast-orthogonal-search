/*
 *  fos_regressor.h
 *  fast-orthogonal-search
 *
 *  Created by Gregg johns on 14-03-09.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#include "fos_core.h"

#include <string>
#include <stdexcept>
using namespace std;


class FosRegressor : public FosCore{


public:
	FosRegressor(string, string);
	~FosRegressor();

	vector<string> read_equations(string);
	
	FosModel* train_regressor(int);	
	FosResult* train_regressor(vector<string>,int);

private:
	vector <FosCandidate*> train_candidates(vector <string>);
	void load_data(string);
	void free_ground_truth();
	void delete_candidates(vector <FosCandidate*>);
	
	int num_instances;
	int num_attributes;
	value_type **x;
	value_type *y;
	vector <FosCandidate*> candidates;

	mu::Parser equ_parser;
	
	void free_data();

};