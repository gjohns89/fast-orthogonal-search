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
#include "fos_simulation.h"

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

// Forward declarations
void CalcBulk();

//---------------------------------------------------------------------------
// Factory function for creating new parser variables
// This could as well be a function performing database queries.
value_type* AddVariable(const char_type *a_szName, void *a_pUserData)
{
	throw mu::ParserError(_T("Variable must be declared.\n"));
}

int IsHexValue(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal) 
{ 
	if (a_szExpr[1]==0 || (a_szExpr[0]!='0' || a_szExpr[1]!='x') ) 
		return 0;
	
	unsigned iVal(0);
	
	// New code based on streams for UNICODE compliance:
	stringstream_type::pos_type nPos(0);
	stringstream_type ss(a_szExpr + 2);
	ss >> std::hex >> iVal;
	nPos = ss.tellg();
	
	if (nPos==(stringstream_type::pos_type)0)
		return 1;
	
	*a_iPos += (int)(2 + nPos);
	*a_fVal = (value_type)iVal;
	
	return 1;
}

//---------------------------------------------------------------------------
value_type SelfTest()
{
	mu::console() << _T( "-----------------------------------------------------------\n");
	mu::console() << _T( "Running test suite:\n\n");
	
	// Skip the self test if the value type is set to an integer type.
	if (mu::TypeInfo<mu::value_type>::IsInteger())
	{
		mu::console() << _T( "  Test skipped: integer data type are not compatible with the unit test!\n\n");
	}
	else
	{
		mu::Test::ParserTester pt;
		pt.Run();
	}
	
	return 0;
}

//---------------------------------------------------------------------------
void ListVar(const mu::ParserBase &parser)
{
	// Query the used variables (must be done after calc)
	mu::varmap_type variables = parser.GetVar();
	if (!variables.size())
		return;
	
	cout << "\nParser variables:\n";
	cout <<   "-----------------\n";
	cout << "Number: " << (int)variables.size() << "\n";
	varmap_type::const_iterator item = variables.begin();
	for (; item!=variables.end(); ++item)
		mu::console() << _T("Name: ") << item->first << _T("   Address: [0x") << item->second << _T("]\n");
}

//---------------------------------------------------------------------------
void ListConst(const mu::ParserBase &parser)
{
	mu::console() << _T("\nParser constants:\n");
	mu::console() << _T("-----------------\n");
	
	mu::valmap_type cmap = parser.GetConst();
	if (!cmap.size())
	{
		mu::console() << _T("Expression does not contain constants\n");
	}
	else
	{
		valmap_type::const_iterator item = cmap.begin();
		for (; item!=cmap.end(); ++item)
			mu::console() << _T("  ") << item->first << _T(" =  ") << item->second << _T("\n");
	}
}

//---------------------------------------------------------------------------
void ListExprVar(const mu::ParserBase &parser)
{
	string_type sExpr = parser.GetExpr();
	if (sExpr.length()==0)
	{
		cout << _T("Expression string is empty\n");
		return;
	}
	
	// Query the used variables (must be done after calc)
	mu::console() << _T("\nExpression variables:\n");
	mu::console() <<   _T("---------------------\n");
	mu::console() << _T("Expression: ") << parser.GetExpr() << _T("\n");
	
	varmap_type variables = parser.GetUsedVar();
	if (!variables.size())
	{
		mu::console() << _T("Expression does not contain variables\n");
	}
	else
	{
		mu::console() << _T("Number: ") << (int)variables.size() << _T("\n");
		mu::varmap_type::const_iterator item = variables.begin();
		for (; item!=variables.end(); ++item)
			mu::console() << _T("Name: ") << item->first << _T("   Address: [0x") << item->second << _T("]\n");
	}
}

//---------------------------------------------------------------------------
void CalcBulk()
{
	/*const int nBulkSize = 200;
	value_type *x = new value_type[nBulkSize];
	value_type *y = new value_type[nBulkSize];
	value_type *result = new value_type[nBulkSize];
	
	try
	{
		for (int i=0; i<nBulkSize; ++i)
		{
			x[i] = i;
			y[i] = (value_type)i/10;
		}
		mu::Parser  parser;
		parser.DefineVar(_T("x"), x);
		parser.DefineVar(_T("y"), y);
		parser.DefineFun(_T("fun1"), BulkFun1);
		parser.SetExpr(_T("fun1(0)+x+y"));
		parser.Eval(result, nBulkSize);
		
		for (int i=0; i<nBulkSize; ++i)
		{
			mu::console() << _T("Eqn. ") << i << _T(": x=") << x[i] << _T("; y=") << y[i] << _T("; result=") << result[i] << _T("\n");
		}
	}
	catch(...)
	{
		delete [] x;
		delete [] y;
		delete [] result;
		throw;
	}
	
	delete [] x;
	delete [] y;
	delete [] result;*/
}


//---------------------------------------------------------------------------
void Calc()
{
	/*mu::Parser  parser;
	
	// Define the variable factory
	parser.SetVarFactory(AddVariable, &parser);
	
	for(;;)
	{
		try
		{
			string_type sLine;
			std::getline(mu::console_in(), sLine);
			
			switch (CheckKeywords(sLine.c_str(), parser))
			{
				case  0: break;
				case  1: continue;
				case -1: return;
			}
			
			if (!sLine.length())
				continue;
			
			parser.SetExpr(sLine);
			mu::console() << std::setprecision(12);
			
			// There are multiple ways to retrieve the result...
			// 1.) If you know there is only a single return value or in case you only need the last 
			//     result of an expression consisting of comma separated subexpressions you can 
			//     simply use: 
			mu::console() << _T("ans=") << parser.Eval() << _T("\n");
			
			// 2.) As an alternative you can also retrieve multiple return values using this API:
			int nNum = parser.GetNumResults();
			if (nNum>1)
			{
				mu::console() << _T("Multiple return values detected! Complete list:\n");
				
				// this is the hard way if you need to retrieve multiple subexpression
				// results
				value_type *v = parser.Eval(nNum);
				mu::console() << std::setprecision(12);
				for (int i=0; i<nNum; ++i)
				{
					mu::console() << v[i] << _T("\n");
				}
			}
		}
		catch(mu::Parser::exception_type &e)
		{
			mu::console() << _T("\nError:\n");
			mu::console() << _T("------\n");
			mu::console() << _T("Message:     ")   << e.GetMsg()   << _T("\n");
			mu::console() << _T("Expression:  \"") << e.GetExpr()  << _T("\"\n");
			mu::console() << _T("Token:       \"") << e.GetToken()    << _T("\"\n");
			mu::console() << _T("Position:    ")   << (int)e.GetPos() << _T("\n");
			mu::console() << _T("Errc:        ")   << std::dec << e.GetCode() << _T("\n");
		}
	} // while running*/
}

vector <string> read_equations(string fileName){
	
	vector <string> equations;
	string line;
	
	ifstream myfile (fileName.c_str());
	
	if (myfile.is_open()){
		while ( getline (myfile,line)){
			
			equations.push_back(line);
		}
		myfile.close();
	}
	else cout << "Unable to open file"; 
	
	return equations;
}

void doit(string fileName, string equations_path){
	int num_attributes, i;
	string sLine;
	ArffAttr* attr;
	const char* tmp;
	
	mu::Parser equ_parser;
	equ_parser.SetVarFactory(AddVariable, &equ_parser);
	vector <string> equations;
	
	//read the ARFF filename in.
	ArffParser arff_parser(fileName);
	ArffData* data = arff_parser.parse();
	
	std::cout << "File " << fileName << "loaded.\n";
	
	num_attributes = data->num_attributes();
	
	value_type x[num_attributes];
	
	for (i=0; i < num_attributes; i++){
		attr = data->get_attr(i);
		std::cout << "Adding variable: " << attr->name() << "\n";
		
		tmp = attr->name().c_str();
		
		equ_parser.DefineVar(_T(tmp), &x[i]);
		x[i] = 0;
	}
	
	std::cout << "\n\nLoading equations from " << equations_path << ".\n";
	
	equations = read_equations(equations_path);
	
	for (i=0; i<equations.size(); i++){
		sLine = equations.at(i);
		
		std::cout << "Equ: " << sLine << "\n";
		equ_parser.SetExpr(sLine);
		
		std::cout << sLine << "=" << equ_parser.Eval() << "\n";
	}
	/*x[0] = 0;
	x[1] = 1;
	string_type sLine = "sepallength+sepalwidth";
	equ_parser.SetExpr(sLine);
	std::cout << sLine << "=" << equ_parser.Eval() << "\n";*/
	
	
}



//---------------------------------------------------------------------------
int main(int, char**)
{
	
	string train_path = "/Users/greggjohns/Documents/C projects/csv/case4.arff";
	string equ_path = "/Users/greggjohns/Documents/C projects/csv/equations.txt";
	
	FosSimulation sim("force", equ_path, train_path, train_path, train_path);
		
	FosModel *model = sim.train(0);
	
	std::cout << model->get_model_equation() << "\n";
		
	//doit(fileName, equations_path);
	
	/*//setup the parser.
	mu::Parser parser;
	parser.SetVarFactory(AddVariable, &parser);
	
	
	
	int i;
	int n_attributes = 64;
	int max_attribute_length = 100;
	
	char_type attr_name[n_attributes][max_attribute_length];
	
	value_type x[64];
	
	for (i=0; i< n_attributes; i++){
		sprintf(attr_name[i], "ch%d", i);
		parser.DefineVar(_T(attr_name[i]), &x[i]);
	}
	
	try{
		string_type sLine = "ch1=5";
		parser.SetExpr(sLine);
		std::cout << sLine << "=" << parser.Eval() << "\n";
		
		sLine = "ch1+5";
		parser.SetExpr(sLine);
		std::cout << sLine << "=" << parser.Eval() << "\n";
	
		
	}
	catch(Parser::exception_type &e)
	{
		// Only erros raised during the initialization will end up here
		// formula related errors are treated in Calc()
		console() << _T("Initialization error:  ") << e.GetMsg() << endl;
		console() << _T("aborting...") << endl;
		string_type sBuf;
		console_in() >> sBuf;
	}
	catch(std::exception & )
	{
		// there is no unicode compliant way to query exc.what()
		// so i'll leave it for this example.
		console() << _T("aborting...\n");
	}*/
	
	return 0;
}
