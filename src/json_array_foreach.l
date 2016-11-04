%{

#include "json_array_foreach.flex.h"
#include "json_array_foreach.bison.h"



using namespace std;

%}



%%

\( 	return  ARRAY_OPEN;
\) 	return  ARRAY_CLOSE;

.|\n	;

%%


