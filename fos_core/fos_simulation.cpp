/*
 *  fos_simulation.cpp
 *  csv
 *
 *  Created by Gregg johns on 14-02-24.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_simulation.h"

FosSimulation::FosSimulation(string system_output, string equ_path, string train_path, vector<string> test_path):
equ_path(equ_path), train_path(train_path), test_path(test_path), x(0), y(0),
system_output(system_output), result_returned(0){
	
	result = new FosResult();
	result->set_test_file(test_path);
	result->add_train_file(train_path);	
}

FosSimulation::~FosSimulation(){
	free_data();
	free_ground_truth();
	delete_candidates();
	
	if (!result_returned)
		delete result;
}

void FosSimulation::delete_candidates(){
	int i;
	
	for (i=0; i< candidates.size(); i++){
		delete candidates.at(i);
	}
	
	candidates.clear();
}

void FosSimulation::train_candidates(){
	int num_candidates = equations.size();
	int i;
	value_type *result = new value_type[num_instances];
	string equ;
	FosCandidate * c;

	value_type tmp;
	
	for (i=0; i<num_candidates; i++){
		equ = equations.at(i);
		
		std::cout << "Training candidate " << equ << "\n";
		
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
}

//this will load the training data into memory, as well
//as the equations from file. It will generate the candidate functions 
//with the required training data.
void FosSimulation::init_simulation(){
	std::cout << "Loading training data.\n";
	
	//read the training arff into memory, initialize parser variable bindings.
	
	try{
		load_data(train_path);
	}catch (int e){
		std::cout << "ARFF file at " << train_path << " could not be loaded.\n";
		exit(-1);
	}
		
		
	std::cout << "Trainind data loaded.\n";
	
	//read the equations into memory from file at path equ_path.
	read_equations();
		
	std::cout << "Initializing candidate functions.\n";
	train_candidates();
	free_data();
		
}

void FosSimulation::free_data(){
	int i;
	
	if (x){
		for(i = 0 ; i < num_attributes ; i++ ){
			delete x[i] ;   
		}
		delete [] x ;
	
		x = NULL;
	}
}

void FosSimulation::free_ground_truth(){
	
	if (y){
		delete []y;
		y = NULL;
	}
}

void FosSimulation::load_data(string path){
	
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
	
	std::cout << "Data parsed, there are " << num_attributes << " base attributes and " << num_instances << " observations.\n";
		
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

void FosSimulation::read_equations(){
	
	string line;
	
	ifstream myfile (equ_path.c_str());
	
	//clear the old equation.
	equations.clear();
	
	if (myfile.is_open()){
		while ( getline (myfile,line)){
			equations.push_back(line);
		}
		myfile.close();
	}
	else cout << "Unable to open file"; 	
}

value_type inline FosSimulation::time_average(value_type *y, int N)
{
	int n;
	value_type avg = 0.0;
	
	for (n = 0; n < N; n++){
		avg += (y[n]) / (N);
	}
	return avg;
}

value_type inline FosSimulation::multiply_time_average(value_type *x, value_type *y, int N)
{
	int n;
	value_type avg = 0.0;
	
	for (n = 0; n < N; n++)
	{
		avg += (x[n] * y[n]) / (N);
	}

	return avg;
}

value_type FosSimulation::calculate_mean_squared_error(value_type *y_bar, value_type *y, int N){
	value_type mse = 0;
	int n;
	
	for (n=0; n<N; n++){
		mse += (y_bar[n] - y[n])*(y_bar[n]-y[n])/N;
	}
	return mse;
}

value_type FosSimulation::calculate_relative_mean_squared_error(value_type *y_bar, value_type *y, int N){
	value_type pmse = 0, tmp = 0;
	int n;
	
	value_type mean = time_average(y, N);
	
	for (n=0; n<N; n++){
		pmse += (y_bar[n] - y[n])*(y_bar[n]-y[n])/N;
		tmp += (mean-y[n])*(mean-y[n]);
	}
	return pmse/tmp;
}

value_type FosSimulation::calculate_correlation_coefficient(value_type *x, value_type *y, int N){
	value_type cc = 0, tmp_x = 0, tmp_y=0;
	int n;
	
	value_type x_mu, y_mu;
	
	x_mu = time_average(x, N);
	y_mu = time_average(y, N);
	
	for (n=0; n<N; n++){
		cc += (x[n]-x_mu)*(y[n]-y_mu);
		tmp_x += (x[n]-x_mu)*(x[n]-x_mu);
		tmp_y += (y[n]-y_mu)*(y[n]-y_mu);
	}
	
	cc = cc/(sqrt(tmp_x)*sqrt(tmp_y));
	
	return cc;
}
 
value_type FosSimulation::calculate_average_absolute_error(value_type *y_bar, value_type *y, int N){
	value_type aae = 0;
	int n;
	
	for (n=0; n<N; n++){
		aae += fabs((y_bar[n] - y[n]))/N;
	}
	return aae;
}

value_type FosSimulation::variance(value_type *x, int N){
	value_type mu;
	value_type var;
	int n;
	value_type tmp;

	mu = time_average(x, N);
	var = 0;
	
	for (n = 0; n < N; n++){
		tmp = (x[n] - mu) * (x[n] - mu);
		var += (tmp) / (N + 1);
	}
	
	return var;
}

FosResult* FosSimulation::run(int MAX_ORDER){
	FosModel *model;
	int i, j, k, N;
	value_type* response, *input, tmp;
	
	init_simulation();
	
	//train the fos model.
	model = train(MAX_ORDER);
	
	free_ground_truth();
	
	//initialize
	model->init_pt_by_pt();
	
	//now to do the testing. 

	for (i=0; i<test_path.size(); i++){
		//data is now loaded in x wich is n_attributes x n_instances.
		load_data(test_path.at(i));
		
		input = new value_type[num_attributes];
		
		response = new value_type[num_instances];
		
		N = num_instances;
		
		//get the response from the model pt-by-pt
		for (j=0; j<num_instances; j++){
			//copy one feature vector into input.
			for (k=0; k<num_attributes; k++){
				input[k] = x[k][j];
			}
			
			response[j] = model->pt_by_pt(input);
		}
		
		result->add_response_data(response,N);
		
		tmp = calculate_average_absolute_error(response, y, N);
		result->add_average_absolute_error(tmp);
		
		tmp = calculate_mean_squared_error(response, y, N);
		result->add_mean_squared_error(tmp);
		
		tmp = calculate_relative_mean_squared_error(response, y, N);
		result->add_relative_mean_squared_error(tmp);
		
		tmp = calculate_correlation_coefficient(response, y, N);
		result->add_correlation_coefficient(tmp);
		
		delete []input;
		free_data();
		free_ground_truth();
	}
	
	result->set_fos_model(model);
	
	result_returned = 1;

	return result;
	
}

FosModel* FosSimulation::train(int MAX_ORDER){
		
	FosModel *model;
	
	int flag=1;
	
	//the actual order of the model, total and currently. 
	int M = 0;
	int c, r, i, m, j;
	
	value_type D[MAX_ORDER + 1][MAX_ORDER + 1];
	value_type C[MAX_ORDER + 1];
	value_type gm[MAX_ORDER + 1];
	
	//pointer to current working training data.
	value_type *pm[MAX_ORDER + 1];
	
	//pointer to current working candidate.
	FosCandidate *cand;
	
	//pointer to current maximum in that fos iteration term search... wait what?
	FosCandidate *max;
	
	value_type alpha[MAX_ORDER + 1][MAX_ORDER];
	
	value_type maxG = 0, maxC = 0, mse, ta_yy, pmse, prev_pmse=0, var_y, a;
	
	value_type maxAlpha[MAX_ORDER + 1];
	value_type maxD[MAX_ORDER + 1];
	value_type v[MAX_ORDER + 1];

	ta_yy = multiply_time_average(y,y,num_instances);
	var_y = variance(y, num_instances);
	
	model = new FosModel(variables, system_output);
	
	//initialization.
	D[0][0] = 1;
	C[0] = time_average(y, num_instances);
	gm[0] = C[0] / D[0][0];
	
	//cannot exceed the maximum order.
	while(M <= MAX_ORDER){
		//std::cout << "Finding term " << M << ".\n";
		
		//iterate through all of the candidates.
		for (c=0; c<candidates.size(); c++){		
			
			//std::cout << "Trying candidate " << c << "\n";
			
			/*if that candidate has already been selected
			  as a model term, then we simply go to the next one.*/
			if ((candidates.at(c))->get_selected()){
				continue;
			}
			
			cand = candidates.at(c);
			
			//get a pointer to the training data for candidate c.
			pm[M] = cand->get_training_data();
			
			D[M][0] = cand->get_time_average();
			
			//must init to zero since C[M] is computed recursively.
			C[M] = 0;
			
			for (r=0; r<= (M-1); r++){
				//calculate projection coefficent alpha.
				alpha[M][r] = (D[M][r]) / (D[r][r]);
				D[M][r + 1] = 0;
				
				for (i = 0; i <= (r); i++){
					D[M][r + 1] -= alpha[r + 1][i] * D[M][i];
				}
				
				D[M][r + 1] += multiply_time_average(pm[M], pm[r + 1], num_instances);
				
				C[M] -= alpha[M][r] * C[r];
			}
			
			C[M] += cand->get_ytime_average();
			
			//std::cout << "C=" << C[M] << "\n";
			//std::cout << "D=" << D[M][M] << "\n";
			
			gm[M] = C[M] / D[M][M];
			
			//std::cout << "G=" << gm[M] << "\n"; 
			
			if ((D[M][M] > D_THRESHOLD) && (gm[M] > G_THRESHOLD)){
				cand->set_q(gm[M] * gm[M] * D[M][M]);
			}
			else{
				cand->set_q(0);
			}
			
			//std::cout << "Q=" << cand->get_q() << "\n";
			
			/*maxQ is init to zero, first iteration is pointless but oh well */
			if (flag){
				maxG = gm[M];
				maxC = C[M];
				max = cand;
				
				for (i = 0; i <= (M - 1); i++){
					maxAlpha[i] = alpha[M][i];
					maxD[i] = D[M][i];
				}
				
				maxD[M] = D[M][M];
				flag = 0;
				
			}
			else if ((cand->get_q() > max->get_q())){
				maxG = gm[M];
				maxC = C[M];
				max = cand;
				
				for (i = 0; i <= (M - 1); i++){
					maxAlpha[i] = alpha[M][i];
					maxD[i] = D[M][i];
				}
				maxD[M] = D[M][M];
			}
		}//end of looping through candidates
		
		pm[M] = max->get_training_data();
		gm[M] = maxG;
		C[M] = maxC;
		
		max->set_selected(1);
		
		//add that candidate to the model
		model->add_candidate(max);
		
		for (i = 0; i <= (M - 1); i++){
			alpha[M][i] = maxAlpha[i];
			D[M][i] = maxD[i];
		}
		
		D[M][M] = maxD[M];
		
		for (i = 0; i <= (M - 1); i++){
			alpha[M][i] = maxAlpha[i];
		}
		
		mse = ta_yy;
		
		//calculate the mean-squared error
		
		for (m = 0; m <= (M); m++){
			mse -= gm[m] * gm[m] * D[m][m];
		}
		
		pmse = (mse / var_y) * 100;
		
		if ((max->get_q() < MSE_REDUCTION_THRESHOLD)) /*|| (pmse < PMSE_THRESHOLD) || (fabs(prev_pmse - pmse) < PMSE_CHANGE_THRESHOLD))*/{
			//std::cout << "Stop criteria met at M=" << M << ".\n";
			//std::cout << "Q=" << max->get_q() << "\n";
			//std::cout << "PMSE="<< pmse << "\n";
			//std::cout << "dPMSE=" << (fabs(prev_pmse - pmse)) << "\n";
			break;
		}
		
		prev_pmse = pmse;
		flag = 1;
		M++;
	}//end of looping through model terms.
	
	//if stop-criteria isn't met, we increase M past MAX_ORDER
	if (M > MAX_ORDER){
		M--;
	}
	
	//std::cout << "Calculating model coeffients.\n";
	
	for (m = 0; m <= M; m++){
		a = 0;
				
		for (j = 0; j <= M; j++){
			v[j] = 0;
		}
		
		for (i = m; i <= M; i++){
			
			if (i == m){
				v[i] = 1;

			}
			else{
				for (r = m; r <= (i - 1); r++){
					v[i] -= alpha[i][r] * v[r];
				}
			}
			
			a += gm[i] * v[i];
		}

		if (fabs(a) < A_THESHOLD){
			a = 0;
		}
		
		//std::cout << "Adding a[" << m << "]=" << a << " to model.\n";
		model->add_coefficient(a);		
	}
	
	//std::cout << "Setting model order to " << M << ".\n";
	
	model->set_model_order(M);
	
	return model;
}