#include <fstream>
#include <map>
#include "md5.hh"
#include "colsmd5.tab.h"
#include "stringsplitter.hh"
#include "stringutils.hh"

using namespace std;

class ColsMd5: public ColsMd5Base {
public:
	vector<unsigned int> colindexes0;
	StringSplitter splitter;
	StringUtils stringutils;
	
	virtual void processIstream(const char* f,std::istream& in) {
		string line;
		vector<string> tokens;
		while(getline(in,line))
			{
			splitter.split(line,tokens);
			
			MD5Sum hash;
			
			if(colindexes0.empty())
				{
				hash.update(line);
				}
			else
				{
				bool first=true;
				for(vector<unsigned int>::iterator ir=colindexes0.begin();ir != colindexes0.end();++ir)
					{
					if(!first) hash.update('\0');
					first=false;
					if( *ir <  tokens.size())
						{
						hash.update(tokens[*ir ]);
						}
					}
				}
				
			cout << hash.finalize() << this->delimiter << line << endl;
			} while(getline(in,line));
		
		}
	virtual int main(int argc,char** argv)
		{
		
		int optind = parseOptions(argc,argv);
		splitter.set_delimiter(this->delimiter);
		StringSplitter comma(',');
		set<int> seen_index;
		for(vector<string>::iterator r=this->columns.begin();r!=this->columns.end();++r)
			{
			vector<string> tokens= comma.split(*r);
			for(size_t i=0;i< tokens.size();++i)
				{
				string s = stringutils.trim(tokens[i]);
				if(s.empty()) continue;
				int col=atoi(s.c_str());
				if(col<=0) THROW("Bad column index: "<< (*r) );
				if( seen_index.find(col) !=  seen_index.end() )
					{
					THROW("column " << (col) << " defined twice");
					}
				seen_index.insert(col);
				colindexes0.push_back((unsigned int)(col - 1));
				}
			}
		
		manyOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	ColsMd5 app;
	return app.main(argc,argv);
	}
