#include <fstream>
#include <map>
#include "categorize.tab.h"
#include "stringsplitter.hh"
#include "stringutils.hh"

using namespace std;

class Categorize: public CategorizeBase {
public:
	vector<double> limits;
	StringSplitter splitter;
	StringUtils stringutils;
	
	virtual void processIstream(const char* f,std::istream& in) {
		string line;
		vector<string> tokens;
		unsigned int count_bad_value=0;
		unsigned int count_col_missing=0;
		unsigned int coldix=(unsigned int)(this->column_index -1 );
		while(getline(in,line))
			{
			splitter.split(line,tokens);
			
			if(tokens.size()<=coldix)
				{
				count_col_missing++;
				cout << line << endl;
				continue;
				}
			for(unsigned int i=0;i< tokens.size();++i)
				{
				if(i>0) cout <<  this->delimiter;
				if(i==coldix)
					{
					char* p2=NULL;
					double v=strtod(tokens[i].c_str(),&p2);
					if(*p2!=0) { cout << tokens[i];count_bad_value++; continue;}
					if(v< limits.front())
						{
						cout << "LT_" <<  limits.front();
						}
					else if(v> limits.back())
						{
						cout << "GT_" <<  limits.back();
						}
					else
						{
						for(unsigned int j=0;j+1< limits.size();++j)
							{
							if(v>=limits[j] && v< limits[j+1])
								{
								cout << "[" <<  limits[j] << "/" <<  limits[j+1] << "["; 
								break;
								}
							}
						}
					}
				else
					{
					cout << tokens[i];
					}
				}
			cout << endl;
			}
		if(count_col_missing++) {
			cerr << "[WARNING] number of lines with less than " << this->column_index << " columns : " << count_col_missing << endl;
			}
		if(count_bad_value++) {
			cerr << "[WARNING] number of lines with NaN in column [" << this->column_index << "] : " <<count_bad_value << endl;
			}
		
		}
	virtual int main(int argc,char** argv)
		{
		
		int optind = parseOptions(argc,argv);
		splitter.set_delimiter(this->delimiter);
		if(this->tranches==NULL)
			{
			cerr << "No tranche was defined" << endl;
			return EXIT_FAILURE;
			}
		if(this->column_index<1)
			{
			cerr << "Bad column index" << endl;
			return EXIT_FAILURE;
			}
		StringSplitter comma(',');
		vector<string> tokens= comma.split(this->tranches);
		
		
		for(vector<string>::iterator r= tokens.begin();r!=tokens.end();++r)
			{
			char* p2=NULL;
			double v=strtod(r->c_str(),&p2);
			if(*p2!=0) THROW("Not a double :" << (*r));
			if(!limits.empty() && limits.back()>=v)
				{
				THROW("Limits should be provided in increasing order.");
				}
			limits.push_back(v);
			}
		if(limits.empty())
			{
			cerr << "At least one tranche must be defined" << endl;
			return EXIT_FAILURE;
			}
		
		manyOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	Categorize app;
	return app.main(argc,argv);
	}
