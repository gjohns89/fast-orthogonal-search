#include "muParserTest.h"

#if defined(_WIN32) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define CREATE_LEAKAGE_REPORT
#endif

#if defined( USINGDLL ) && defined( _WIN32 )
#error This sample can be used only with STATIC builds of muParser (on win32)
#endif

/** \brief This macro will enable mathematical constants like M_PI. */
#define _USE_MATH_DEFINES		

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <iostream>
#include <locale>
#include <limits>
#include <ios> 
#include <iomanip>
#include <numeric>
#include <fstream>

#include "muParser.h"
#include "arff_parser.h"
#include "arff_attr.h"
#include "fos_regressor.h"

#include "fos_spectrum.h"

using namespace std;
using namespace mu;


#if defined(CREATE_LEAKAGE_REPORT)

// Dumping memory leaks in the destructor of the static guard
// guarantees i won't get false positives from the ParserErrorMsg 
// class wich is a singleton with a static instance.
struct DumpLeaks
{
	~DumpLeaks()
	{
		_CrtDumpMemoryLeaks();
	}
} static LeakDumper;

#endif

void get_paths(){
	int session, trial;
	int n_sessions = 4, n_trials =4;
	
	//session x trial path
	string M000 [4][4];
	string M001 [4][4];
	string M002 [4][4];
	string F000 [4][4];
	string F001 [4][4];	

	for (session=0; session<n_sessions; session++){
		for (trial=0; trial<n_trials; trial++){
			M000[session][trial] = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/case4.arff";
			M001[session][trial] = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/case4.arff";
			M002[session][trial] = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/case4.arff";
			F000[session][trial] = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/case4.arff";
			F001[session][trial] = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/case4.arff";
		}
	}
}


//---------------------------------------------------------------------------
int main(int, char**)
{
	vector <string> test_paths;
	//FosModel *model;
	
	//actual model equation is force = exp(input) + sin(input);
	string train_path = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/case4.arff";
	string equ_path = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/equations.txt";
	string save = "/Users/greggjohns/Documents/C projects/fast-orthogonal-search/models/test_model";
	
	FosSpectralAnalyzer(500, 0.1, 1024);
	
	return 0;
}
