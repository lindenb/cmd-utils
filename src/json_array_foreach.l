%{
#include "json_array_foreach.flex.h"
#define YY_DO_BEFORE_ACTION ((json_array_foreach_lexer*)::json_array_foreachget_extra(yyscanner))->do_before()
#define YY_NEW_FILE ((json_array_foreach_lexer*)::json_array_foreachget_extra(yyscanner))->do_new_file()
#define YY_INPUT(buf,result,max_size) result=((json_array_foreach_lexer*)::json_array_foreachget_extra(yyscanner))->read(buf,max_size)
#include "json_array_foreach.flex-m4.hh"



using namespace std;

%}

%option reentrant noyywrap

%%

\( 	return  1;
\) 	return  2;

.|\n	;

%%

int main(int argc,char** argv)
	{
	int i=0;
	json_array_foreach_lexer in(stdin);
	while( (i=in.lex()) >0) {
		cerr << i << endl;
		}
	cerr << i << endl;
	return 0;
	}
