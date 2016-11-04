%code {

	
	#include "json_array_foreach.flex.h"
	#include "json_array_foreach.m4.hh"
	#include "json_array_foreach.bison.h"

	
	
	#undef yylex
	#define yylex lexer.lex
}


%pure-parser
%locations 
%parse-param{json_array_foreachParser* parser}
%lex-param{void* handle_to_the_scanner}

%token ARRAY_OPEN
%token ARRAY_CLOSE
%start input
%%

input: array;

array: ARRAY_OPEN ARRAY_CLOSE;


%%

