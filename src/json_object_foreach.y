%{ 
#include <string>
#include "throw.hh"
#include "json_object_foreach.flex.h"

using namespace std;

static void yyerror(const char* msg)
	{
	cerr << "ERROR " << msg << endl;
	exit(EXIT_FAILURE);
	}
static int json_depth=0;
char field_delimiter='\t';

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

input: object;

array: ARRAY_OPEN  ARRAY_CLOSE { cout << "[]"; }
	| ARRAY_OPEN {cout << "[";} array_items ARRAY_CLOSE {cout << "]";} 
	;
array_items: array_items COMMA {cout << ",";} any | any ;
object: OBJECT_OPEN OBJECT_CLOSE
	{
	if(json_depth!=0) cout << "{}";
	}
	|OBJECT_OPEN { if(json_depth!=0) cout << "{";json_depth++;} object_items OBJECT_CLOSE { if(json_depth!=1) cout << "}";json_depth--;}
	;
object_items: object_pair |  object_pair COMMA  { if(json_depth!=1) { cout << ",";} } object_items;
object_pair: prim_string COLON { if(json_depth==1) {cout << field_delimiter;} else {cout << ":";} } any {if(json_depth==1) { cout << endl;}};
any: prim_string  | PRIMITIVE_NULL { cout << "null";} | PRIMITIVE_NUMBER { cout << *($1); delete $1;} | PRIMITIVE_TRUE { cout << "true";}  | PRIMITIVE_FALSE { cout << "false";} | array | object;
prim_string: PRIMITIVE_STRING { cout << *($1); delete $1;}


%%

