/*
 *  fos_model.cpp
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

/*
 * 
 * @TODO: change the equation parser to only create variables for the used candidates
 *        and print out variable binding map for programming dynamic models.
 *
 *
 */
#include "fos_model.h"


FosModel::FosModel(vector <string> variables, string system_output):variables(variables), 
system_output(system_output),M(0), initialized(0){

}

FosModel::FosModel(string base_path){
	load_model(base_path);
}

FosModel::~FosModel(){
	int i;
	
	delete []x;
	
	for (i=0; i<p.size(); i++)
		delete p.at(i);
}

value_type FosModel::calculate_mean_squared_error(value_type *y_bar, value_type *y, int N){
	value_type mse = 0;
	int n;
	
	for (n=0; n<N; n++){
		mse += ((y_bar[n] - y[n])*(y_bar[n]-y[n]))/N;
	}
	return mse;
}

value_type FosModel::calculate_relative_mean_squared_error(value_type *y_bar, value_type *y, int N){
	value_type pmse = 0, tmp = 0;
	int n;
	
	//	value_type mean = time_average(y, N);
	
	for (n=0; n<N; n++){
		pmse += (y_bar[n] - y[n])*(y_bar[n]-y[n])/N;
		
		tmp += (y[n])*(y[n]);
	}
	return pmse/tmp;
}

value_type FosModel::calculate_correlation_coefficient(value_type *x, value_type *y, int N){
	value_type cc = 0, tmp_x = 0, tmp_y=0;
	int n;
	
	value_type x_mu=0, y_mu=0;
	
	for (n=0; n<N; n++){
		x_mu += x[n]/N;
		y_mu += y[n]/N;
	}
	
	for (n=0; n<N; n++){
		cc += (x[n]-x_mu)*(y[n]-y_mu);
		tmp_x += (x[n]-x_mu)*(x[n]-x_mu);
		tmp_y += (y[n]-y_mu)*(y[n]-y_mu);
	}
	
	cc = cc/(sqrt(tmp_x)*sqrt(tmp_y));
	
	return cc;
}

value_type FosModel::calculate_average_absolute_error(value_type *y_bar, value_type *y, int N){
	value_type aae = 0;
	int n;
	
	for (n=0; n<N; n++){
		aae += fabs((y_bar[n] - y[n]))/N;
	}
	return aae;
}

vector<string> FosModel::read_file(string path){
	
	string line;
	vector <string> data;
	
	ifstream myfile (path.c_str());
	
	//clear the old equation.
	data.clear();
	
	if (myfile.is_open()){
		while ( getline (myfile,line)){
			data.push_back(line);
		}
		myfile.close();
	}
	else cout << "Unable to open file: " << path << "\n";
	
	return data;
}

void FosModel::load_model(string base_path){
	stringstream stream;
	vector <string> tmp;
	int i;
	
	stream << base_path << "_constants.txt";
	
 	tmp = read_file(stream.str());
	
	for (i=0; i<tmp.size(); i++){
		double dval = atof(tmp.at(i).c_str());
		a.push_back((value_type)dval);
	}
	
	stream.str( std::string());
	stream.clear();
	
	stream << base_path << "_var.txt";
	
	variables = read_file(stream.str());
	
	stream.str( std::string() );
	stream.clear();	
	
	//read the candidate function equations.
	stream << base_path << ".txt";

	tmp = read_file(stream.str());
	
	//first equation is system output.
	system_output = tmp.at(0);
	
	for (i=1; i<tmp.size(); i++){
		FosCandidate *cand = new FosCandidate(tmp.at(i));
		p.push_back(cand);
	}
	
	M = tmp.size()-1;
}

void FosModel::save_model(string base_path){
	stringstream path;
	ofstream myfile;
	int m;
	
	//first write all the model coefficients to file.
	path << base_path << "_constants.txt";
		
	myfile.open (path.str().c_str());
	
	for (m=0; m<M;m++){
		myfile << a.at(m) << "\n";
	}
	
	myfile.close();
	
	path.str( std::string() );
	path.clear();

	path << base_path << ".txt";
	
	myfile.open (path.str().c_str());
	
	myfile << system_output << "\n";
	
	for (m=0; m<M;m++){
		myfile << p.at(m)->get_equ() << "\n";
	}
	
	myfile.close();
	
	path.str( std::string() );
	path.clear();	
	
	path << base_path << "_var.txt"; 
	
	myfile.open (path.str().c_str());
	
	for (m=0; m< variables.size();m++){
		myfile << variables.at(m) << "\n";
	}
	
	myfile.close();
	
	
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
	
	
	if (initialized)
		return; 
	
	initialized = 1;
	
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