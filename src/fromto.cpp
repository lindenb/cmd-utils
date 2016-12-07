#include "regexp.hh"
#include "fromto.tab.h"


using namespace std;

class FromTo: public FromToBase {
public:
	RegExp* reg_begin;
	bool incl_begin;
	RegExp* reg_end;
	bool incl_end;
	
	FromTo():reg_begin(0),incl_begin(false),
		reg_end(0),incl_end(false) {
		}
	
	virtual void processIstream(const char* f,std::istream& in) {
		string line;
		bool is_in=(reg_begin==0?true:false);
		
		while(getline(in,line)) {
			if(!is_in)
				{
				if( reg_begin!=0 && reg_begin->test(line.c_str()))
					{
					if(incl_begin)
						{
						cout << line << endl;
						}
					is_in=true;
					}
				}
			else  if( reg_end!=0 && reg_end->test(line.c_str()))
				{
				if(incl_end)
					{
					cout << line << endl;
					}
				is_in=false;
				if(reg_begin!=0 && !multiple) break;
				}
			else
				{
				cout << line << endl;
				}
			}
			
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		
		
		if(from_str!=0 && after_str!=0)
			{
			cerr << "Option from and after cannot be both defined" << endl;
			return EXIT_FAILURE;
			}
		else if( from_str!=0) {
			reg_begin = new RegExp(from_str);
			incl_begin = true;
			}
		else if(after_str!=0 ) {
			reg_begin = new RegExp(after_str);
			incl_begin = false;
			}
		
		
		if(to_str!=0 && before_str!=0)
			{
			cerr << "Option to and before cannot be both defined" << endl;
			return EXIT_FAILURE;
			}
		else if( to_str!=0) {
			reg_end = new RegExp(to_str);
			incl_end = true;
			}
		else if(before_str!=0 ) {
			reg_end = new RegExp(before_str);
			incl_end = false;
			}
		
		manyOrStdin(optind,argc,argv);
		
		if( reg_begin!=0) delete reg_begin;
		reg_begin=0;
		if( reg_end!=0) delete reg_end;
		reg_end=0;
		
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	FromTo app;
	return app.main(argc,argv);
	}
