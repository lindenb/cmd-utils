#include <set>
#include <map>
#include "yxv2table.tab.h"
#include "stringsplitter.hh"

using namespace std;

class Aggregator
	{
	public:
		virtual ~Aggregator(){}
		virtual void visit(const std::string& s)=0;
		virtual void print(std::ostream& out)=0;
	};



typedef std::pair<std::string,std::string> pair_of_str_t;

struct cmpByStringLength {
    bool operator()(const pair_of_str_t& a, const pair_of_str_t& b) const {
         if(  a.first < b.first ) return true;
         if(  a.first > b.first ) return false;
         return a.second < b.second ;
    	}
};




class YXV2Table: public YXV2TableBase
	{
	public:
	
	class SimpleAggregator: public Aggregator
		{
		public:
			std::string value;
			virtual ~SimpleAggregator(){}
			virtual void visit(const std::string& s) { value.assign(s);};
			virtual void print(std::ostream& out) { out << this->value; }
		};
	
	Aggregator* build()
		{
		return new SimpleAggregator();
		}

	int main(int argc,char** argv)
		{
		std::set<std::string> x_headers;
		std::set<std::string> y_headers;
		std::map<pair_of_str_t,Aggregator*,cmpByStringLength> yxvalues;
		std::string line;
		std::vector<std::string> tokens;
		StringSplitter splitter;
		while(getline(cin,line)) {
			splitter.split(line,tokens);
			pair_of_str_t key = make_pair(tokens[0],tokens[1]);
			std::map<pair_of_str_t,Aggregator*>::iterator rm = yxvalues.find(key);
			Aggregator* ag;
			if( rm == yxvalues.end() )
				{
				ag = build();
				x_headers.insert(tokens[1]);
				y_headers.insert(tokens[0]);
				yxvalues.insert(make_pair(key,ag));
				}
			else
				{
				ag = rm->second;
				}
			ag->visit(tokens[2]);
			}
		
		cout << this->firstheader;
		for(std::set<std::string>::iterator rx= x_headers.begin(); rx!=x_headers.end();++rx)
			{
			cout << "\t" << (*rx);
			}
		cout << endl;
	
		for(std::set<std::string>::iterator ry= y_headers.begin(); ry!=y_headers.end();++ry)
			{
			cout << (*ry);
			for(std::set<std::string>::iterator rx= x_headers.begin(); rx!=x_headers.end();++rx)
				{
				pair_of_str_t k = make_pair(*ry,*rx);
				cout << "\t" ;//<< k.first+"/"+k.second<<":";
				std::map<pair_of_str_t,Aggregator*>::iterator rm = yxvalues.find(k);
				if( rm == yxvalues.end()) {
					cout << this->nullvalue;
					}
				else
					{
					rm->second->print(cout);
					delete rm->second;
					}
			
				}
			cout << endl;
			}
	
		return 0;
		}
	};


int main(int argc,char** argv)
	{
	YXV2Table app;
	return app.main(argc,argv);
	}
		

	
