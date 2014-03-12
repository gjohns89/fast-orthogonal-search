/*
 *  fos_result.cpp
 *  csv
 *
 *  Created by Gregg johns on 14-02-27.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_result.h"

FosResult::FosResult():response(0){
	
}

FosResult::~FosResult(){
	int i; 
	
	for (i=0; i<response.size(); i++)
		delete [] response.at(i);
	} 

void FosResult::set_model(FosModel* model){
	this->model = model;
}

string FosResult::to_string(){
	int n,N;
	stringstream stream;
	N = mse.size();
	
	for (n=0; n<N; n++){
		stream << "Test File: " << test_file.at(n) << "\n";
		stream << "MSE: " << mse.at(n) << "\n";
		stream << "RMSE: " << rmse.at(n) << "\n";
		stream << "AAE: " << aae.at(n) << "\n";
		stream << "R^2: " << cc.at(n)*cc.at(n) << "\n";
		stream << "CC: " << cc.at(n) << "\n\n";
	}
		
	return stream.str();
}

void FosResult::add_mean_squared_error(value_type mse){
	this->mse.push_back(mse);
}

void FosResult::add_relative_mean_squared_error(value_type rmse){
	this->rmse.push_back(rmse);
}

void FosResult::add_average_absolute_error(value_type aae){
	this->aae.push_back(aae);
}

void FosResult::add_correlation_coefficient(value_type cc){
	this->cc.push_back(cc);
}

void FosResult::add_test_file(string test_file){
	this->test_file.push_back(test_file);
}

void FosResult::set_test_file(vector <string> test_file){
	this->test_file = test_file;
}

void FosResult::add_response_data(value_type * response, int N){
	int n;
	value_type *tmp = new value_type[N];
	
	for (n=0; n<N; n++)
		tmp[n] = response[n];
	
	this->response.push_back(tmp);
	this->N.push_back(N);
}

vector<value_type*> FosResult::get_response_data(){
	return response;
}
	
vector<int> FosResult::get_response_size(){
	return N;
}

vector<value_type> FosResult::get_cross_correlation(){
	return cc;
}

vector <value_type> FosResult::get_average_absolute_error(){
	return aae;
}

vector<value_type> FosResult::get_relative_mean_squared_error(){
	return rmse;
}

vector<value_type> FosResult::get_mean_squared_error(){
	return mse;
}
