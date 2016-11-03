%code {

	
	#include "json_array_foreach.flex.hh"
	#include "json_array_foreach.scanner.hh"
	#undef yylex
	#define yylex lexer.lex
}

%define parser_class_name {json_array_foreach_parser}
%parse-param { json_array_foreach_scanner& lexer }



%token ARRAY_OPEN
%token ARRAY_CLOSE
%start input
%%

input: array;

array: ARRAY_OPEN ARRAY_CLOSE;


%%

