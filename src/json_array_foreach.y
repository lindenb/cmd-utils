%{ 
#include <string>
#include "throw.hh"
#include "json_array_foreach.flex.h"

using namespace std;

static void yyerror(const char* msg)
	{
	cerr << "ERROR " << msg << endl;
	exit(EXIT_FAILURE);
	}
static int json_depth=0;

%}

%union {
	std::string* str;
	}

%token ARRAY_OPEN
%token ARRAY_CLOSE
%token OBJECT_OPEN
%token OBJECT_CLOSE
%token<str> PRIMITIVE_STRING  PRIMITIVE_NUMBER PRIMITIVE_TRUE PRIMITIVE_FALSE
%token PRIMITIVE_NULL
%token COMMA COLON
%start input
%%

input: array;

array: ARRAY_OPEN  ARRAY_CLOSE
	{
	if(json_depth!=0)
		{
		cout << "[]";
		}
	}
	| ARRAY_OPEN {if(json_depth!=0) {cout << "[";} json_depth++;} array_items ARRAY_CLOSE { json_depth--;if(json_depth==0) { cout << endl;} else {cout << "]";} }
	;
array_items:  array_items COMMA {if(json_depth==1) {cout << endl;} else {cout << ",";}} any | any ;
object: OBJECT_OPEN OBJECT_CLOSE { cout << "{}";} | OBJECT_OPEN { cout << "{";} object_items OBJECT_CLOSE { cout << "}";};
object_items: object_pair |  object_pair COMMA  { cout << ",";} object_items;
object_pair: prim_string  COLON { cout << ":";} any;
any: prim_string | PRIMITIVE_NULL { cout << "null";} | PRIMITIVE_NUMBER { cout << *($1); delete $1;} | PRIMITIVE_TRUE { cout << "true";}  | PRIMITIVE_FALSE { cout << "false";} | array | object;
prim_string: PRIMITIVE_STRING { cout << *($1); delete $1;}


%%

