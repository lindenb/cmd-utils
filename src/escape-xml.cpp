#include "escape-xml.tab.h"


using namespace std;

class EscapeXML: public EscapeXMLBase {
public:
	
	virtual void processFILE(const char* f,std::FILE* in) {

		int c;
		while((c=fgetc(in))!=EOF)
			{
			switch(c)
				{
				case '\'' : cout << "&apos;"; break;
				case '\"' : cout << "&quot;"; break;
				case '&' : cout << "&amp;"; break;
				case '<' : cout << "&lt;"; break;
				case '>' : cout << "&gt;"; break;
				default: cout << (char)c;break;
				}
			}

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
	EscapeXML app;
	return app.main(argc,argv);
	}
