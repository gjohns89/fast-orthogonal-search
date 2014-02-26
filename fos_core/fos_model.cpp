/*
 *  fos_model.cpp
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_model.h"


FosModel::FosModel(vector <string> variables, string system_output):variables(variables), 
system_output(system_output),M(0){

}

FosModel::~FosModel(){
	delete []x;
}

void FosModel::set_system_output(string system_output){
	this->system_output = system_output;
}

string FosModel::get_system_output(){
	return system_output;
}

void FosModel::set_model_order(value_type M){
	this->M = M+1;
}

value_type FosModel::get_model_order(){
	return M-1;
}

string FosModel::get_model_equation(){
	int m;
	stringstream equ;
	
	equ << system_output << "=";
	
	//create the model expression.
	for (m=0; m<(M-1); m++){
		equ << "(" << a.at(m) << ")"<<"*(" << (p.at(m))->get_equ() << ")+";
	}
	
	equ << "(" <<a.at(M-1) << ")" << "*(" << (p.at(m))->get_equ() << ")";
	
	return equ.str();
}

void FosModel::init_pt_by_pt(){
	int m, i;
	
	stringstream model_equ;
	
	//allocate size for variable bindings.
	x = new value_type[variables.size()];
		
	//create the model expression.
	for (m=0; m<(M-1); m++){
		
		model_equ << (p.at(m))->get_equ() << ",";
	}
	
	model_equ << p.at(M-1)->get_equ();
	
	
	//add all the original variables.
	for (i=0; i<variables.size(); i++){
		parser.DefineVar(_T(variables.at(i).c_str()), &x[i]);
	}

	parser.SetExpr(model_equ.str());
	
}

//x is a vector of the 
value_type  FosModel::pt_by_pt(value_type* x_in){
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

/*void FosModel::response(value_type *y, value_type **x_in, int N){
	
}*/


void FosModel::add_candidate(FosCandidate *c){
	
	FosCandidate *new_c = c->clone();
	p.push_back(new_c);
}

vector<FosCandidate*> FosModel::get_candidates(){
	return p;
}

void FosModel::add_coefficient(value_type a){
	this->a.push_back(a);
}

vector<value_type> FosModel::get_coefficients(){
	return a;
}