/*
 *  fos_regressor.cpp
 *  fast-orthogonal-search
 *
 *  Created by Gregg johns on 14-03-09.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_regressor.h"

FosRegressor::FosRegressor(string data_path, string equ_path){
	//load the equations into memory.
	vector<string> equations = read_equations(equ_path);
	
	//load the training data into memory.
	try{
		load_data(data_path);
	}catch (int e){
		std::stringstream errMsg;
		
		errMsg << "ARFF file at " << data_path << " could not be loaded.\n";
		
		throw errMsg.str();
	}	
	
	candidates = train_candidates(equations);
	
	free_data();
}

FosRegressor::~FosRegressor(){
	free_ground_truth();
	delete_candidates(candidates);
}

void FosRegressor::free_ground_truth(){
	if (y){
		delete []y;
		y = NULL;
	}
}

void FosRegressor::free_data(){
	int i;
	
	if (x){
		for(i = 0 ; i < num_attributes ; i++ ){
			delete x[i] ;   
		}
		delete [] x ;
		
		x = NULL;
	}
}

void FosRegressor::delete_candidates(vector <FosCandidate*> candidates){
	int i;
	
	for (i=0; i< candidates.size(); i++){
		delete candidates.at(i);
	}
	
	candidates.clear();
}

FosModel* FosRegressor::train_regressor(int MAX_ORDER){
	
	 FosModel* model = train(candidates, y, num_instances, MAX_ORDER);	
	
	return model;
}

FosResult* FosRegressor::train_regressor(vector<string> test_files, int MAX_ORDER){
	int i, n, j;
	
	value_type *y_bar; 
	
	value_type *x_in;
	
	FosModel* model = train(candidates, y, num_instances, MAX_ORDER);
	
	free_ground_truth();
	
	FosResult* result = new FosResult();
	result->set_model(model);
	
	result->set_test_file(test_files);

	model->init_pt_by_pt();
	
	//loop through each model.
	for (i=0; i<test_files.size(); i++){
		//load the test file.
		load_data(test_files.at(i));
		
		x_in = new value_type[num_attributes];
		y_bar = new value_type[num_instances];
		
		for (n=0; n<num_instances; n++){
			for (j=0; j<num_attributes; j++){
				x_in[j] = x[j][i];
			}
			
			y_bar[n]=model->pt_by_pt(x_in);
		}
		
		result->add_response_data(y_bar, num_instances);
		result->add_mean_squared_error(model->calculate_mean_squared_error(y_bar, y, num_instances));
		result->add_relative_mean_squared_error(model->calculate_relative_mean_squared_error(y_bar, y, num_instances));
		result->add_correlation_coefficient(model->calculate_correlation_coefficient(y_bar,y,num_instances));
		result->add_average_absolute_error(model->calculate_average_absolute_error(y_bar,y,num_instances));	
		
		delete [] y_bar;
		delete [] x_in;
	}
	
	return result;
}

vector <FosCandidate*> FosRegressor::train_candidates(vector <string> equations){
	int num_candidates = equations.size();
	int i;
	value_type *result = new value_type[num_instances];
	string equ;
	FosCandidate * c;
	
	vector <FosCandidate*> candidates;
	
	value_type tmp;
	
	for (i=0; i<num_candidates; i++){
		equ = equations.at(i);
		
#ifdef DEBUG
		std::cout << "Training candidate " << equ << "\n";
#endif
		
		c = new FosCandidate(equations.at(i));
		
		//compute the candidate function training information.
		
		try{
			equ_parser.SetExpr(equ);		
			equ_parser.Eval(result, num_instances);
		}catch(mu::Parser::exception_type &e){
			mu::console() << _T("\nError:\n");
			mu::console() << _T("------\n");
			mu::console() << _T("Message:     ")   << e.GetMsg()   << _T("\n");
			mu::console() << _T("Expression:  \"") << e.GetExpr()  << _T("\"\n");
			mu::console() << _T("Token:       \"") << e.GetToken()    << _T("\"\n");
			mu::console() << _T("Position:    ")   << (int)e.GetPos() << _T("\n");
			mu::console() << _T("Errc:        ")   << std::dec << e.GetCode() << _T("\n");
		}
		
		//set the training data for the canidate
		c->set_training_data(result ,num_instances);
		
		tmp = time_average(result, num_instances);
		
		c->set_time_average(tmp);
		
		tmp = multiply_time_average(y, result, num_instances);
		c->set_ytime_average(tmp);
		
		//push into vector.
		candidates.push_back(c);
	}
	
	delete [] result;
	
	return candidates;
}

vector<string> FosRegressor::read_equations(string equ_path){
	string line;
	
	vector <string> equations;
	
	ifstream myfile (equ_path.c_str());
	
	//clear the old equation.
	equations.clear();
	
	if (myfile.is_open()){
		while ( getline (myfile,line)){
			equations.push_back(line);
		}
		myfile.close();
	}
	else{
		std::stringstream errMsg;
		errMsg << "Could not load equation fie: '" << equ_path << "'\n";
		throw errMsg.str();
	}
	
	return equations;
}

//x is loaded in as num_attributesxnum_intances.
void FosRegressor::load_data(string path){
	int  i,j;
	int gt_index;
	const char* tmp;
	ArffAttr* attr;
	ArffInstance * instance;
	ArffParser arff_parser(path);
	ArffData* data;
	
	data = arff_parser.parse();
	
	num_attributes = data->num_attributes();
	num_instances = data->num_instances();
	
#ifdef DEBUG
	std::cout << "Data parsed, there are " << num_attributes << " base attributes and " << num_instances << " observations.\n";
#endif
	
	//allocate space for training data.
	x = new value_type *[num_attributes];
	
	for (i=0; i<num_attributes; i++){
		x[i] = new value_type[num_instances];
	}
	
	//allocate space for ground truth.
	y = new value_type[num_instances];
	
	variables.clear();
	equ_parser.ClearVar();
	
	//add the variables to the parser.
	for (i=0; i < num_attributes; i++){
		attr = data->get_attr(i);
		
		if (attr->name() == system_output){
			gt_index = i;
		}
		
		variables.push_back(attr->name());
		
		tmp = attr->name().c_str();
		
		equ_parser.DefineVar(_T(tmp), x[i]);
	}
	
	//here we need to copy the observations into the array x.
	for (i=0; i<num_instances; i++){
		instance = data->get_instance(i);
		
		for (j=0; j< instance->size(); j++){
			if (gt_index == j)
				y[i] =  float(*(instance->get(j)));
			
			x[j][i] = float(*(instance->get(j)));
			
		}
	}
}
