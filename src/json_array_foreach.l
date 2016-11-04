%{
#include <string>
#include "throw.hh"
#include "json_array_foreach.bison.h"

using namespace std;




%}


DIGIT1to9 [1-9]
DIGIT [0-9]
DIGITS {DIGIT}+
INT {DIGIT}|{DIGIT1to9}{DIGITS}|-{DIGIT}|-{DIGIT1to9}{DIGITS}
FRAC [.]{DIGITS}
EXP {E}{DIGITS}
E [eE][+-]?
HEX_DIGIT [0-9a-f]
NUMBER {INT}|{INT}{FRAC}|{INT}{EXP}|{INT}{FRAC}{EXP}
UNESCAPEDCHAR [ -!#-\[\]-~]
ESCAPEDCHAR \\["\\bfnrt/]
UNICODECHAR \\u{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}
CHAR {UNESCAPEDCHAR}|{ESCAPEDCHAR}|{UNICODECHAR}
CHARS {CHAR}+
DBL_QUOTE ["]
%%
{DBL_QUOTE}{DBL_QUOTE}|{DBL_QUOTE}{CHARS}{DBL_QUOTE} {
	yylval.str=new string(yytext);
	return PRIMITIVE_STRING;
	}
{NUMBER} {
    yylval.str=new string(yytext);
    return PRIMITIVE_NUMBER;
	}
\[ 	return  ARRAY_OPEN;
\] 	return  ARRAY_CLOSE;
\{ 	return  OBJECT_OPEN;
\} 	return  OBJECT_CLOSE;
, 	return COMMA;
\:	return COLON;
true return PRIMITIVE_TRUE;
false return PRIMITIVE_FALSE;
null return PRIMITIVE_NULL;
[ \t\n]+  ;
.	{
	THROW("unexpected char " << yytext << ".\n");
	}

%%

int yywrap()
{
   return 1;
}



