#include <list>
#include "csv2tsv.tab.h"


using namespace std;

#define SKIP_COMMENT_LINE if(in.peek()==this->commchar) {\
	for(;;) {\
		c= in.get();\
		cout << (char)c;\
		if(c=='\n') { ++nlines;break;}\
		}}

class CsvToTsv: public CsvToTsvBase {
public:
	
	class Reader
		{
		public:
			FILE* f;
			int next;
			Reader(FILE* f):f(f),next(-1)
				{
				
				}
			int _read() {
				if(f==0) return -1;
				int c=-1;
				for(;;) {
					c=fgetc(f);
					if(c=='\r') continue;
					if(c==-1) {
						f=0;
						return -1;
						}
					break;
					}
				return c;
				}
			int peek() {
				if(next!=-1) return next;
				next = _read();
				return next;	
				}
			
			int get() {
				if(next!=-1) {
					int c=next;
					next=-1;
					return c;
					}
				return _read();
				}
			
			
		};
	CsvToTsv() {
		}
	
	bool is_quote(int c)
		{
		return c==this->quotechar;
		}
	
	virtual void processFILE(const char* f,std::FILE* is) {
		long nlines=1L;
		Reader in(is);
		int c;
		SKIP_COMMENT_LINE
		for(;;)
			{
			c=in.get();
			if(c==-1) break;
			if(c=='\n') {
				cout << endl;
				++nlines;
				SKIP_COMMENT_LINE
				continue;
				}
			if(is_quote(c)) 
				{
				int quote=c;
				for(;;)
					{
					c=in.get();
					if(c==-1 || c=='\n') {
						cerr << "[WARNING] Quote opened but not closed line "<< nlines  << endl;
						if(c=='\n') SKIP_COMMENT_LINE
						continue;
						}
					if(c==quote) break;
					if(c=='\\' )
						{
						switch(in.peek())
							{
							case '\\': case '\'': case '\"' : cout << (char) in.get(); break;
							case 't': cout << "\\t"; in.get(); break;
							case 'n': cout << "\\n"; in.get();break;
							default: cerr << "How can I convert \\" << (char)c << "?" << endl;cout << (char)in.get();break;
							}
						}
					else
						{
						cout << (char)c;
						}
					
					}
				while(isspace(c=in.peek()))
					{
					in.get();
					}
				}
			else if(c==this->csvdelim)
				{
				cout << '\t';
				}
			else
				{
				cout << (char)c;
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
	CsvToTsv app;
	return app.main(argc,argv);
	}
