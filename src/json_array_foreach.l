%{
#include "json_array_foreach.bison.h"
#include "json_array_foreach.flex.h"
#include "json_array_foreach.flex-m4.hh"

#ifdef xxxxxxxxxxxxxxxxxxxxxxxx
#define YY_DO_BEFORE_ACTION ((json_array_foreach_lexer*)::json_array_foreachget_extra(yyscanner))->do_before()
#define YY_NEW_FILE ((json_array_foreach_lexer*)::json_array_foreachget_extra(yyscanner))->do_new_file()
#define YY_INPUT(buf,result,max_size) result=((json_array_foreach_lexer*)::json_array_foreachget_extra(yyscanner))->read(buf,max_size)
#endif


#include "json_array_foreach.flex-m4.hh"



using namespace std;

%}

%option reentrant noyywrap bison-bridge

%%

\( 	return  ARRAY_OPEN;
\) 	return  ARRAY_CLOSE;

.|\n	;

%%


