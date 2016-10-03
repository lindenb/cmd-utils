#include "tab2spaces.tab.h"


using namespace std;


class Tab2Spaces : public Tab2SpacesBase
	{
	public:

	virtual void processFILE(const char* f,std::FILE* in) {
		int c=0;
		unsigned int x=0;
		while((c=fgetc(in))!=EOF)
			{
			switch(c)
				{
				case '\t':
					do {
						fputc(' ',stdout);
						++x;
						} while(x%(this->tabsize)!=0);
					break;
				case '\n':
					x=0 ;
					fputc(c,stdout);
					break;
				default:
					fputc(c,stdout);
					++x;
					break;
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
	Tab2Spaces app;
	return app.main(argc,argv);
	}
		

	
