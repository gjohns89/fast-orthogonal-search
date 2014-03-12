/*
 *  fos_core.cpp
 *  fast-orthogonal-search
 *
 *  Created by Gregg johns on 14-03-09.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "fos_core.h"

FosCore::FosCore(){
}

FosCore::~FosCore(){
}

void FosCore::setSystemOutput(string system_output){
	this->system_output = system_output;
}

void FosCore::setVariables(vector<string> variables){
	this->variables = variables;
}

value_type FosCore::variance(value_type *x, int N){
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

FosModel* FosCore::train(vector <FosCandidate*> candidates, value_type *y, int num_instances, int MAX_ORDER){
	
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
#ifdef DEBUG
		std::cout << "Finding term " << M << ":\n";
#endif
		
		//iterate through all of the candidates.
		for (c=0; c<candidates.size(); c++){		
			
			/*if that candidate has already been selected
			 as a model term, then we simply go to the next one.*/
			if ((candidates.at(c))->get_selected()){
				continue;
			}
			
			cand = candidates.at(c);
#ifdef DEBUG
			std::cout << "Trying candidate: " << cand->get_equ() << "\n";
#endif
			
			//get a pointer to the training data for candidate c.
			pm[M] = cand->get_training_data();
			
			D[M][0] = multiply_time_average(pm[M], pm[0], num_instances);//cand->get_time_average()*time_average(pm[M], num_instances);
			
#ifdef DEBUG		
			std::cout << "D[" << M << "][0]=" << D[M][0] << "\n";
#endif
			
			//must init to zero since C[M] is computed interatively.
			C[M] = 0;
			
			for (r=0; r< M; r++){
				//calculate projection coefficent alpha.
				alpha[M][r] = (D[M][r]) / (D[r][r]);
				
#ifdef DEBUG	
				std::cout << "alpha[" << M << "][" << r << "]=" << alpha[M][r] << "\n";
#endif
				
				D[M][r + 1] = 0;
				
				for (i = 0; i <= (r); i++){
					D[M][r + 1] -= alpha[r + 1][i] * D[M][i];
				}
				
				D[M][r + 1] += multiply_time_average(pm[M], pm[r + 1], num_instances);
				
				C[M] -= alpha[M][r] * C[r];
			}
			
			C[M] += cand->get_ytime_average();
			
#ifdef DEBUG
			std::cout << "C[M]=" << C[M] << "\n";
			std::cout << "D[" << M << "][" << M <<"]=" << D[M][M] << "\n";
#endif
			
			//this is true for m=0,...,M.
			gm[M] = C[M] / D[M][M];
			
#ifdef DEBUG
			std::cout << "G[" << M << "]=" << gm[M] << "\n"; 
#endif 
			
			if ((fabs(D[M][M]) > D_THRESHOLD) && (fabs(gm[M]) > G_THRESHOLD)){
				cand->set_q(gm[M] * gm[M] * D[M][M]);
			}
			else{
				cand->set_q(0);
			}
			
#ifdef DEBUG
			std::cout << "Q=" << cand->get_q() << "\n";
#endif
			
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
#ifdef DEBUG		
		std::cout << "Selecting candidate " << max->get_equ() << "\n";
#endif
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
#ifdef DEBUG
			std::cout << "Stop criteria met at M=" << M << ".\n";
			std::cout << "Q=" << max->get_q() << "\n";
			std::cout << "PMSE="<< pmse << "\n";
			std::cout << "dPMSE=" << (fabs(prev_pmse - pmse)) << "\n";
#endif
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
		
		model->add_coefficient(a);		
	}
	
	model->set_model_order(M);
	
	return model;
}


