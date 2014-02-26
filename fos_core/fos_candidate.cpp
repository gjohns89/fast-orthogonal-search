/*
 *  fos_candidate.cpp
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_candidate.h"


FosCandidate::FosCandidate(string equ):equ(equ),ta(0),
y_ta(0),selected(0), Q(0), x(0){

}

FosCandidate::~FosCandidate(){
	if (x != NULL)
		delete []x;
}

FosCandidate* FosCandidate::clone(){
	FosCandidate *c = new FosCandidate(equ);
	c->set_time_average(ta);
	c->set_ytime_average(y_ta);
	c->set_q(Q);
	c->set_training_data(x,N);
	c->set_selected(selected);

	return c;
}

string FosCandidate::get_equ(){
	return equ;
}

double FosCandidate::get_time_average(){
	return ta;
}

double FosCandidate::get_ytime_average(){
	return y_ta;
}

double FosCandidate::get_q(){
	return Q;
}

int FosCandidate::get_selected(){
	return selected;
}

void FosCandidate::set_selected(int selected){
	this->selected = selected;
}

void FosCandidate::set_time_average(value_type ta){
	this->ta = ta;
	
}

void FosCandidate::set_ytime_average(value_type y_ta){
	this->y_ta = y_ta;
	
}

void FosCandidate::set_q(value_type Q){
	this->Q = Q;
}

void FosCandidate::set_training_data(value_type *x, int N){
	int i;
	this->x = new value_type[N];
	this->N = N;
	
	for(i=0; i<N; i++){
		this->x[i] = x[i];
	}
	
}

double* FosCandidate::get_training_data(){
	return x;
}
