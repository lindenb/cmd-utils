#include <fstream>
#include <map>
#include "cutbyname.tab.h"
#include "stringsplitter.hh"

using namespace std;

class CutByNames: public CutByNamesBase {
public:
	StringSplitter splitter;
	
	void run(const char* fname,istream& in)
		{
		string line;
		if(!getline(in,line))
			{
			cerr << "Cannot read the first line." <<  endl;
			exit(EXIT_FAILURE);
			}
		std::vector<std::string> header=splitter.split(line);
		map<string,size_t> header2col;
		std::set<size_t> offsets;
		
		for(size_t i=0;i< header.size();++i)
			{
			set<string>::iterator r= this->columns.find(header.at(i));
			if(r==this->columns.end()) continue;
			if( header2col.find(header.at(i))!= header2col.end())
				{
				cerr << "Duplicate column " << header.at(i) <<" in header";
				exit(EXIT_FAILURE);
				}
			header2col.insert(make_pair(header.at(i),i));
			offsets.insert(i);
			}
		
		for(set<string>::iterator r=this->columns.begin();r!=this->columns.end();++r)
			{
			if( header2col.find(*r) != header2col.end() ) continue;
			cerr << "Error: Cannot find "<< (*r) << " in header." << endl;
			if(this->abort_if_missing)
				{
				exit(EXIT_FAILURE);
				}
			}
		
		vector<string> tokens;
		do
			{
			splitter.split(line,tokens);
			bool first=true;
			
			if( this-> inverse )
				{
				for(size_t i=0;i< tokens.size();++i)
					{
					if(offsets.find(i)!=offsets.end()) continue;
					if(!first) cout << this->delimiter; 
					first=false;
					cout << tokens[i];
					}
				}
			else
				{
				for(set<size_t>::iterator ir=offsets.begin();ir != offsets.end();++ir)
					{
					if(!first) cout << this->delimiter; 
					first=false;
					if( *ir >=  tokens.size())
						{
						cerr << "Error: not enough columns in  "<< line << endl;
						if(this->abort_if_missing)
							{
							exit(EXIT_FAILURE);
							}
						}
					else
						{
					
						cout << tokens[*ir];
						}
					}
				}
			cout << endl;
			} while(getline(in,line));
		
		}
	int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		splitter.set_delimiter(this->delimiter);
		if(optind==argc)
			{
			run(0,cin);
			}
		else if(optind+1==argc)
			{
			ifstream f(argv[optind]);
			if(!f.is_open()) {
				cerr << "Cannot open " << argv[optind] << ": "
					<< strerror(errno) << "." << endl;
				return EXIT_FAILURE;
				}
			run(argv[optind],f);
			f.close();
			}
		else
			{
			cerr << "Illegal Number of arguments"<< endl;
			return EXIT_FAILURE;
			}
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	CutByNames app;
	return app.main(argc,argv);
	}
