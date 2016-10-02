#include <fstream>
#include <algorithm>
#include <map>
#include "table2yxv.tab.h"
#include "stringsplitter.hh"

using namespace std;


class Table2XYV: public Table2YXVBase
	{
	public:
		

		virtual void processIstream(const char* fname,istream& in)
			{
			StringSplitter splitter;
			splitter.set_delimiter(this->delimiter);
			string line;
			if(!getline(in,line)) THROW("cannot read first line");
			vector<string> header = splitter.split(line);
			
			vector<string> tokens;
			while(getline(in,line)) {
				splitter.split(line,tokens);

				for(int i= this->lastycolumn ; i< (int)header.size()  && i< (int)tokens.size();++i)
					{
					for(int j=0;j< this->lastycolumn && j< (int)tokens.size();++j)
						{
						if(j>0) cout << this->delimiter;
						cout << tokens[j];
						}
					cout << this->delimiter;
					cout << header[i];
					cout << this->delimiter;
					cout << tokens[i];
					cout << endl;
					}
				}
			}
	
		int main(int argc,char** argv)
			{
			int optind = parseOptions(argc,argv);
			oneOrStdin(optind,argc,argv);
			return EXIT_SUCCESS;
			}

	};


int main(int argc,char** argv)
	{
	Table2XYV app;
	return app.main(argc,argv);
	}
		

	
