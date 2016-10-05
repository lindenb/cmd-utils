#include "linearizefasta.tab.h"

using namespace std;


class LinearizeFasta : public LinearizeFastaBase
	{
	public:

	virtual void processFILE(const char* f,std::FILE* in) {
		bool at_begin=true;
		long N=0L;
		int c;
		while((c=fgetc(in))!=EOF)
			{
			switch(c)
				{
				case '>':
					{
					if(!at_begin) THROW("illegal '>' in file");
					if(N>0) fputc('\n',stdout);
					if( keepgt ) fputc('>',stdout);
					while((c=fgetc(in))!=EOF && c!='\n') {
						fputc(c,stdout);
						}
					fputc(this->delimiter,stdout);
					at_begin=true;
					N++;
					break;
					}
				case '\n': {
					at_begin=true;
					break;
					}
				default:
					{
					fputc(c,stdout);
					at_begin=false;
					break;
					}
				}
			}
		if(N>0) fputc('\n',stdout);
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
	LinearizeFasta app;
	return app.main(argc,argv);
	}
		

	
