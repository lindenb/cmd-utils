#include "json_array_foreach.tab.h"
#include "json_array_foreach.bison.h"
#include "json_array_foreach.flex.h"

using namespace std;

extern int json_array_foreachparse();

class JsonArrayForEach: public JsonArrayForEachBase {
public:

	virtual void processFILE(const char* f,std::FILE* in) {
		json_array_foreachin = in;
		json_array_foreachparse();
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		fOneOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	JsonArrayForEach app;
	
	app.main(argc,argv); 
		
	}
