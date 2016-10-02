#include <fstream>

#include <map>
#include "yxv2table.tab.h"
#include "aggregators.hh"
#include "stringsplitter.hh"

using namespace std;


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
	
	Aggregator* build()
		{
		switch(this->aggregator)
			{
			case E_aggregator_first :return new FirstAggregator();
			case E_aggregator_sum :return new SumAggregator();
			case E_aggregator_mean :return new MeanAggregator();
			case E_aggregator_min :return new MinAggregator();
			case E_aggregator_max :return new MaxAggregator();
			case E_aggregator_set :return new SetAggregator();
			case E_aggregator_median :return new MedianAggregator();
			default: return new SimpleAggregator();
			}
		}

	virtual void processIstream(const char* fname,istream& in)
		{
		std::set<std::string> x_headers;
		std::set<std::string> y_headers;
		std::map<pair_of_str_t,Aggregator*,cmpByStringLength> yxvalues;
		std::string line;
		std::vector<std::string> tokens;
		StringSplitter splitter;
		splitter.set_delimiter(this->delimiter);
		while(getline(in,line)) {
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
			cout << this->delimiter << (*rx);
			}
		cout << endl;
	
		for(std::set<std::string>::iterator ry= y_headers.begin(); ry!=y_headers.end();++ry)
			{
			cout << (*ry);
			for(std::set<std::string>::iterator rx= x_headers.begin(); rx!=x_headers.end();++rx)
				{
				pair_of_str_t k = make_pair(*ry,*rx);
				cout << this->delimiter ;//<< k.first+"/"+k.second<<":";
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
	YXV2Table app;
	return app.main(argc,argv);
	}
		

	
