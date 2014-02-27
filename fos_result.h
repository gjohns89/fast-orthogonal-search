/*
 *  fos_result.h
 *  csv
 *
 *  Created by Gregg johns on 14-02-27.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __FOS_RESULT_H__
#define __FOS_RESULT_H__

#include "muParser.h"
#include "fos_model.h"

using namespace std;
using namespace mu;

class FosResult{
	
	public:
	FosResult();
	~FosResult();
	
	void add_response_data(value_type *, int);
	void add_correlation_coefficient(value_type);
	void add_average_absolute_error(value_type);
	void add_relative_mean_squared_error(value_type);
	void add_mean_squared_error(value_type);
	
	void set_test_file(vector <string>);
	void add_test_file(string);
	void add_train_file(string);
	
	string to_string();
	
	void set_fos_model(FosModel *model);
	FosModel* get_fos_model();

	vector<value_type*> get_response_data();
	vector<int> get_response_size();
	vector<value_type> get_cross_correlation();
	vector<value_type> get_average_absolute_error();
	vector<value_type> get_relative_mean_squared_error();
	vector<value_type> get_mean_squared_error();
	
	private:
	
	FosModel *model;
	vector<value_type> mse;
	vector<value_type> rmse;
	vector<value_type> aae;
	vector<value_type> cc;
	
	//the output of the model that generated this response.
	vector<value_type*> response;
	vector<int> N;

	vector<string> train_file;
	vector<string> test_file;	
};

#endif