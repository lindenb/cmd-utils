#include "escape-http.tab.h"
#include "stringutils.hh"

using namespace std;

class EscapeHttp: public EscapeHttpBase {
private:
	StringUtils utils;
public:
	
char x2c(int c0,int c1)
	{
    	int digit;
    
    	digit = (c0 >= 'A' ? ((c0 & 0xdf) - 'A')+10 : (c0 - '0'));
    	digit *= 16;
    	digit += (c1 >= 'A' ? ((c1 & 0xdf) - 'A')+10 : (c1 - '0'));
    	return (char)digit;
	}

	virtual void processFILE(const char* f,std::FILE* in) {

		int c1;
		while((c1=fgetc(in))!=EOF)
			{
			if(c1==' ') { cout << "+" ; continue;}
			if(!isalnum(c1)) { 
				char tmp[10];
				sprintf(tmp,"%x", c1);
				cout << "%" << tmp ;
				continue;
				}
			if(c1!='%') { cout << (char)c1; continue;}
			int c2 = fgetc(in);
			if(c2==-1 ) { cout << '%'; break;}
			if(!utils.isHex(c2)) { cout << "%" << (char)c2;continue;}
			int c3 = fgetc(in);
			if(c3==-1) { cout << "%" << (char)c2;break;}
			if(!utils.isHex(c3)) { cout << "%" << (char)c2 << (char)c3;continue;}			
			cout << x2c(c2,c3);
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
	EscapeHttp app;
	return app.main(argc,argv);
	}
