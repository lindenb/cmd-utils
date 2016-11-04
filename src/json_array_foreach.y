%code {

	
	#include "json_array_foreach.flex.h"
	#include "json_array_foreach.bison.h"

}



%token ARRAY_OPEN
%token ARRAY_CLOSE
%start input
%%

input: array;

array: ARRAY_OPEN ARRAY_CLOSE;


%%

