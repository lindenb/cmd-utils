#include "escape-c.tab.h"


using namespace std;

class EscapeC: public EscapeCBase {
public:

	virtual void processFILE(const char* f,std::FILE* in) {
		bool at_begin = true;
		int c;
		while((c=fgetc(in))!=EOF)
			{
			if( at_begin) cout << prefix;
			at_begin=false;
			switch(c)
				{
				case '\'' : cout << "\\\'"; break;
				case '\"' : cout << "\\\""; break;
				case '\\' : cout << "\\\\"; break;
				case '\t' : cout << "\\t"; break;
				case '\n': at_begin = true; cout << suffix << endl; break;
				default: cout << (char)c;break;
				}
			}
		if(!at_begin) cout << suffix ;
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		fManyOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	EscapeC app;
	return app.main(argc,argv);
	}
