#include "json_object_foreach.tab.h"
#include "json_object_foreach.bison.h"
#include "json_object_foreach.flex.h"

using namespace std;

class JsonObjectForEach: public JsonObjectForEachBase {
public:

	virtual void processFILE(const char* f,std::FILE* in) {
		if(this->print_header)
			{
			cout << "#key" << this->delimiter << "value" << endl;
			}
		yyin = in;
		::yyparse();
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		extern char field_delimiter;
		field_delimiter = this->delimiter;
		
		fOneOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	JsonObjectForEach app;
	
	app.main(argc,argv); 
		
	}
